/**
 * Définition de la classe SSAOLight, une lampe ambiante avec ombrage causé par les occlusions locales
 * voir http://john-chapman-graphics.blogspot.fr/2013/01/ssao-tutorial.html
 * voir http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
 * voir http://blog.evoserv.at/index.php/2012/12/hemispherical-screen-space-ambient-occlusion-ssao-for-deferred-renderers-using-openglglsl/
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <gl-matrix.h>
#include <utils.h>
#include <SSAOLight.h>


/**
 * constructeur
 * C'est une lampe ambiante avec ombrage causé par les occlusions locales
 * @param radius : distance relative à [0.0, 1.0] d'exploration des points, mettre 0.04
 * @param mindistance : distance en unités du monde où un objet est considéré comme proche
 * @param maxdistance : distance en unités du monde à partir de laquelle un objet est trop loin pour occulter
 */
SSAOLight::SSAOLight(float radius, float mindistance, float maxdistance):
    Light()
{
    m_Name = "SSAOLight";

    // initialisation des variables membre spécifiques
    m_Radius      = radius;
    m_MinDistance = mindistance;
    m_MaxDistance = maxdistance;

    // compiler le shader
    compileShader();
}


/** destructeur */
SSAOLight::~SSAOLight()
{
}


/**
 * construit le Fragment Shader qui calcule l'éclairement de cette lampe
 * @return source du shader
 */
std::string SSAOLight::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "uniform sampler2D MapDiffuse;\n"
        "uniform sampler2D MapPosition;\n"
        "uniform sampler2D MapNormale;\n"
        "uniform sampler2D MapDepth;\n"
        "uniform vec3 LightColor;\n"
        "\n"
        "// paramètres du traitement\n"
        "uniform float radius;\n"
        "uniform float MinDistance;\n"
        "uniform float MaxDistance;\n"
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
        "    // coordonnées, profondeur, couleur et normale du fragment\n"
        "    vec4 center = texture(MapPosition, frgTexCoord);\n"
        "    if (center.w != 1.0) discard;\n"
        "    gl_FragDepth = texture(MapDepth, frgTexCoord).r;\n"
        "    vec4 color   = texture(MapDiffuse, frgTexCoord);\n"
        "    vec3 N       = texture(MapNormale, frgTexCoord).xyz;\n"
        "\n"
        "    // initialiser l'échantillonnage aléatoire de Poisson\n"
        "    float angle = rand(frgTexCoord*gl_FragDepth) * 6.283285;\n"
        "    float s = sin(angle);\n"
        "    float c = cos(angle);\n"
        "    mat2 RandomRotation = mat2(c, s, -s, c);\n"
        "\n"
        "    // passer le voisinage en revue\n"
        "    float occlusion = 0.0;\n"
        "    for (int i=0; i<PoissonCount; i++) {\n"
        "        // échantillon courant\n"
        "        vec2 offset = RandomRotation * radius * PoissonSamples[i];\n"
        "        vec4 neighbor = texture(MapPosition, frgTexCoord + offset);\n"
        "\n"
        "        // direction et distance entre ce voisin et le fragment\n"
        "        vec3 D = neighbor.xyz - center.xyz;\n"
        "        float distance = length(D);\n"
        "        float neighbor_in_front = step(0.1, dot(N, D));\n"
        "\n"
        "        // accumuler les occlusions\n"
        "        occlusion += neighbor.w * neighbor_in_front * (1.0-smoothstep(MinDistance, MaxDistance, distance));\n"
        "    }\n"
        "    \n"
        "    // normalisation de l'occlusion\n"
        "    occlusion = 1.0 - occlusion / float(PoissonCount);\n"
        "\n"
        "    // couleur finale\n"
        "    glFragColor = vec4(occlusion * color.rgb * LightColor, 1.0) * color.a;\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de cette lampe
 */
void SSAOLight::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Light::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques de cette lampe
    m_RadiusLoc      = glGetUniformLocation(m_ShaderId, "radius");
    m_MinDistanceLoc = glGetUniformLocation(m_ShaderId, "MinDistance");
    m_MaxDistanceLoc = glGetUniformLocation(m_ShaderId, "MaxDistance");

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
 * active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe
 * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
 */
void SSAOLight::startProcess(FrameBufferObject* gbuffer)
{
    // appeler la méthode de la superclasse
    Light::startProcess(gbuffer);

    // paramètres de l'ombre SSAO
    glUniform1f(m_RadiusLoc,      m_Radius);
    glUniform1f(m_MinDistanceLoc, m_MinDistance);
    glUniform1f(m_MaxDistanceLoc, m_MaxDistance);
}
