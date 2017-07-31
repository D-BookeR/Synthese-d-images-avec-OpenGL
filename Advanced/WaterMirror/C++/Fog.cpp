// Cette classe permet d'appliquer un traitement d'image sur un FBO


#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>

#include <Fog.h>


/**
 * constructeur
 * @param color : vec4 contenant la couleur de saturation de la brume
 * @param exponent : exposant pour élever la distance
 */
Fog::Fog(vec4 color, float exponent):
    Process("Fog")
{
    // compiler le shader
    compileShader();

    // fournir la couleur d'effacement au shader
    glUseProgram(m_ShaderId);
    vec4::glUniform(m_BackgroundColorLoc, color);
    glUniform1f(m_ExponentLoc, exponent);
    glUseProgram(0);
}


/**
 * retourne le source du Fragment Shader
 */
std::string Fog::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "uniform sampler2D DepthMap;\n"
        "uniform vec4 BackgroundColor;\n"
        "uniform float exponent;\n"
        "\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // couleur actuellement présente\n"
        "    vec4 color = texture(ColorMap, frgTexCoord);\n"
        "\n"
        "    // distance donnée par le depth buffer\n"
        "    float distance = pow(texture(DepthMap, frgTexCoord).r, exponent);\n"
        "\n"
        "    // mélange entre la couleur présente et celle du fond\n"
        "    glFragColor = mix(color, BackgroundColor, distance);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void Fog::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_DepthMapLoc    = glGetUniformLocation(m_ShaderId, "DepthMap");
    m_BackgroundColorLoc = glGetUniformLocation(m_ShaderId, "BackgroundColor");
    m_ExponentLoc    = glGetUniformLocation(m_ShaderId, "exponent");
}


/**
 * effectue le traitement
 * @param fbo : FBO contenant l'image à traiter
 */
void Fog::process(FrameBufferObject* fbo)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer et le depth buffer du FBO
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());
    setTextureUnit(GL_TEXTURE1, m_DepthMapLoc, fbo->getDepthBuffer());

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);
    setTextureUnit(GL_TEXTURE1);

    // libérer les ressources
    endProcess();
}


/** destructeur */
Fog::~Fog()
{
}
