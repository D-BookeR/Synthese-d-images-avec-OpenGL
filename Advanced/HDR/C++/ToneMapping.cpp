// Cette classe permet d'appliquer un tone mapping sur un FBO
// biblio : http://www.nutty.ca/?page_id=352&link=hdr

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <ToneMapping.h>


/**
 * Constructeur
 */
ToneMapping::ToneMapping():
    Process("ToneMapping")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string ToneMapping::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// pour normaliser les luminances\n"
        "uniform float MaxLuminance;\n"
        "uniform float AvgLuminance;\n"
        "uniform float alpha;\n"
        "\n"
        "// conversion RGB vers xyY\n"
        "vec3 rgb2xyY(vec3 rgb)\n"
        "{\n"
        "    const mat3 matXYZ = mat3(\n"
        "            0.4124, 0.3576, 0.1805,\n"
        "            0.2126, 0.7152, 0.0722,\n"
        "            0.0193, 0.1192, 0.9505);\n"
        "    vec3 XYZ = matXYZ * rgb;\n"
        "    float s = XYZ.x + XYZ.y + XYZ.z;\n"
        "    return vec3(XYZ.x/s, XYZ.y/s, XYZ.y);\n"
        "}\n"
        "\n"
        "// conversion xyY vers RGB\n"
        "vec3 xyY2rgb(vec3 xyY)\n"
        "{\n"
        "    vec3 XYZ = vec3(\n"
        "            (xyY.z / xyY.y) * xyY.x,\n"
        "            xyY.z,\n"
        "            (xyY.z / xyY.y) * (1.0 - xyY.x - xyY.y));\n"
        "    const mat3 matRGB = mat3(\n"
        "             3.2406, -1.5372, -0.4986,\n"
        "            -0.9689,  1.8758,  0.0415,\n"
        "             0.0557, -0.2040,  1.0570);\n"
        "    return matRGB * XYZ;\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // teinte au niveau du fragment\n"
        "    vec3 Kd = texture(ColorMap, frgTexCoord).rgb;\n"
        "\n"
        "    // conversion RGB vers xyY\n"
        "    vec3 xyY = rgb2xyY(Kd);\n"
        "\n"
        "    // luminance normalisée\n"
        "    float luminance = xyY.z / AvgLuminance * alpha;\n"
        "\n"
        "    // transformation de la luminance par la formule de Reinhard\n"
        "    luminance *= (1.0 + luminance/(MaxLuminance*MaxLuminance)) / (1.0 + luminance);\n"
        "\n"
        "    // remplacement de la luminance sans changer les couleurs\n"
        "    xyY.z = luminance;\n"
        "\n"
        "    // conversion xyY vers RGB\n"
        "    Kd = xyY2rgb(xyY);\n"
        "\n"
        "    // correction gamma\n"
        "    //Kd = pow(Kd, vec3(1.0 / 2.2));\n"
        "\n"
        "    // couleur finale du fragment\n"
        "    glFragColor = vec4(Kd, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void ToneMapping::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_MaxLuminanceLoc = glGetUniformLocation(m_ShaderId, "MaxLuminance");
    m_AvgLuminanceLoc = glGetUniformLocation(m_ShaderId, "AvgLuminance");
    m_AlphaLoc        = glGetUniformLocation(m_ShaderId, "alpha");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param fbo : FBO contenant l'image à flouter
 * @param maxlum : luminosité maximale de la scène
 * @param avglum : luminosité moyenne de la scène
 * @param alpha : luminance d'un gris moyen, 0.18 par défaut
 */
void ToneMapping::process(FrameBufferObject* fbo, float maxlum, float avglum, float alpha)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir les paramètres du traitement
    glUniform1f(m_MaxLuminanceLoc, maxlum);
    glUniform1f(m_AvgLuminanceLoc, avglum);
    glUniform1f(m_AlphaLoc, alpha);

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
ToneMapping::~ToneMapping()
{
}
