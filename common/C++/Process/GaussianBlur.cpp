// Cette classe permet d'appliquer un flou gaussien sur un FBO
// voir https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5
// voir http://xissburg.com/faster-gaussian-blur-in-glsl/
// voir https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>

#include <GaussianBlur.h>


/**
 * constructeur
 * @param width : largeur du FBO utilisé pour calculer le flou
 * @param height : hauteur du FBO utilisé pour calculer le flou
 */
GaussianBlur::GaussianBlur(int width, int height):
    Process("GaussianBlur")
{
    // créer un FBO pour le résultat intermédiaire
    m_FBO = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_NONE);

    // initialisations
    m_HorizShaderId = 0;
    m_VertiShaderId = 0;

    // il y a deux shaders : un pour flouter horizontalement, l'autre verticalement
    compileHorizShader();
    compileVertiShader();
}


/** destructeur */
GaussianBlur::~GaussianBlur()
{
    delete m_FBO;
    Utils::deleteShaderProgram(m_HorizShaderId);
    Utils::deleteShaderProgram(m_VertiShaderId);
}


/**
 * retourne le source du Vertex Shader horizontal
 */
std::string GaussianBlur::getHorizVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "uniform float TexelSize;\n"
        "out vec2 frgTexCoord;\n"
        "out vec2 frgBlurTexCoord[14];\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 0.0, 1.0);\n"
        "    frgTexCoord = glTexCoord;\n"
        "    frgBlurTexCoord[ 0] = glTexCoord + vec2(-7.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 1] = glTexCoord + vec2(-6.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 2] = glTexCoord + vec2(-5.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 3] = glTexCoord + vec2(-4.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 4] = glTexCoord + vec2(-3.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 5] = glTexCoord + vec2(-2.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 6] = glTexCoord + vec2(-1.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 7] = glTexCoord + vec2( 1.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 8] = glTexCoord + vec2( 2.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[ 9] = glTexCoord + vec2( 3.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[10] = glTexCoord + vec2( 4.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[11] = glTexCoord + vec2( 5.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[12] = glTexCoord + vec2( 6.0*TexelSize, 0.0);\n"
        "    frgBlurTexCoord[13] = glTexCoord + vec2( 7.0*TexelSize, 0.0);\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Vertex Shader vertical
 */
std::string GaussianBlur::getVertiVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "uniform float TexelSize;\n"
        "out vec2 frgTexCoord;\n"
        "out vec2 frgBlurTexCoord[14];\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 0.0, 1.0);\n"
        "    frgTexCoord = glTexCoord;\n"
        "    frgBlurTexCoord[ 0] = glTexCoord + vec2(0.0, -7.0*TexelSize);\n"
        "    frgBlurTexCoord[ 1] = glTexCoord + vec2(0.0, -6.0*TexelSize);\n"
        "    frgBlurTexCoord[ 2] = glTexCoord + vec2(0.0, -5.0*TexelSize);\n"
        "    frgBlurTexCoord[ 3] = glTexCoord + vec2(0.0, -4.0*TexelSize);\n"
        "    frgBlurTexCoord[ 4] = glTexCoord + vec2(0.0, -3.0*TexelSize);\n"
        "    frgBlurTexCoord[ 5] = glTexCoord + vec2(0.0, -2.0*TexelSize);\n"
        "    frgBlurTexCoord[ 6] = glTexCoord + vec2(0.0, -1.0*TexelSize);\n"
        "    frgBlurTexCoord[ 7] = glTexCoord + vec2(0.0,  1.0*TexelSize);\n"
        "    frgBlurTexCoord[ 8] = glTexCoord + vec2(0.0,  2.0*TexelSize);\n"
        "    frgBlurTexCoord[ 9] = glTexCoord + vec2(0.0,  3.0*TexelSize);\n"
        "    frgBlurTexCoord[10] = glTexCoord + vec2(0.0,  4.0*TexelSize);\n"
        "    frgBlurTexCoord[11] = glTexCoord + vec2(0.0,  5.0*TexelSize);\n"
        "    frgBlurTexCoord[12] = glTexCoord + vec2(0.0,  6.0*TexelSize);\n"
        "    frgBlurTexCoord[13] = glTexCoord + vec2(0.0,  7.0*TexelSize);\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string GaussianBlur::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "in vec2 frgTexCoord;\n"
        "in vec2 frgBlurTexCoord[14];\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "\n"
        "    vec4 sum = vec4(0.0);\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 0])*0.0044299121055113265;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 1])*0.00895781211794;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 2])*0.0215963866053;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 3])*0.0443683338718;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 4])*0.0776744219933;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 5])*0.115876621105;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 6])*0.147308056121;\n"
        "    sum += texture(ColorMap, frgTexCoord        )*0.159576912161;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 7])*0.147308056121;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 8])*0.115876621105;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[ 9])*0.0776744219933;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[10])*0.0443683338718;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[11])*0.0215963866053;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[12])*0.00895781211794;\n"
        "    sum += texture(ColorMap, frgBlurTexCoord[13])*0.0044299121055113265;\n"
        "\n"
        "    glFragColor = sum;\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader horizontal
 */
