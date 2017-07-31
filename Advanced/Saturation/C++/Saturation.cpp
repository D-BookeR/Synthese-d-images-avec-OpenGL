// Cette classe permet de saturer les couleurs d'un FBO


#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>

#include <Saturation.h>


/**
 * constructeur
 * @param width : largeur du FBO utilisé
 * @param height : hauteur du FBO utilisé
 */
Saturation::Saturation(int width, int height):
    Process("Saturation")
{
    // compiler le shader
    compileShader();
}


/** destructeur */
Saturation::~Saturation()
{
}


/**
 * retourne le source du Fragment Shader
 */
std::string Saturation::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// conversion RGB en HSV\n"
        "vec3 rgb2hsv(vec3 c)\n"
        "{\n"
        "    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
        "    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
        "    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"
        "    float d = q.x - min(q.w, q.y);\n"
        "    float e = 1.0e-10;\n"
        "    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
        "}\n"
        "\n"
        "// conversion HSV en RGB\n"
        "vec3 hsv2rgb(vec3 c)\n"
        "{\n"
        "    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
        "    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
        "    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
        "}\n"
        "\n"
        "uniform float strength;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 color = texture(ColorMap, frgTexCoord);\n"
        "    vec3 hsv = rgb2hsv(color.rgb);\n"
        "    /// modification de la saturation (décommenter la ligne suivante)\n"
        "    hsv.y = clamp(hsv.y * strength, 0.0, 1.0);\n"
        "    /// sépia (décommenter la ligne suivante)\n"
        "    //hsv.xy = vec2(0.1, 0.5);\n"
        "    /// vignettage (décommenter les deux lignes suivantes)\n"
        "    //float dist = distance(frgTexCoord, vec2(0.5, 0.5));\n"
        "    //hsv.z *= 1.0 - smoothstep(0.2, 0.9, dist);\n"
        "    glFragColor = vec4(hsv2rgb(hsv), color.a);\n"
        "}";
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void Saturation::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_StrengthLoc = glGetUniformLocation(m_ShaderId, "strength");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param fbo : FBO contenant l'image à traiter
 * @param strength : force de l'effet, 1.0 ne change rien
 */
void Saturation::process(FrameBufferObject* fbo, float strength)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir le paramètre du traitement
    glUniform1f(m_StrengthLoc, strength);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);

    // libérer les ressources
    endProcess();
}
