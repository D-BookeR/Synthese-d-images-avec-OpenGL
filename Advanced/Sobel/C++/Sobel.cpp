// Cette classe permet d'appliquer le filtre de Sobel sur un FBO


#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>

#include <Sobel.h>


/**
 * constructeur
 * @param width : largeur du FBO utilisé
 * @param height : hauteur du FBO utilisé
 */
Sobel::Sobel(int width, int height):
    Process("Sobel")
{
    // créer deux FBO pour les résultats intermédiaires
    m_FBO[0] = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_NONE);
    m_FBO[1] = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_NONE);

    // taille des pixels : inverse de la fenêtre
    vec2 texelsize = vec2::fromValues(1.0/width, 1.0/height);

    // il y a quatre shaders : un pour chaque passe
    for (int pass=0; pass<4; pass++) {
        // compiler le shader correspondant à la passe
        compileShader(pass);

        // lui fournir la dimension de la fenêtre
        glUseProgram(m_ShaderId[pass]);
        vec2::glUniform(m_TexelSizeLoc[pass], texelsize);
        glUseProgram(0);
    }
}


/** destructeur */
Sobel::~Sobel()
{
    delete m_FBO[0];
    delete m_FBO[1];

    for (int pass=0; pass<4; pass++) {
        Utils::deleteShaderProgram(m_ShaderId[pass]);
    }
}


/**
 * retourne le source du Vertex Shader correspondant à la passe
 * @param pass : code à 2 bits indiquant passe une horizontale pour 0 ou 1, verticale pour 2 ou 3
 */
std::string Sobel::getVertexShader(int pass)
{
    if (pass < 2) return
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "uniform vec2 TexelSize;\n"
        "out vec2 frgKernelTexCoord[3];\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 0.0, 1.0);\n"
        "    frgKernelTexCoord[0] = glTexCoord + vec2(-TexelSize.x, 0.0);\n"
        "    frgKernelTexCoord[1] = glTexCoord + vec2( 0.0,         0.0);\n"
        "    frgKernelTexCoord[2] = glTexCoord + vec2(+TexelSize.x, 0.0);\n"
        "}";
    else return
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "uniform vec2 TexelSize;\n"
        "out vec2 frgKernelTexCoord[3];\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 0.0, 1.0);\n"
        "    frgKernelTexCoord[0] = glTexCoord + vec2(0.0, -TexelSize.y);\n"
        "    frgKernelTexCoord[1] = glTexCoord + vec2(0.0,  0.0);\n"
        "    frgKernelTexCoord[2] = glTexCoord + vec2(0.0, +TexelSize.y);\n"
        "}";
}


/**
 * retourne le source du Fragment Shader correspondant à la passe
 * @param pass : convolution [1 2 1] pour 0 ou 2, [-1 0 +1] pour 1 ou 3
 */
std::string Sobel::getFragmentShader(int pass)
{
    if (pass%2 == 0) return
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "in vec2 frgKernelTexCoord[3];\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    vec4 sum = vec4(0.0);\n"
        "    sum += texture(ColorMap, frgKernelTexCoord[0]) * +1.0;\n"
        "    sum += texture(ColorMap, frgKernelTexCoord[1]) * +2.0;\n"
        "    sum += texture(ColorMap, frgKernelTexCoord[2]) * +1.0;\n"
        "\n"
        "    glFragColor = sum;\n"
        "}";
    else return
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "in vec2 frgKernelTexCoord[3];\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    vec4 sum = vec4(0.0);\n"
        "    sum += texture(ColorMap, frgKernelTexCoord[0]) * -1.0;\n"
        "    sum += texture(ColorMap, frgKernelTexCoord[2]) * +1.0;\n"
        "\n"
        "    glFragColor = sum;\n"
        "}";
}


/**
 * compile ou recompile le shader correspondant à la passe
 * @param pass : code à 2 bits de 0 à 3
 */
void Sobel::compileShader(int pass)
{
    // supprimer l'ancien shader s'il y en avait un
    if (m_ShaderId[pass] > 0) Utils::deleteShaderProgram(m_ShaderId[pass]);

    // compiler le shader
    std::string srcVertexShader = getVertexShader(pass);
    std::string srcFragmentShader = getFragmentShader(pass);
    m_ShaderId[pass] = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "Sobel");

    // déterminer où sont les variables attribute
    m_VertexLoc[pass]   = glGetAttribLocation(m_ShaderId[pass], "glVertex");
    m_TexCoordLoc[pass] = glGetAttribLocation(m_ShaderId[pass], "glTexCoord");

    // déterminer où sont les variables uniform
    m_ColorMapLoc[pass]   = glGetUniformLocation(m_ShaderId[pass], "ColorMap");
    m_TexelSizeLoc[pass] = glGetUniformLocation(m_ShaderId[pass], "TexelSize");
}


/**
 * applique le traitement sur l'image fournie dans le FBO
 * @param fbo : FBO contenant l'image à traiter
 */
void Sobel::process(FrameBufferObject* fbo)
{
    // désactiver le test du depth buffer
    glDisable(GL_DEPTH_TEST);

    // 4 passages sur l'image, en alternant les FBO
    for (int pass=0; pass<4; pass++) {

        // dessiner dans un FBO intermédiaire sauf au dernier passage
        if (pass < 3) m_FBO[pass%2]->enable();

        // activer le shader de cette passe
        glUseProgram(m_ShaderId[pass]);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_VertexLoc[pass]);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertexLoc[pass], 2, GL_FLOAT, GL_FALSE, 0, 0);

        // activer et lier le buffer contenant les coordonnées de texture
        glEnableVertexAttribArray(m_TexCoordLoc[pass]);
        glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
        glVertexAttribPointer(m_TexCoordLoc[pass], 2, GL_FLOAT, GL_FALSE, 0, 0);

        // fournir le color buffer selon la passe
        if (pass == 0) {
            // c'est le premier passage, on fournit le color buffer du fbo à traiter
            setTextureUnit(GL_TEXTURE0, m_ColorMapLoc[pass], fbo->getColorBuffer());
        } else {
            // fournir le FBO intermédiaire précédent
            setTextureUnit(GL_TEXTURE0, m_ColorMapLoc[pass], m_FBO[(pass-1)%2]->getColorBuffer());
        }

        // dessiner un quadrilatère avec les quatre vertices
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        setTextureUnit(GL_TEXTURE0);

        // désactiver le buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexLoc[pass]);
        glDisableVertexAttribArray(m_TexCoordLoc[pass]);

        // désactiver le shader
        glUseProgram(0);

        // termine les dessins dans le FBO intermédiaire et remet le précédent FBO
        if (pass < 3) m_FBO[pass%2]->disable();
    }

    // réactiver le test du depth buffer
    glEnable(GL_DEPTH_TEST);
}
