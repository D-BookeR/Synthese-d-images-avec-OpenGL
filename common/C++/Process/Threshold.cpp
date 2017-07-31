// Cette classe permet d'appliquer un seuil sur un FBO

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>

#include <Threshold.h>


/**
 * constructeur
 */
Threshold::Threshold():
    Process("Threshold")
{
    // compiler le shader
    compileShader();
}


/** destructeur */
Threshold::~Threshold()
{
}


/**
 * retourne le source du Fragment Shader
 */
std::string Threshold::getFragmentShader()
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
        "    glFragColor = vec4(vec3(1.0) * step(threshold, luminance(color)), 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void Threshold::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_ThresholdLoc = glGetUniformLocation(m_ShaderId, "threshold");
}


/**
 * applique le traitement sur le FBO fourni : les pixels dont la luminance est
 * inférieure au seuil sont mis à noir, les autres sont remplacés par du blanc
 * @param fbo : FBO contenant l'image à traiter
 * @param threshold : seuil 0.0 à 1.0
 */
void Threshold::process(FrameBufferObject* fbo, float threshold)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO intermédiaire
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir les paramètres du shader
    glUniform1f(m_ThresholdLoc, threshold);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);

    // libérer les ressources
    endProcess();
}
