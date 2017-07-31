// Cette classe permet d'appliquer une déformation type loupe sur un FBO

#include <iostream>
#include <math.h>

#include <utils.h>


#include <Lens.h>


/**
 * Constructeur
 */
Lens::Lens():
    Process("Lens")
{
    // rapport largeur/hauteur de la fenêtre
    m_Ratio = 1.0;

    // compiler le shader
    compileShader();
}


/**
 * met à jour le rapport largeur/hauteur de la fenêtre
 * @param ratio largeur/hauteur
 */
void Lens::setRatio(float ratio)
{
    m_Ratio = ratio;
}


/**
 * retourne le source du Fragment Shader
 */
std::string Lens::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// centre de la loupe\n"
        "const vec2 center = vec2(0.5, 0.5);\n"
        "const float PI = 3.14159;\n"
        "\n"
        "// fourni par m_Ratio\n"
        "uniform float ratio;\n"
        "\n"
        "// force de l'effet\n"
        "uniform float strength;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // coordonnées par rapport au centre\n"
        "    vec2 texCoords = frgTexCoord - center;\n"
        "\n"
        "    // compensation de l'image non carrée\n"
        "    vec2 xyratio = vec2(1.0, ratio);\n"
        "    texCoords /= xyratio;\n"
        "\n"
        "    // coordonnées polaires par rapport au centre\n"
        "    float r = length(texCoords);\n"
        "    float angle = atan(texCoords.y, texCoords.x);\n"
        "\n"
        "    // modifier le rayon\n"
        "    r = pow(r, strength+1.0);\n"
        "\n"
        "    // retourner en coordonnées cartésiennes\n"
        "    texCoords = r*vec2(cos(angle), sin(angle))*xyratio + center;\n"
        "\n"
        "    // aller chercher le texel situé à ces coordonnées modifiées\n"
        "    vec3 color = texture(ColorMap, texCoords).rgb;\n"
        "    glFragColor = vec4(color, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void Lens::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_StrengthLoc = glGetUniformLocation(m_ShaderId, "strength");
    m_RatioLoc       = glGetUniformLocation(m_ShaderId, "ratio");
}


/**
 * applique une distorsion en forme de loupe à l'image du FBO
 * @param fbo : FBO contenant l'image à traiter
 * @param strength : ampleur de l'effet, aucun pour 0.0
 */
void Lens::process(FrameBufferObject* fbo, float strength)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir les paramètres du traitement
    glUniform1f(m_StrengthLoc, strength);
    glUniform1f(m_RatioLoc,       m_Ratio);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);

    // libérer les ressources
    endProcess();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Lens::~Lens()
{
}
