/**
 * Définition de la classe SoftSpotLight, un type de lampe plus sophistiqué
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <SoftSpotLight.h>
#include <SceneBase.h>


/**
 * constructeur
 * @param anglemax : angle en degrés d'ouverture total de la lampe
 * @param anglemin : angle dans lequel l'intensité est 100%, il y a un dégradé entre anglemin et anglemax
 * @param lightradius : largeur de la source de lumière en unités à rapporter à far et à la taille de la shadow map
 * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
 * @param near : distance la plus proche dans la shadowmap
 * @param far : distance la plus lointaine dans la shadowmap
 * @param pcss : algorithme employé pour le calcul du flou des ombres true:PCSS, false:PCF
 * @param offsetfill : true si la shadow map doit activer le décalage de polygones
 * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
 */
SoftSpotLight::SoftSpotLight(float anglemax, float anglemin, float lightradius, int shadowmapsize, float near, float far, bool pcss, bool offsetfill, GLenum cullface):
    SpotLight(anglemax, anglemin, shadowmapsize, near, far, offsetfill, cullface)
{
    init(lightradius, pcss, offsetfill, cullface);
}
SoftSpotLight::SoftSpotLight(float anglemax, float anglemin, float lightradius, int shadowmapsize, float near, float far, bool pcss):
    SpotLight(anglemax, anglemin, shadowmapsize, near, far)
{
    init(lightradius, pcss, false, GL_FRONT);
}
SoftSpotLight::SoftSpotLight(float anglemax, float anglemin, float lightradius, int shadowmapsize, float near, float far):
    SpotLight(anglemax, anglemin, shadowmapsize, near, far)
{
    init(lightradius, true, false, GL_FRONT);
}

/**
 * initialisations communes à tous les constructeurs
 * @param lightradius : largeur de la source de lumière en unités à rapporter à far et à la taille de la shadow map
 * @param pcss : algorithme employé pour le calcul du flou des ombres true:PCSS, false:PCF
 * @param offsetfill : true si la shadow map doit activer le décalage de polygones
 * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
 */
void SoftSpotLight::init(float lightradius, bool pcss, bool offsetfill, GLenum cullface)
{
    // initialisation des variables membre (voir aussi le constructeur de SpotLight)
    m_Name = "SoftSpotLight";
    m_TanMaxAngle = 2.0 * tan(m_MaxAngle * 0.5);
    m_LightRadius = lightradius;
    m_PCSS = pcss;

    // configurer la shadow map
    m_ShadowMap->setAcneReduction(offsetfill, cullface);

    // compiler le shader
    compileShader();
}


/**
 * destructeur
 */
SoftSpotLight::~SoftSpotLight()
{
}


/**
 * Cette méthode définit l'étendue de la lampe
 * @param lightradius : float donnant la largeur de la lampe
 */
void SoftSpotLight::setRadius(float lightradius)
{
    m_LightRadius = lightradius;
}


/**
 * retourne l'étendue de la lampe
 * @return étendue
 */
float SoftSpotLight::getRadius()
{
    return m_LightRadius;
}


/**
 * construit le Fragment Shader qui calcule l'éclairement de cette lampe
 * @return source du shader
 */
