/**
 * Définition de la classe SpotLight, un type de lampe plus sophistiqué
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <SpotLight.h>
#include <SceneBase.h>


/**
 * constructeur
 * @param maxangle : angle en degrés d'ouverture total de la lampe
 * @param minangle : angle dans lequel l'intensité est 100%, il y a un dégradé entre minangle et maxangle
 * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
 * @param near : distance la plus proche dans la shadowmap
 * @param far : distance la plus lointaine dans la shadowmap
 * @param offsetfill : true si la shadow map doit activer le décalage de polygones
 * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
 */
SpotLight::SpotLight(float maxangle, float minangle, int shadowmapsize, float near, float far, bool offsetfill, GLenum cullface):
    OmniLight()
{
    init(maxangle, minangle, shadowmapsize, near, far, offsetfill, cullface);
}
SpotLight::SpotLight(float maxangle, float minangle, int shadowmapsize, float near, float far):
    OmniLight()
{
    init(maxangle, minangle, shadowmapsize, near, far, true, GL_BACK);
}
SpotLight::SpotLight(float maxangle, float minangle):
    OmniLight()
{
    init(maxangle, minangle, 0, 1.0f, 100.0f, true, GL_BACK);
}

/**
 * initialisations communes à tous les constructeurs
 * @param maxangle : angle en degrés d'ouverture total de la lampe
 * @param minangle : angle dans lequel l'intensité est 100%, il y a un dégradé entre minangle et maxangle
 * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
 * @param near : distance la plus proche dans la shadowmap
 * @param far : distance la plus lointaine dans la shadowmap
 * @param offsetfill : true si la shadow map doit activer le décalage de polygones
 * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
 */
void SpotLight::init(float maxangle, float minangle, int shadowmapsize, float near, float far, bool offsetfill, GLenum cullface)
{
    // initialisation des variables membre
    m_Name = "SpotLight";
    if (maxangle >= 170.0) throw std::invalid_argument("Max angle of spot light is too big, >=170°");
    if (minangle > maxangle) minangle = maxangle;
    m_MaxAngle = Utils::radians(maxangle);
    m_CosMaxAngle = cos(m_MaxAngle * 0.5);
    m_MinAngle = Utils::radians(minangle);
    m_CosMinAngle = cos(m_MinAngle * 0.5);
    m_Direction = vec3::create();
    m_TargetScene = vec4::create();
    m_TargetCamera = vec4::create();

    // créer une shadow map si c'est demandé en paramètres
    if (shadowmapsize > 0) {
        m_ShadowMap = new ShadowMap(shadowmapsize, offsetfill, cullface);
        m_Near = near;
        m_Far = far;
    } else {
        m_ShadowMap = nullptr;
        m_Near = 0.1;
        m_Far = 100.0;
    }

    // matrice d'ombre
    m_ShadowMatrix = mat4::create();

    // compiler le shader
    compileShader();
}


/**
 * destructeur
 */
SpotLight::~SpotLight()
{
    if (m_ShadowMap != nullptr) delete m_ShadowMap;
}


/**
 * Cette méthode définit la position visée par la lampe
 * @param target : vec4 indiquant le point ou l'axe que vise la lampe
 * NB: l'une des deux informations position et cible doivent être des points
 */
void SpotLight::setTarget(vec4 target)
{
    m_TargetScene = vec4::clone(target);
    m_TargetCamera = vec4::clone(target);
}


/**
 * applique une matrice sur la position de la lampe afin d'obtenir la position caméra
 * @param matrix : mat4
 */
void SpotLight::transformPosition(mat4& matrix)
{
    // calculer les coordonnées dans le repère caméra
    OmniLight::transformPosition(matrix);

    // forcer la coordonnée w à 1
    m_PositionCamera[3] = 1.0;

    // recalculer la direction de la lampe
    vec3::subtract(m_Direction, vec3::fromVec(m_PositionCamera), vec3::fromVec(m_TargetCamera));
    vec3::normalize(m_Direction, m_Direction);
}


