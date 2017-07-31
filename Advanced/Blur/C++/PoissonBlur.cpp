// Cette classe permet d'appliquer un flou de Poisson sur un FBO
// voir https://github.com/spite/Wagner/blob/master/fragment-shaders/poisson-disc-blur-fs.glsl
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <PoissonBlur.h>


/**
 * Constructeur
 */
PoissonBlur::PoissonBlur():
    Process("PoissonBlur")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string PoissonBlur::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "uniform vec2 TexelSize;\n"
        "in vec2 frgTexCoord;\n"
        "uniform float Time;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// échantillonnage de Poisson\n"
        "const int PoissonCount = 16;\n"
        "uniform vec2 PoissonSamples[PoissonCount];\n"
        "\n"
        "highp float rand(vec2 co)\n"
        "{\n"
        "    highp float a  = 12.9898;\n"
        "    highp float b  = 78.233;\n"
        "    highp float c  = 43758.5453;\n"
        "    highp float dt = dot(co ,vec2(a,b));\n"
        "    highp float sn = mod(dt, 3.141592);\n"
        "    return fract(sin(sn) * c);\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // initialiser l'échantillonnage aléatoire de Poisson\n"
        "    float angle = rand(frgTexCoord) * 6.283285 * Time;\n"
        "    float s = sin(angle);\n"
        "    float c = cos(angle);\n"
        "    mat2 RandomRotation = mat2(c, s, -s, c);\n"
        "\n"
        "    // accumuler les couleurs\n"
        "    vec3 sum = vec3(0.0);\n"
        "    for (int i=0; i < PoissonCount; i++) {\n"
        "        vec2 offset = RandomRotation * PoissonSamples[i];\n"
        "        sum += texture(ColorMap, frgTexCoord + offset*TexelSize).rgb;\n"
        "    }\n"
        "    glFragColor = vec4(sum/float(PoissonCount), 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void PoissonBlur::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_TexelSizeLoc = glGetUniformLocation(m_ShaderId, "TexelSize");
    m_TimeLoc      = glGetUniformLocation(m_ShaderId, "Time");

    // initialiser le tableau des constantes (erreur : arrays may not be declared constant since they cannot be initialized)
    GLint PoissonSamplesLoc = glGetUniformLocation(m_ShaderId, "PoissonSamples");
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
    glUniform2fv(PoissonSamplesLoc, 16, PoissonSamples);
    glUseProgram(0);
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param fbo : FBO contenant l'image à flouter
 * @param radius : rayon du flou
 */
void PoissonBlur::process(FrameBufferObject* fbo, float radius)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir les paramètres du flou
    radius = fabs(radius);
    vec2 texelSize = vec2::fromValues(radius/fbo->getWidth(), radius/fbo->getHeight());
    vec2::glUniform(m_TexelSizeLoc, texelSize);
    glUniform1f(m_TimeLoc, Utils::Time);

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
PoissonBlur::~PoissonBlur()
{
}
