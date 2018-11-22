// Cette classe permet d'appliquer un flou variable selon la distance
// voir http://www.nutty.ca/?page_id=352&link=depth_of_field
// voir https://github.com/ga-m3dv/ga-worldwind-suite/blob/master/Common/src/main/java/au/gov/ga/worldwind/common/effects/depthoffield/DepthOfFieldFragmentShader.glsl

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <DepthOfFieldBlur.h>


DepthOfFieldBlur::DepthOfFieldBlur():
    Process("DepthOfFieldBlur")
{
    // compiler le shader
    compileShader();
}


/**
 * change les distances near et far du shader
 * @param near : distance la plus proche dans le depth buffer du FBO à traiter
 * @param far : distance la plus lointaine dans le depth buffer du FBO à traiter
 */
void DepthOfFieldBlur::setNearFar(float near, float far)
{
    m_Near = near;
    m_Far = far;
}


/**
 * retourne le source du Fragment Shader
 */
std::string DepthOfFieldBlur::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform float sharpDistance;\n"
        "uniform float sharpness;\n"
        "uniform sampler2D ColorMap;\n"
        "uniform sampler2D DepthMap;\n"
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
        "const float maxBlurRadius = 5.0; // rayon maximal du flou\n"
        "\n"
        "// calcul de la distance réelle correspondant à depth\n"
        "uniform float near;\n"
        "uniform float far;\n"
        "float depth2distance(float depth)\n"
        "{\n"
        "    if (depth == 1.0) return 1e38;\n"
        "    return (near * far) / (far - depth * (far - near));\n"
        "}\n"
        "\n"
        "// retourne le rayon du flou à appliquer en ft de la distance\n"
        "float BlurRadius(float distance)\n"
        "{\n"
        "    float radius = abs(distance - sharpDistance) * sharpness;\n"
        "    return clamp(radius, -maxBlurRadius, +maxBlurRadius);\n"
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
        "        // définir un texel à consulter dans le voisinage\n"
        "        vec2 offset = RandomRotation * PoissonSamples[i] * maxBlurRadius;\n"
        "        vec2 coord = frgTexCoord +  TexelSize * offset;\n"
        "\n"
        "        // déterminer le rayon du disque de Bokeh de ce texel\n"
        "        float radius = BlurRadius(depth2distance(texture(DepthMap, coord).r));\n"
        "\n"
        "        // si le rayon > distance du fragment, on voit le disque de Bokeh\n"
        "        //if (radius > length(offset)) {\n"
        "        //    // le fragment est dans le disque de bokeh de l'échantillon\n"
        "        //    sum += texture(ColorMap, coord).rgb;\n"
        "        //} else {\n"
        "        //    // on ne voit pas le disque de bokeh\n"
        "        //    sum += texture(ColorMap, frgTexCoord).rgb;\n"
        "        //}\n"
        "        coord += step(radius, length(offset)) * (frgTexCoord-coord);\n"
        "        sum += texture(ColorMap, coord).rgb;\n"
        "    }\n"
        "\n"
        "    glFragColor = vec4(sum / float(PoissonCount), 1.0);\n"
        "}";
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void DepthOfFieldBlur::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_DepthMapLoc      = glGetUniformLocation(m_ShaderId, "DepthMap");
    m_TexelSizeLoc     = glGetUniformLocation(m_ShaderId, "TexelSize");
    m_NearLoc          = glGetUniformLocation(m_ShaderId, "near");
    m_FarLoc           = glGetUniformLocation(m_ShaderId, "far");
    m_SharpDistanceLoc = glGetUniformLocation(m_ShaderId, "sharpDistance");
    m_SharpnessLoc     = glGetUniformLocation(m_ShaderId, "sharpness");
    m_TimeLoc          = glGetUniformLocation(m_ShaderId, "Time");

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
 * dessine la texture fournie sur l'écran
 * @param fbonet : FBO contenant l'image et son depth buffer
 * @param focalLength : distance focale de l'objectif
 * @param aperture : ouverture de l'objectif
 * @param distance : distance où l'image est parfaitement nette (doit être >> focale)
 */
void DepthOfFieldBlur::process(FrameBufferObject* fbonet, float focalLength, float aperture, float distance)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir les color et depth buffers
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbonet->getColorBuffer());
    setTextureUnit(GL_TEXTURE1, m_DepthMapLoc, fbonet->getDepthBuffer());

    // taille des texels
    vec2 texelSize = vec2::fromValues(1.0/fbonet->getWidth(), 1.0/fbonet->getHeight());
    vec2::glUniform(m_TexelSizeLoc, texelSize);

    // fournir les paramètres du calcul de netteté
    glUniform1f(m_NearLoc, m_Near);
    glUniform1f(m_FarLoc, m_Far);
    glUniform1f(m_SharpDistanceLoc, distance);
    glUniform1f(m_SharpnessLoc, focalLength/aperture);
    glUniform1f(m_TimeLoc, Utils::Time);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);
    setTextureUnit(GL_TEXTURE1);

    // libérer les ressources
    endProcess();
}


/**
 * Cette méthode supprime les ressources allouées
 */
DepthOfFieldBlur::~DepthOfFieldBlur()
{
}
