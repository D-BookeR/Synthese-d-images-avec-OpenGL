// Cette classe permet d'appliquer un seuil sur une image
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include "BWThreshold.h"


/**
 * Constructeur
 */
BWThreshold::BWThreshold():
    Process("BWThreshold")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string BWThreshold::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "float luminance(vec3 rgb)\n"
        "{\n"
        "    const vec3 coefs = vec3(0.299, 0.587, 0.114);\n"
        "    return dot(rgb, coefs);\n"
        "}\n"
        "\n"
        "uniform float threshold;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 color = texture(ColorMap, frgTexCoord).rgb;\n"
        "    //if (luminance(color) > threshold) {\n"
        "    //    glFragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    //} else {\n"
        "    //    glFragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
        "    //}\n"
        "    glFragColor = vec4(vec3(step(threshold, luminance(color))), 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void BWThreshold::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_ThresholdLoc = glGetUniformLocation(m_ShaderId, "threshold");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param texture : Texture2D contenant l'image à traiter
 */
void BWThreshold::process(Texture2D* texture, float threshold)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    texture->setTextureUnit(GL_TEXTURE0, m_ColorMapLoc);

    // fournir les paramètres du shader
    glUniform1f(m_ThresholdLoc, threshold);

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
BWThreshold::~BWThreshold()
{
}
