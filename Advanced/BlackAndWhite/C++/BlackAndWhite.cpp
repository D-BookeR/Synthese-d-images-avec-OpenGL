// Cette classe permet de mettre une image en noir&blanc
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <BlackAndWhite.h>


/**
 * Constructeur
 */
BlackAndWhite::BlackAndWhite():
    Process("BlackAndWhite")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string BlackAndWhite::getFragmentShader()
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
        "// coefficients UIT-R BT 709\n"
        "const vec3 coefs = vec3(0.2126, 0.7152, 0.0722);"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 color = texture(ColorPicture, frgTexCoord).rgb;\n"
        "    float luma = dot(color, coefs);\n"
        "    glFragColor = vec4(luma,luma,luma, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void BlackAndWhite::findUniformLocations()
{
    // déterminer où sont les variables uniform
    m_ColorPictureLoc  = glGetUniformLocation(m_ShaderId, "ColorPicture");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param texture : Texture2D contenant l'image à traiter
 */
void BlackAndWhite::process(Texture2D* texture)
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
BlackAndWhite::~BlackAndWhite()
{
}