/**
 * applique une matrice sur la cible de la lampe afin d'obtenir la position caméra
 * @param matrix : mat4
 */
void SpotLight::transformTarget(mat4& matrix)
{
    // calculer les coordonnées dans le repère caméra
    vec4::transformMat4(m_TargetCamera, m_TargetScene, matrix);

    // forcer la coordonnée w à 1
    m_TargetCamera[3] = 1.0;

    // recalculer la direction de la lampe
    vec3::subtract(m_Direction, vec3::fromVec(m_PositionCamera), vec3::fromVec(m_TargetCamera));
    vec3::normalize(m_Direction, m_Direction);
}


/**
 * applique une matrice sur la position et la cible de la lampe
 * @param matrix : mat4
 */
void SpotLight::transform(mat4& matrix)
{
    transformPosition(matrix);
    transformTarget(matrix);
}


/**
 * dessine la scène dans la shadowmap de cette lampe
 * @param scene à dessiner vue de la lampe this
 * @param mat4ViewCamera : matrice de transformation dans laquelle sont dessinés les objets
 */
void SpotLight::makeShadowMap(SceneBase* scene, mat4& mat4ViewCamera)
{
    // s'il n'y a pas de shadowmap, alors sortir sans rien faire
    if (m_ShadowMap == nullptr) return;

    // construire une matrice de projection à partir de la lampe
    // TODO changer le type de perspective selon la nature de position et target (directions ou positions)
    mat4 mat4LightProjection = mat4::create();
    mat4::perspective(mat4LightProjection, m_MaxAngle, 1.0, m_Near, m_Far);

    // construire une matrice de vue à partir de la lampe
    mat4 mat4LightView = mat4::create();
    mat4::lookAt(mat4LightView, vec3::fromVec(m_PositionCamera), vec3::fromVec(m_TargetCamera), vec3::fromValues(0,1,0));

    // appliquer la matrice de la scène
    mat4::multiply(mat4LightView, mat4LightView, mat4ViewCamera);

    // calculer la matrice d'ombre
    mat4::invert(m_ShadowMatrix, mat4ViewCamera);
    mat4::multiply(m_ShadowMatrix, mat4LightView, m_ShadowMatrix);
    mat4::multiply(m_ShadowMatrix, mat4LightProjection, m_ShadowMatrix);
    mat4::multiply(m_ShadowMatrix, ShadowMap::c_MatBias, m_ShadowMatrix);

    // passer en mode dessin dans la shadowmap
    m_ShadowMap->enable();

    // dessiner la scène sans aucun matériau
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->onDraw(mat4LightProjection, mat4LightView);

    // revenir en mode dessin normal
    m_ShadowMap->disable();
}


/**
 * construit le Fragment Shader qui calcule l'éclairement de cette lampe
 * @return source du fragment shader
 */