std::string SoftSpotLight::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.precision(5);
    srcFragmentShader << std::fixed;
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
    srcFragmentShader << "\n";
    srcFragmentShader << "uniform vec3 LightColor;\n";
    srcFragmentShader << "uniform vec4 LightPosition;\n";
    srcFragmentShader << "uniform float cosmaxangle;\n";
    srcFragmentShader << "uniform float cosminangle;\n";
    srcFragmentShader << "uniform vec3 LightDirection;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// échantillonnage de Poisson\n";
    srcFragmentShader << "const int PoissonCount = 16;\n";
    srcFragmentShader << "uniform vec2 PoissonSamples[PoissonCount];\n";
    srcFragmentShader << "mat2 RandomRotation;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "highp float rand(vec2 co)\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    highp float a  = 12.9898;\n";
    srcFragmentShader << "    highp float b  = 78.233;\n";
    srcFragmentShader << "    highp float c  = 43758.5453;\n";
    srcFragmentShader << "    highp float dt = dot(co ,vec2(a,b));\n";
    srcFragmentShader << "    highp float sn = mod(dt, 3.141592);\n";
    srcFragmentShader << "    return fract(sin(sn) * c);\n";
    srcFragmentShader << "}\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// moduler l'éclairement d'un point selon son écart à l'axe du spot\n";
    srcFragmentShader << "float inLightBeam(vec3 L)\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // écart entre l'axe central de la lumière et le point considéré\n";
    srcFragmentShader << "    return smoothstep(cosmaxangle, cosminangle, dot(L, LightDirection));\n";
    srcFragmentShader << "}\n";
    if (m_ShadowMap != nullptr) {
        srcFragmentShader << "\n";
        srcFragmentShader << "// calcul de la distance réelle correspondant à depth\n";
        srcFragmentShader << "float depth2distance(float depth)\n";
        srcFragmentShader << "{\n";
        srcFragmentShader << "    if (depth == 1.0) return 1e38;\n";
        srcFragmentShader << "    return ("<<m_Near<<" * "<<m_Far<<") / ("<<m_Far<<" - depth * ("<<m_Far<<" - "<<m_Near<<"));\n";
        srcFragmentShader << "}\n";
        srcFragmentShader << "\n// shadow map et matrice de retour pour cette lampe\n";
        srcFragmentShader << "uniform sampler2D ShadowMap;\n";
        srcFragmentShader << "uniform mat4 mat4Shadow;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "// retourne (1, 0) si le point est éclairé, (0, d) s'il est dans l'ombre d'un objet situé à la distance d\n";
        srcFragmentShader << "vec2 isIlluminated1(vec3 posshadow)\n";
        srcFragmentShader << "{\n";
        srcFragmentShader << "    // distance entre ce fragment et la lumière\n";
        srcFragmentShader << "    float distancePointLight = posshadow.z;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "    // comparer la valeur donnée par la ShadowMap avec la distance du fragment à la lumière\n";
        srcFragmentShader << "    float distanceObstacleLight = texture(ShadowMap, posshadow.xy).r;\n";
        srcFragmentShader << "    if (distanceObstacleLight < distancePointLight) {\n";
        srcFragmentShader << "        // un objet opaque est entre nous et la lumière\n";
        srcFragmentShader << "        float distance = depth2distance(distanceObstacleLight);\n";
        srcFragmentShader << "        return vec2(0.0, distance);\n";
        srcFragmentShader << "    } else {\n";
        srcFragmentShader << "        // pas d'ombre\n";
        srcFragmentShader << "        return vec2(1.0, 0.0);\n";
        srcFragmentShader << "    }\n";
        srcFragmentShader << "}\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "// retourne une valeur entre 1.0 (point totalement éclairé) et 0.0 (entièrement dans l'ombre)\n";
        srcFragmentShader << "float isIlluminatedN(vec3 posshadow, float radius)\n";
        srcFragmentShader << "{\n";
        srcFragmentShader << "    // tester la shadow map en différents endroits proches du point courant\n";
        srcFragmentShader << "    float visibility = 0.0;\n";
        srcFragmentShader << "    for (int i=0; i<PoissonCount; i++) {\n";
        srcFragmentShader << "        vec2 offset = RandomRotation * PoissonSamples[i] * radius;\n";
        srcFragmentShader << "        visibility += isIlluminated1(posshadow + vec3(offset, 0.0)).s;\n";
        srcFragmentShader << "    }\n";
        srcFragmentShader << "    return visibility / float(PoissonCount);\n";
        srcFragmentShader << "}\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "// largeur de la source de lumière\n";
        srcFragmentShader << "uniform float LightRadius;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "// retourne une valeur entre 1.0 (point totalement éclairé) et 0.0 (entièrement dans l'ombre)\n";
        srcFragmentShader << "float isIlluminated(vec4 position)\n";
        srcFragmentShader << "{\n";
        srcFragmentShader << "    // calculer les coordonnées du vertex dans la shadow map\n";
        srcFragmentShader << "    vec4 posshadow = mat4Shadow * position;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "    // normaliser les coordonnées homogènes\n";
        srcFragmentShader << "    posshadow /= posshadow.w;\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "    // distance réelle entre ce fragment et la lumière\n";
        srcFragmentShader << "    float realDistanceToLight = depth2distance(posshadow.z);\n";
        srcFragmentShader << "\n";
        srcFragmentShader << "    // rayon maximal de l'ombre portée en ce point\n";
        srcFragmentShader << "    float shadowradius = LightRadius / realDistanceToLight;\n";
        srcFragmentShader << "\n";
        if (m_PCSS) {
            srcFragmentShader << "    // explorer le voisinage à la recherche d'occulteurs et de leur distance moyenne\n";
            srcFragmentShader << "    vec2 nb_dist = vec2(0.0, 0.0); // vec2(s=nombre, t=distance)\n";
            srcFragmentShader << "    for (int i=0; i<6; i++) {\n";
            srcFragmentShader << "        vec2 offset = RandomRotation * PoissonSamples[i] * shadowradius * 0.25;\n";
            srcFragmentShader << "        vec2 visib = isIlluminated1(posshadow.xyz + vec3(offset,0.0));\n";
            srcFragmentShader << "        nb_dist += vec2(1.0-visib.x, visib.y);\n";
            srcFragmentShader << "    }\n";
            srcFragmentShader << "    if (nb_dist.x > 0.0) {\n";
            srcFragmentShader << "        // valeur moyenne de la distance des occulteurs\n";
            srcFragmentShader << "        float distance = nb_dist.y / nb_dist.x;\n";
            srcFragmentShader << "\n";
            srcFragmentShader << "        // calculer le rayon de l'ombre portée sur ce point\n";
            srcFragmentShader << "        shadowradius *= clamp(4.0*(realDistanceToLight - distance) / distance, 0.0, 1.0);\n";
            srcFragmentShader << "\n";
            srcFragmentShader << "        // calculer l'ombre à partir de ce rayon de pénombre\n";
            srcFragmentShader << "        return isIlluminatedN(posshadow.xyz, shadowradius);\n";
            srcFragmentShader << "    } else {\n";
            srcFragmentShader << "        // aucun occulteur dans le champ, aucune ombre\n";
            srcFragmentShader << "        return 1.0;\n";
            srcFragmentShader << "    }\n";
        } else {
            srcFragmentShader << "    return isIlluminatedN(posshadow.xyz, shadowradius);\n";
        }
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
    srcFragmentShader << "        // initialiser l'échantillonnage aléatoire de Poisson\n";
    srcFragmentShader << "        float angle = rand(frgTexCoord*gl_FragDepth) * 6.283285;\n";
    srcFragmentShader << "        float s = sin(angle);\n";
    srcFragmentShader << "        float c = cos(angle);\n";
    srcFragmentShader << "        RandomRotation = mat2(c, s, -s, c);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // dans la zone éclairée ?\n";
    srcFragmentShader << "        float visibility = inLightBeam(L);\n";
    if (m_ShadowMap != nullptr) {
        srcFragmentShader << "        visibility *= isIlluminated(position);\n";
    }
    srcFragmentShader << "        if (visibility > 0.0) {\n";
    srcFragmentShader << "            // direction de la normale et produit scalaire\n";
    srcFragmentShader << "            vec3 N  = normal.xyz;\n";
    srcFragmentShader << "            float dotNL = clamp(dot(N,L), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
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
void SoftSpotLight::findUniformLocations()
{
    // obtenir les emplacements de la superclasse
    SpotLight::findUniformLocations();

    // emplacement des variables uniform du shader
    m_LightRadiusLoc = glGetUniformLocation(m_ShaderId, "LightRadius");

    // initialiser le tableau des constantes (erreur : arrays may not be declared constant since they cannot be initialized)
    GLint PoissonLoc = glGetUniformLocation(m_ShaderId, "PoissonSamples");
    glUseProgram(m_ShaderId);
    GLfloat PoissonSamples[] = {
        +0.79636, -0.56051,
        -0.64373, +0.68863,
        -0.00856, +0.93684,
        -0.26651, -0.82735,
        -0.81180, +0.29899,
        -0.58342, -0.62513,
        -0.78033, -0.28858,
        +0.43087, +0.70461,
        +0.71023, +0.34784,
        +0.70937, -0.22050,
        +0.28912, -0.65419,
        -0.44501, +0.31930,
        -0.47742, -0.03139,
        -0.21586, -0.39196,
        +0.26554, -0.22434,
        +0.02771, +0.26856,
    };
    glUniform2fv(PoissonLoc, 16, PoissonSamples);
    glUseProgram(0);
}


/**
 * active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe
 * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
 */
void SoftSpotLight::startProcess(FrameBufferObject* gbuffer)
{
    // appeler la méthode de la superclasse
    SpotLight::startProcess(gbuffer);

    // étendue relative de la lampe
    glUniform1f(m_LightRadiusLoc, m_LightRadius / m_TanMaxAngle);

    // axe de la lampe spot = position-target
    vec3::glUniform(m_DirectionLoc, m_Direction);
}
