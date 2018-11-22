// Cette classe permet d'appliquer une convolution sur un FBO
// voir https://github.com/spite/Wagner/blob/master/fragment-shaders/poisson-disc-blur-fs.glsl
#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <Convolution.h>


/**
 * Constructeur
 */
Convolution::Convolution():
    Process("Convolution")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string Convolution::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "uniform vec2 TexelSize;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "vec3 getTexel(int ds, int dt)\n"
        "{\n"
        "    vec2 dsdt = vec2(float(ds), float(dt)) * TexelSize;\n"
        "    vec2 texcoord = frgTexCoord + dsdt;\n"
        "    return texture(ColorMap, texcoord).rgb;\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 sum = vec3(0.0);\n"
        "\n"
        "    sum +=    1.0 * getTexel(-2, -2);\n"
        "    sum +=    4.0 * getTexel(-1, -2);\n"
        "    sum +=    6.0 * getTexel( 0, -2);\n"
        "    sum +=    4.0 * getTexel(+1, -2);\n"
        "    sum +=    1.0 * getTexel(+2, -2);\n"
        "\n"
        "    sum +=    4.0 * getTexel(-2, -1);\n"
        "    sum +=   16.0 * getTexel(-1, -1);\n"
        "    sum +=   24.0 * getTexel( 0, -1);\n"
        "    sum +=   16.0 * getTexel(+1, -1);\n"
        "    sum +=    4.0 * getTexel(+2, -1);\n"
        "\n"
        "    sum +=    6.0 * getTexel(-2,  0);\n"
        "    sum +=   24.0 * getTexel(-1,  0);\n"
        "    sum += -476.0 * getTexel( 0,  0);\n"
        "    sum +=   24.0 * getTexel(+1,  0);\n"
        "    sum +=    6.0 * getTexel(+2,  0);\n"
        "\n"
        "    sum +=    4.0 * getTexel(-2, +1);\n"
        "    sum +=   16.0 * getTexel(-1, +1);\n"
        "    sum +=   24.0 * getTexel( 0, +1);\n"
        "    sum +=   16.0 * getTexel(+1, +1);\n"
        "    sum +=    4.0 * getTexel(+2, +1);\n"
        "\n"
        "    sum +=    1.0 * getTexel(-2, +2);\n"
        "    sum +=    4.0 * getTexel(-1, +2);\n"
        "    sum +=    6.0 * getTexel( 0, +2);\n"
        "    sum +=    4.0 * getTexel(+1, +2);\n"
        "    sum +=    1.0 * getTexel(+2, +2);\n"
        "\n"
        "    const float sommecoefs = -256.0;\n"
        "\n"
        "    glFragColor = vec4(sum/sommecoefs, 1.0);\n"
        "}";
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void Convolution::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_TexelSizeLoc = glGetUniformLocation(m_ShaderId, "TexelSize");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param fbo : FBO contenant l'image à traiter
 */
void Convolution::process(FrameBufferObject* fbo)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir les paramètres du flou
    vec2 texelSize = vec2::fromValues(1.0/fbo->getWidth(), 1.0/fbo->getHeight());
    vec2::glUniform(m_TexelSizeLoc, texelSize);

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
Convolution::~Convolution()
{
}
