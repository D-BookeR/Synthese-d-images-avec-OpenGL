#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <DisplacementMaterial.h>

/**
 * Constructeur
 */
DisplacementMaterial::DisplacementMaterial() : Material("DisplacementMaterial")
{
    // charger les textures
    m_DiffuseTexture = new Texture2D("data/textures/13302/diffuse.jpg", GL_LINEAR, GL_REPEAT);
    m_DiffuseTextureLoc = -1;
    m_NormalTexture = new Texture2D("data/textures/13302/normal.jpg", GL_LINEAR, GL_REPEAT);
    m_NormalTextureLoc = -1;
    m_ElevationTexture = new Texture2D("data/textures/13302/height.jpg", GL_LINEAR, GL_REPEAT);
    m_ElevationTextureLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string DisplacementMaterial::getVertexShader()
{
    return
        "#version 300 es\n"
        "// matrices de changement de repère\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "uniform mat3 mat3Normal;\n"
        "\n"
        "// VBOs contenant le repère de Frenet complet\n"
        "in vec3 glVertex;\n"
        "in vec3 glNormal;\n"
        "in vec3 glTangent;\n"
        "in vec2 glTexCoord;\n"
        "\n"
        "// variables à interpoler pour le fragment shader\n"
        "out vec4 frgPosition;\n"
        "out vec3 frgNormal;\n"
        "out vec3 frgTangent;\n"
        "out vec2 frgTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    frgNormal = mat3Normal * glNormal;\n"
        "    frgTangent = mat3Normal * glTangent;\n"
        "    frgTexCoord = glTexCoord;\n"
        "}";
}


/**
 * retourne le source du Fragment Shader
 */
std::string DisplacementMaterial::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "\n"
        "// variables interpolées venant du vertex shader\n"
        "in vec4 frgPosition;\n"
        "in vec3 frgNormal;\n"
        "in vec3 frgTangent;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// textures diffuse, normale et hauteur\n"
        "uniform sampler2D txDiffuse;\n"
        "uniform sampler2D txNormal;\n"
        "uniform sampler2D txHeight;\n"
        "\n"
        "// grandeur de l'effet de déplacement\n"
        "const float relative_height = 0.05;\n"
        "\n"
        "// nombre d'itérations de la recherche d'intersection\n"
        "const int steps_phase1 = 12;\n"
        "const int steps_phase2 = 8;\n"
        "\n"
        "/** calcul de l'intersection entre la vue et la surface\n"
        " * @param V : vec3 donnant l'axe de la vue au niveau de P\n"
        " * @param P : vec2 donnant les coordonnées de texture du point à la surface\n"
        " * @return vec2 donnant les coordonnées de texture de R correspondant au point effectivement vu\n"
        " */\n"
        "vec2 find_contact(vec3 V, vec2 P)\n"
        "{\n"
        "    // incrément pour parcourir la texture\n"
        "    vec3 increment = vec3(V.xy/V.z * (-relative_height), -1.0) / float(steps_phase1);\n"
        "\n"
        "    // positions parcourues successivement, écrites dans le repère de Frenet\n"
        "    vec3 position = vec3(P, 1.0);\n"
        "    vec3 position_prec;\n"
        "    vec3 position_high = position;\n"
        "    vec3 position_low = position;\n"
        "\n"
        "    // passe à 1.0 dès qu'il y a un contact avec la surface\n"
        "    float ground_reached = 0.0;\n"
        "    \n"
        "    // recherche grossière du point de contact\n"
        "    for (int i=0; i<steps_phase1; i++) {\n"
        "        position_prec = position;\n"
        "        position += increment;\n"
        "        // altitude du fond au niveau du point courant\n"
        "        float ground_level = texture(txHeight, position.xy).r;\n"
        "        // cette hauteur dépasse-t-elle la position courante ?\n"
        "        float first_contact = step(position.z + ground_reached*100.0, ground_level);\n"
        "        // mettre à jour les hauteurs haute et basse si c'est le premier contact\n"
        "        position_high = mix(position_high, position_prec, first_contact);\n"
        "        position_low  = mix(position_low, position,       first_contact);\n"
        "        ground_reached += first_contact;\n"
        "    }\n"
        "    // cas d'un contact non rencontré (ground_reached valait 0.0 au dernier tour)\n"
        "    position_high = mix(position_prec, position_high, ground_reached);\n"
        "    position_low  = mix(position,      position_low, ground_reached);\n"
        "    //return (position_high.xy + position_low.xy)/2.0;\n"
        "    //return position_high.xy;\n"
        "    \n"
        "    // recherche fine du point de contact par dichotomie\n"
        "    for (int i=0; i<steps_phase2; i++) {\n"
        "        // point milieu entre position_high et position_low\n"
        "        position = (position_low + position_high) * 0.5;\n"
        "        // altitude du fond au niveau du point courant\n"
        "        float ground_level = texture(txHeight, position.xy).r;\n"
        "        // plus haut ou plus bas ?\n"
        "        float position_too_low = step(position.z, ground_level);\n"
        "        position_low  = mix(position_low, position,  position_too_low);\n"
        "        position_high = mix(position, position_high, position_too_low);\n"
        "    }\n"
        "    \n"
        "    // dernier milieu\n"
        "    position = (position_low + position_high) * 0.5;\n"
        "    return position.xy;\n"
        "}\n"
        "\n"
        "//// fonction manquante dans cette version de GLSL\n"
        "//mat3 transpose(in mat3 inMatrix)\n"
        "//{\n"
        "//    vec3 i0 = inMatrix[0];  // 1e colonne\n"
        "//    vec3 i1 = inMatrix[1];  // 2e colonne\n"
        "//    vec3 i2 = inMatrix[2];  // 3e colonne\n"
        "//    return mat3(\n"
        "//        vec3(i0.x, i1.x, i2.x),\n"
        "//        vec3(i0.y, i1.y, i2.y),\n"
        "//        vec3(i0.z, i1.z, i2.z));\n"
        "//}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // matrices de passage entre les espaces tangent et global\n"
        "    vec3 N = normalize(frgNormal);\n"
        "    vec3 T = normalize(frgTangent);\n"
        "    vec3 B = cross(N, T);\n"
        "    mat3 TBN = mat3(T, B, N);\n"
        "    mat3 TBNinv = transpose(TBN);\n"
        "    \n"
        "    // recherche du point de contact exact entre la vue et la surface\n"
        "    vec3 Vue = normalize(TBNinv * frgPosition.xyz);\n"
        "    vec2 newTexCoord = find_contact(Vue, frgTexCoord);\n"
        "    \n"
        "    // vecteur normal dans le repère de Frenet\n"
        "    vec3 Nfrenet = texture(txNormal, newTexCoord).xyz * 2.0 - 1.0;\n"
        "    \n"
        "    // changement de repère du vecteur normal extrait de la normal map\n"
        "    N = normalize(TBN * Nfrenet);\n"
        "    \n"
        "    // caractéristiques locales du matériau\n"
        "    vec3 Kd = 0.2+texture(txDiffuse, newTexCoord).rgb;\n"
        "    vec3 Ks = vec3(0.3, 0.3, 0.3);\n"
        "    float Ns = 20.0;\n"
        "    \n"
        "    // direction et couleur de la lumière\n"
        "    vec3 LightDirection = vec3(0.5, 0.7, 0.7);\n"
        "    vec3 LightColor = vec3(1.5, 1.5, 1.5);\n"
        "    \n"
        "    // contribution diffuse de Lambert\n"
        "    vec3 L = normalize(LightDirection);\n"
        "    float dotNL = clamp(dot(N, L), 0.0, 1.0);\n"
        "    \n"
        "    // contribution spéculaire de Phong\n"
        "    vec3 R = reflect(normalize(frgPosition.xyz), N);\n"
        "    float dotRL = clamp(dot(R, L), 0.0, 1.0);\n"
        "    \n"
        "    // couleur finale\n"
        "    glFragColor = vec4(LightColor*(Kd*dotNL + Ks*pow(dotRL, Ns)), 1.0);\n"
        "}";
}