void GaussianBlur::compileHorizShader()
{
    // supprimer l'ancien shader s'il y en avait un
    if (m_HorizShaderId > 0) Utils::deleteShaderProgram(m_HorizShaderId);

    // compiler le shader de floutage horizontal
    std::string srcVertexShader = getHorizVertexShader();
    std::string srcFragmentShader = getFragmentShader();
    m_HorizShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "HorizBlur");

    // déterminer où sont les variables attribute
    m_HorizVertexLoc   = glGetAttribLocation(m_HorizShaderId, "glVertex");
    m_HorizTexCoordLoc = glGetAttribLocation(m_HorizShaderId, "glTexCoord");

    // déterminer où sont les variables uniform
    m_HorizColorMapLoc  = glGetUniformLocation(m_HorizShaderId, "ColorMap");
    m_HorizTexelSizeLoc = glGetUniformLocation(m_HorizShaderId, "TexelSize");
}


/**
 * compile ou recompile le shader vertical
 */
void GaussianBlur::compileVertiShader()
{
    // supprimer l'ancien shader s'il y en avait un
    if (m_VertiShaderId > 0) Utils::deleteShaderProgram(m_VertiShaderId);

    // compiler le shader de floutage vertical
    std::string srcVertexShader = getVertiVertexShader();
    std::string srcFragmentShader = getFragmentShader();
    m_VertiShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "VertiBlur");

    // déterminer où sont les variables attribute
    m_VertiVertexLoc    = glGetAttribLocation(m_VertiShaderId, "glVertex");
    m_VertiTexCoordLoc  = glGetAttribLocation(m_VertiShaderId, "glTexCoord");

    // déterminer où sont les variables uniform
    m_VertiColorMapLoc  = glGetUniformLocation(m_VertiShaderId, "ColorMap");
    m_VertiTexelSizeLoc = glGetUniformLocation(m_VertiShaderId, "TexelSize");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param fbo : FBO contenant l'image à flouter
 * @param radius : rayon du flou
 */
void GaussianBlur::process(FrameBufferObject* fbo, float radius)
{
    // Première étape : flouter horizontalement dans un FBO intermédiaire
    m_FBO->enable();

    // désactiver le test du depth buffer
    glDisable(GL_DEPTH_TEST);

    // activer le shader horizontal
    glUseProgram(m_HorizShaderId);

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_HorizVertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_HorizVertexLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les coordonnées de texture
    glEnableVertexAttribArray(m_HorizTexCoordLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
    glVertexAttribPointer(m_HorizTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // fournir le color buffer
    setTextureUnit(GL_TEXTURE0, m_HorizColorMapLoc, fbo->getColorBuffer());

    // fournir les paramètres du flou
    glUniform1f(m_HorizTexelSizeLoc, radius/fbo->getWidth());

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);

    // désactiver le buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(m_HorizVertexLoc);
    glDisableVertexAttribArray(m_HorizTexCoordLoc);

    // termine les dessins dans le FBO intermédiaire et remet le précédent FBO
    m_FBO->disable();

    // Deuxième phase : flouter verticalement le précédent FBO

    // activer le shader vertical
    glUseProgram(m_VertiShaderId);

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_VertiVertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_VertiVertexLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les coordonnées de texture
    glEnableVertexAttribArray(m_VertiTexCoordLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
    glVertexAttribPointer(m_VertiTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // fournir le color buffer du FBO intermédiaire
    setTextureUnit(GL_TEXTURE0, m_VertiColorMapLoc, m_FBO->getColorBuffer());

    // fournir les paramètres du flou
    glUniform1f(m_VertiTexelSizeLoc, radius/fbo->getHeight());

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);

    // désactiver le buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(m_VertiVertexLoc);
    glDisableVertexAttribArray(m_VertiTexCoordLoc);

    // désactiver le shader
    glUseProgram(0);

    // réactiver le test du depth buffer
    glEnable(GL_DEPTH_TEST);
}
