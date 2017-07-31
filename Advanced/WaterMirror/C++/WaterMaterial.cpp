// Cette classe permet de combiner réflexion et réfraction selon WaterMaterial
// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
// http://graphicsrunner.blogspot.fr/2010/08/water-using-flow-maps.html

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <WaterMaterial.h>

/**
 * Cette classe réalise la fusion du reflet et de la vue sous-surface.
 * Elle contient plusieurs constantes qui correspondent aux textures et objets employés
 */

/**
 * Constructeur
 */
WaterMaterial::WaterMaterial():
    Material("WaterMaterial")
{
    // texture donnant la phase des vaguelettes
    m_MapPhase = new Texture2D("data/textures/eau/phase.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

    // FBO fournissant le reflet et le position_ground
    m_FBOreflection = nullptr;
    m_FBObackground = nullptr;

    // allocation d'une matrice temporaire
    m_Mat3Normal = mat3::create();
    m_ViewPort = vec2::create();

    // compiler le shader
    compileShader();
}


/**
 * fournit les deux g-buffer utilisés par ce matériau
 * @param FBOreflection : FBO contenant deux color buffers : couleur diffuse et position du reflet
 * @param FBObackground : FBO contenant deux color buffers : couleur diffuse et position du fond
 * @param width : largeur du viewport
 * @param height : hauteur du viewport
 */
void WaterMaterial::setGBuffers(FrameBufferObject* FBOreflection, FrameBufferObject* FBObackground, int width, int height)
{
    m_FBOreflection = FBOreflection;
    m_FBObackground = FBObackground;
    vec2::set(m_ViewPort, width, height);
}


/**
 * retourne le source du Vertex Shader
 */
std::string WaterMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "\n"
        "// paramètres uniform\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "\n"
        "// attributs de sommets\n"
        "in vec3 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "\n"
        "// interpolation vers les fragments\n"
        "out vec4 frgPosition;\n"
        "out vec2 frgTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    frgTexCoord = glTexCoord * 0.2;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string WaterMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec4 frgPosition;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragData[4];\n"
        "\n"
        "// informations venant du g-buffer\n"
        "uniform mat3 mat3Normal;\n"
        "uniform vec2 viewport;\n"
        "uniform sampler2D MapIslandPosition;\n"
        "uniform sampler2D MapDepth;\n"
        "\n"
        "// informations venant des dessins de la scène\n"
        "uniform sampler2D MapReflexion;\n"
        "uniform sampler2D MapRefraction;\n"
        "\n"
        "// texture aléatoire\n"
        "uniform sampler2D MapPhase;\n"
        "\n"
        "// temps\n"
        "uniform float Time;\n"
        "\n"
        "// divers\n"
        "const float twopi = 6.283185307;\n"
        "\n"
        "/// rapport des indices air/eau par couleur\n"
        "const vec3 eta = vec3(0.76, 0.75, 0.71);\n"
        "\n"
        "/// exposant du produit scalaire de Schlick, normalement c'est 5.0\n"
        "const float SchlickExponent = 3.0;\n"
        "\n"
        "/// coefficient multiplicatif pour augmenter la profondeur\n"
        "const float DepthFactor = 0.5;\n"
        "\n"
        "/// amplitude (hauteur) des vagues\n"
        "const float WavesAmplitude = 0.2;\n"
        "\n"
        "/// distance minimale pour avoir la taille maximale des vagues\n"
        "const float MinDistance = 8.0;\n"
        "\n"
        "/// fréquence spatiale (écartement) des vagues\n"
        "const float WavesSpatFreq = 8.0;\n"
        "\n"
        "/// fréquence temporelle (vitesse) des vagues\n"
        "const float WavesSpeed = 1.0;\n"
        "\n"
        "/// importance des vagues sur les textures\n"
        "const float WavesReflexionStrength = 0.2;\n"
        "const float WavesRefractionStrength = 0.08;\n"
        "\n"
        "/// coefficients d'absorption du reflet et de la réfraction\n"
        "const float ReflexionAttenuation = 0.6;\n"
        "const float RefractionAttenuation = 0.6;\n"
        "\n"
        "/// couleur du ciel = couleur de l'eau en profondeur infinie\n"
        "const vec3 SkyColor = vec3(0.7,0.9,1.0);\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // coordonnées écran [0, 1]\n"
        "    vec2 screenposition = gl_FragCoord.xy / viewport;\n"
        "\n"
        "    // coordonnées caméra du point de contact avec le fond\n"
        "    vec4 position_ground  = texture(MapIslandPosition, screenposition);\n"
        "\n"
        "    // coordonnées caméra du point de contact avec l'eau\n"
        "    vec4 position_surface  = frgPosition;\n"
        "\n"
        "    // distance traversée dans l'eau à cet endroit\n"
        "    float distance = length(position_surface.xyz - position_ground.xyz) * DepthFactor;\n"
        "\n"
        "    // vecteur vue inversé (il va vers l'oeil)\n"
        "    vec3 V = -normalize(position_ground.xyz);\n"
        "\n"
        "    // perturbation par les vaguelettes stationnaires, l'importance décroit avec la distance\n"
        "    vec2 phase = texture(MapPhase, frgTexCoord * WavesSpatFreq).xy;\n"
        "    vec2 offset = WavesAmplitude * sin(twopi*(Time*WavesSpeed + phase)) / (MinDistance-position_surface.z);\n"
        "\n"
        "    // calculer les coordonnées caméra de la normale\n"
        "    vec3 normal = mat3Normal * vec3(offset.x, 1.0, offset.y);\n"
        "    vec3 N = normalize(normal);\n"
        "    float dotVN = max(dot(V,N), 0.0);\n"
        "\n"
        "    // calcul du coefficient de Fresnel par l'approximation de Schlick\n"
        "    const vec3 R0 = ((1.0-eta)*(1.0-eta)) / ((1.0+eta)*(1.0+eta));\n"
        "    float powdotVN = pow(1.0-dotVN, SchlickExponent);\n"
        "    vec3 fresnel = clamp(R0 + (1.0-R0) * powdotVN, 0.0, 1.0);\n"
        "\n"
        "    // couleurs données par le reflet et la réfraction\n"
        "    vec3 reflexion  = texture(MapReflexion,  screenposition + offset*WavesReflexionStrength).rgb * ReflexionAttenuation;\n"
        "    vec3 refraction = texture(MapRefraction, screenposition + offset*WavesRefractionStrength).rgb;\n"
        "\n"
        "    // la couleur est attenuée par la distance parcourue dans l'eau\n"
        "    refraction = mix(refraction, SkyColor, distance) * RefractionAttenuation;\n"
        "\n"
        "    // mélange entre réfraction et réflexion\n"
        "    vec3 Kd = mix(refraction, reflexion, fresnel);\n"
        "\n"
        "    // couleur résultante\n"
        "    glFragData[0] = vec4(Kd, 1.0);\n"
        "    glFragData[1] = vec4(1.0, 1.0, 1.0, 1024.0);\n"
        "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n"
        "    glFragData[3] = vec4(N, 0.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * recompile le shader du matériau
 */
void WaterMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform spécifiques
    m_MapIslandPositionLoc = glGetUniformLocation(m_ShaderId, "MapIslandPosition");
    m_MapReflectionLoc    = glGetUniformLocation(m_ShaderId, "MapReflexion");
    m_MapRefractionLoc   = glGetUniformLocation(m_ShaderId, "MapRefraction");
    m_MapPhaseLoc        = glGetUniformLocation(m_ShaderId, "MapPhase");
    m_ViewPortLoc        = glGetUniformLocation(m_ShaderId, "viewport");
    m_TimeLoc           = glGetUniformLocation(m_ShaderId, "Time");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* WaterMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void WaterMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // fournir les images à traiter
    m_Unit = GL_TEXTURE0;
    if (m_FBOreflection != nullptr) {
        m_FBOreflection->setTextureUnit(m_Unit++,  m_MapReflectionLoc,    m_FBOreflection->getColorBuffer(0));
    }
    if (m_FBObackground != nullptr) {
        m_FBObackground->setTextureUnit(m_Unit++,  m_MapRefractionLoc,   m_FBObackground->getColorBuffer(0));
        m_FBObackground->setTextureUnit(m_Unit++,  m_MapIslandPositionLoc, m_FBObackground->getColorBuffer(1));
    }
    m_MapPhase->setTextureUnit(m_Unit++, m_MapPhaseLoc);

    // viewport
    vec2::glUniform(m_ViewPortLoc, m_ViewPort);

    // fournir le temps
    glUniform1f(m_TimeLoc, Utils::Time);
}


/**
 * Cette méthode désactive le matériau
 * NB : le shader doit être activé
 */
void WaterMaterial::disable()
{
    // désactiver les textures
    while (m_Unit > GL_TEXTURE0) {
        m_Unit--;
        glActiveTexture(m_Unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    } ;

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
WaterMaterial::~WaterMaterial()
{
    delete m_MapPhase;
}