std::string SpotLight::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "uniform sampler2D MapPosition;\n";
    srcFragmentShader << "uniform sampler2D MapNormale;\n";
    srcFragmentShader << "uniform sampler2D MapDiffuse;\n";
    srcFragmentShader << "uniform sampler2D MapSpecular;\n";
    srcFragmentShader << "uniform sampler2D MapDepth;\n";
    srcFragmentShader << "uniform vec3 LightColor;\n";
    srcFragmentShader << "uniform vec4 LightPosition;\n";
    srcFragmentShader << "uniform float cosmaxangle;\n";
    srcFragmentShader << "uniform float cosminangle;\n";
    srcFragmentShader << "uniform vec3 LightDirection;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// moduler l'éclairement d'un point selon son écart à l'axe du spot\n";
    srcFragmentShader << "float inLightBeam(vec3 L)\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // écart entre l'axe central de la lumière et le point considéré\n";
    srcFragmentShader << "    return smoothstep(cosmaxangle, cosminangle, dot(L, LightDirection));\n";
    srcFragmentShader << "}\n";
    if (m_ShadowMap != nullptr) {
        srcFragmentShader << "\n";
        srcFragmentShader << "\n// shadow map et matrice de retour pour cette lampe\n";
        srcFragmentShader << "uniform sampler2D ShadowMap;\n";
        srcFragmentShader << "uniform mat4 mat4Shadow;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "// retourne 1.0 si le point est éclairé, 0.0 s'il est dans l'ombre\n";
        srcFragmentShader << "float isIlluminated(vec4 position)\n";
        srcFragmentShader << "{\n";
        srcFragmentShader << "    // calculer les coordonnées du vertex dans la shadow map\n";
        srcFragmentShader << "    vec4 posshadow = mat4Shadow * position;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "    // normaliser les coordonnées homogènes\n";
        srcFragmentShader << "    posshadow /= posshadow.w;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "    // distance entre ce fragment et la lumière\n";
        srcFragmentShader << "    float distancePointLight = posshadow.z;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "    // comparer la valeur donnée par la ShadowMap avec la distance du fragment à la lumière\n";
        srcFragmentShader << "    float distanceObstacleLight = texture(ShadowMap, posshadow.xy).r;\n";
        srcFragmentShader << "    //if (distanceObstacleLight < distancePointLight) {\n";
        srcFragmentShader << "    //    // un objet opaque est entre nous et la lumière\n";
        srcFragmentShader << "    //    return 0.0;\n";
        srcFragmentShader << "    //} else {\n";
        srcFragmentShader << "    //    // pas d'ombre\n";
        srcFragmentShader << "    //    return 1.0;\n";
        srcFragmentShader << "    //}\n";
        srcFragmentShader << "    return step(distancePointLight, distanceObstacleLight);\n";
        srcFragmentShader << "}\n";
    }
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // récupérer les infos du g-buffer\n";
    srcFragmentShader << "    vec4 position = texture(MapPosition, frgTexCoord);\n";
    srcFragmentShader << "    if (position.w != 1.0) discard;\n";
    srcFragmentShader << "    gl_FragDepth = texture(MapDepth, frgTexCoord).r;\n";
    srcFragmentShader << "    vec4 normal = texture(MapNormale, frgTexCoord);\n";
    srcFragmentShader << "    vec4 Kd = texture(MapDiffuse, frgTexCoord);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    if (normal.w != 0.0) {\n";
    srcFragmentShader << "        // éclairement diffus uniquement\n";
    srcFragmentShader << "        glFragColor = vec4(LightColor * Kd.rgb, 1.0) * Kd.a;\n";
    srcFragmentShader << "    } else {\n";
    srcFragmentShader << "        // calculer la direction et l'intensité de la lampe\n";
    srcFragmentShader << "        vec3 L;\n";
    srcFragmentShader << "        vec3 color;\n";
    srcFragmentShader << "        if (LightPosition.w != 0.0) {\n";
    srcFragmentShader << "            // positionnelle\n";
    srcFragmentShader << "            L = LightPosition.xyz - position.xyz;\n";
    srcFragmentShader << "            float distance = length(L);\n";
    srcFragmentShader << "            // diviser la couleur par la distance au carré\n";
    srcFragmentShader << "            color = LightColor / (distance*distance);\n";
    srcFragmentShader << "            L = L / distance;\n";
    srcFragmentShader << "        } else {\n";
    srcFragmentShader << "            // directionnelle\n";
    srcFragmentShader << "            L = normalize(LightPosition.xyz);\n";
    srcFragmentShader << "            color = LightColor;\n";
    srcFragmentShader << "        }\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // dans la zone éclairée ?\n";
    srcFragmentShader << "        float visibility = inLightBeam(L);\n";
    if (m_ShadowMap != nullptr) {
        srcFragmentShader << "        if (visibility > 0.0) visibility *= isIlluminated(position);\n";
    }
    srcFragmentShader << "        if (visibility > 0.0) {\n";
    srcFragmentShader << "            // direction de la normale et produit scalaire\n";
    srcFragmentShader << "            vec3 N = normal.xyz;\n";
    srcFragmentShader << "            float dotNL = clamp(dot(N,L), 0.0, 1.0);\n";
    srcFragmentShader << "            // couleur spéculaire et coefficient ns\n";
    srcFragmentShader << "            vec4 Ks = texture(MapSpecular, frgTexCoord);\n";
    srcFragmentShader << "            float ns = Ks.a;\n";
    srcFragmentShader << "            if (ns > 0.0) {\n";
    srcFragmentShader << "                // reflet spéculaire\n";
    srcFragmentShader << "                vec3 R = reflect(normalize(position.xyz), N);\n";
    srcFragmentShader << "                float dotRL = clamp(dot(R,L), 0.0, 1.0);\n";
    srcFragmentShader << "                // éclairement diffus et reflet spéculaire\n";
    srcFragmentShader << "                glFragColor = vec4(visibility * color * (Kd.rgb*dotNL + Ks.rgb*pow(dotRL, ns)), 1.0) * Kd.a;\n";
    srcFragmentShader << "            } else {\n";
    srcFragmentShader << "                // éclairement diffus sans reflet spéculaire\n";
    srcFragmentShader << "                glFragColor = vec4(visibility * color * (Kd.rgb*dotNL), 1.0) * Kd.a;\n";
    srcFragmentShader << "            }\n";
    srcFragmentShader << "        } else {\n";
    srcFragmentShader << "            // le point est dans l'ombre\n";
    srcFragmentShader << "            glFragColor = vec4(0.0, 0.0, 0.0, Kd.a);\n";
    srcFragmentShader << "        }\n";
    srcFragmentShader << "    }\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * détermine où sont les variables uniform spécifiques de cette lampe
 */
