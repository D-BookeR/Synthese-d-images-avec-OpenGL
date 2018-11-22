// Cette classe permet d'extraire une petite fenêtre sur un FBO

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>


#include <SubPicture.h>


SubPicture::SubPicture(int samplesnb):
    Process("Decoupage")
{
    // vérifier que le pilote possède l'extension nécessaire
    if (! glewIsSupported("GL_ARB_texture_multisample")) {
        throw std::runtime_error("Extension GL_ARB_texture_multisample is missing");
    }

    // initialisation
    m_SamplesNumber = samplesnb,

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string SubPicture::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    if (m_SamplesNumber > 1) srcFragmentShader << "#extension GL_ARB_texture_multisample: require\n";
    srcFragmentShader << "precision mediump float;\n";
    if (m_SamplesNumber > 1) {
        srcFragmentShader << "uniform sampler2DMS ColorMap;\n";
    } else {
        srcFragmentShader << "uniform sampler2D ColorMap;\n";
    }
    srcFragmentShader << "uniform vec2 TopLeftCorner;\n";
    srcFragmentShader << "uniform vec2 Size;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // coordonnées du pixel dans la texture\n";
    srcFragmentShader << "    ivec2 texCoords = ivec2(frgTexCoord * Size + TopLeftCorner);\n";
    srcFragmentShader << "\n";
    if (m_SamplesNumber > 1) {
        srcFragmentShader << "    // calculer la moyenne des échantillons\n";
        srcFragmentShader << "    vec4 sum = vec4(0.0);\n";
        srcFragmentShader << "    for (int i=0; i<" <<m_SamplesNumber<< " ; i++) {\n";
        srcFragmentShader << "        sum += texelFetch(ColorMap, texCoords, i);\n";
        srcFragmentShader << "    }\n";
        srcFragmentShader << "    glFragColor = sum / float(" <<m_SamplesNumber<< ");\n";
    } else {
        srcFragmentShader << "    // il n'y a qu'un seul échantillon\n";
        srcFragmentShader << "    glFragColor = texelFetch(ColorMap, texCoords, 0);\n";
    }
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void SubPicture::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_TopLeftCornerLoc = glGetUniformLocation(m_ShaderId, "TopLeftCorner");
    m_SizeLoc          = glGetUniformLocation(m_ShaderId, "Size");
}


/**
 * effectue le traitement
 * @param fbo : FBO contenant l'image à extraire
 * @param x0 : absisse du coin haut gauche de l'image
 * @param y0 : ordonnée du coin haut gauche de l'image
 * @param width : largeur de l'image à extraire
 * @param height : hauteur de l'image à extraire
 */
void SubPicture::process(FrameBufferObject* fbo, int x0, int y0, int width, int height)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());

    // fournir les paramètres du traitement
    vec2 topleftcorner = vec2::fromValues(x0, y0);
    vec2::glUniform(m_TopLeftCornerLoc, topleftcorner);
    vec2 size = vec2::fromValues(width, height);
    vec2::glUniform(m_SizeLoc, size);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);

    // libérer les ressources
    endProcess();
}


/**
 * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
 * @note le shader concerné doit être actif
 * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
 * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
 * @param buffer : m_ColorBuffer ou m_DepthBuffer ou 0 pour désactiver
 */
void SubPicture::setTextureUnit(GLint unit, GLint locSampler, GLuint buffer)
{
    /*****DEBUG*****/
    if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
        throw std::invalid_argument("Process::setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
    }
    /*****DEBUG*****/
    GLenum texturetype;
    if (m_SamplesNumber <= 1) {
        texturetype = GL_TEXTURE_2D;
    } else {
        texturetype = GL_TEXTURE_2D_MULTISAMPLE;
    }
    glActiveTexture(unit);
    if (locSampler < 0 || buffer <= 0) {
        glBindTexture(texturetype, 0);
    } else {
        glBindTexture(texturetype, buffer);
        glUniform1i(locSampler, unit-GL_TEXTURE0);
    }
}



/**
 * Cette méthode supprime les ressources allouées
 */
SubPicture::~SubPicture()
{
}
