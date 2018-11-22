// Cette classe permet de calculer l'image négative (couleurs inversées)
#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <InvertColors.h>


/**
 * Constructeur
 */
InvertColors::InvertColors():
    Process("InvertColors")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string InvertColors::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// sera lié à l'image à traiter\n"
        "uniform sampler2D ColorPicture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 color = texture(ColorPicture, frgTexCoord).rgb;\n"
        "    glFragColor = vec4(1.0 - color, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void InvertColors::findUniformLocations()
{
    // déterminer où sont les variables uniform
    m_ColorPictureLoc  = glGetUniformLocation(m_ShaderId, "ColorPicture");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param texture : Texture2D contenant l'image à traiter
 */
void InvertColors::process(Texture2D* texture)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    texture->setTextureUnit(GL_TEXTURE0, m_ColorPictureLoc);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    texture->setTextureUnit(GL_TEXTURE0);

    // libérer les ressources
    endProcess();
}


/**
 * Cette méthode supprime les ressources allouées
 */
InvertColors::~InvertColors()
{
}