void SpotLight::findUniformLocations()
{
    // obtenir les emplacements de la superclasse
    OmniLight::findUniformLocations();

    // emplacement des variables uniform du shader
    m_ShadowMapLoc    = glGetUniformLocation(m_ShaderId, "ShadowMap");
    m_ShadowMatrixLoc = glGetUniformLocation(m_ShaderId, "mat4Shadow");
    m_CosMaxAngleLoc  = glGetUniformLocation(m_ShaderId, "cosmaxangle");
    m_CosMinAngleLoc  = glGetUniformLocation(m_ShaderId, "cosminangle");
    m_DirectionLoc    = glGetUniformLocation(m_ShaderId, "LightDirection");
}


/**
 * active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe
 * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
 */
void SpotLight::startProcess(FrameBufferObject* gbuffer)
{
    // appeler la méthode de la superclasse
    OmniLight::startProcess(gbuffer);

    // angles de la lampe spot
    glUniform1f(m_CosMaxAngleLoc, m_CosMaxAngle);
    glUniform1f(m_CosMinAngleLoc, m_CosMinAngle);

    // axe de la lampe spot = position-target
    vec3::glUniform(m_DirectionLoc, m_Direction);

    if (m_ShadowMap != nullptr) {
        // associer la shadowmap à l'unité 5
        m_ShadowMap->setTextureUnit(GL_TEXTURE5, m_ShadowMapLoc);
        mat4::glUniformMatrix(m_ShadowMatrixLoc, m_ShadowMatrix);
    }
}


/**
 * désactive les ressources utilisées par cette lampe
 */
void SpotLight::endProcess()
{
    // libérer l'unité de texture
    if (m_ShadowMap != nullptr) {
        // associer la shadowmap à l'unité 5
        m_ShadowMap->setTextureUnit(GL_TEXTURE5);
    }

    // appeler la méthode de la superclasse
    OmniLight::endProcess();
}
