// Cette classe permet d'appliquer un tramage ordonné type Bayer sur un FBO

#include <iostream>
#include <math.h>

#include <utils.h>


#include <Dithering.h>


/**
 * Constructeur
 */
Dithering::Dithering():
    Process("Dithering")
{
    // charger la texture de Bayer, en mode nearest pour ne surtout pas interpoler ses texels
    m_BayerTexture = new Texture2D("data/textures/bayer.png", GL_NEAREST, GL_CLAMP);

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string Dithering::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform vec2 WindowSize;\n"
        "uniform sampler2D ColorMap;\n"
        "uniform sampler2D BayerMap;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "float luminance(vec3 rgb)\n"
        "{\n"
        "    const vec3 coefs = vec3(0.299, 0.587, 0.114);\n"
        "    return dot(rgb, coefs);\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // coordonnées dans la matrice de Bayer\n"
        "    vec2 coordsbayer = mod(frgTexCoord * WindowSize, 4.0) / 4.0;\n"
        "\n"
        "    // seuil de Bayer à cet endroit\n"
        "    float threshold = texture(BayerMap, coordsbayer).r;\n"
        "\n"
        "    // luminance du texel de l'image à traiter\n"
        "    vec3 color = texture(ColorMap, frgTexCoord).rgb;\n"
        "    float lum = luminance(color);\n"
        "\n"
        "    // comparaison au threshold sur chaque composante\n"
        "    glFragColor = vec4(vec3(1.0) * step(threshold, lum), 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void Dithering::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_BayerMapLoc   = glGetUniformLocation(m_ShaderId, "BayerMap");
    m_WindowSizeLoc = glGetUniformLocation(m_ShaderId, "WindowSize");
}


/**
 * applique un tramage de Bayer à l'image du FBO
 * @param fbo : FBO contenant l'image à traiter
 */
void Dithering::process(FrameBufferObject* fbo)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir la texture de Bayer
    m_BayerTexture->setTextureUnit(GL_TEXTURE1, m_BayerMapLoc);

    // fournir les paramètres du traitement
    vec2 windowsize = vec2::fromValues(fbo->getWidth(), fbo->getHeight());
    vec2::glUniform(m_WindowSizeLoc, windowsize);

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
Dithering::~Dithering()
{
    delete m_BayerTexture;
}