/**
 * compile ou recompile le shader
 */
void DisplacementMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_DiffuseTextureLoc   = glGetUniformLocation(m_ShaderId, "txDiffuse");
    m_NormalTextureLoc   = glGetUniformLocation(m_ShaderId, "txNormal");
    m_ElevationTextureLoc  = glGetUniformLocation(m_ShaderId, "txHeight");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* DisplacementMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL,   Utils::VEC3, "glNormal");
    vboset->addAttribute(MeshVertex::ID_ATTR_TANGENT,  Utils::VEC3, "glTangent");
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void DisplacementMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer les textures chacune sur une unité différente
    m_DiffuseTexture->setTextureUnit(GL_TEXTURE0, m_DiffuseTextureLoc);
    m_NormalTexture->setTextureUnit(GL_TEXTURE1, m_NormalTextureLoc);
    m_ElevationTexture->setTextureUnit(GL_TEXTURE2, m_ElevationTextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void DisplacementMaterial::disable()
{
    // désactiver les textures
    m_DiffuseTexture->setTextureUnit(GL_TEXTURE0);
    m_NormalTexture->setTextureUnit(GL_TEXTURE1);
    m_ElevationTexture->setTextureUnit(GL_TEXTURE2);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
DisplacementMaterial::~DisplacementMaterial()
{
    delete m_ElevationTexture;
    delete m_NormalTexture;
    delete m_DiffuseTexture;
}
