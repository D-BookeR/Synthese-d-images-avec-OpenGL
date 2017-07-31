// Cette classe permet d'appliquer un traitement sur une image dans un FBO

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>

#include <Process.h>


// VBO pour dessiner un rectangle occupant tout l'écran
GLint Process::m_VertexBufferId = -1;
GLint Process::m_TexCoordBufferId = -1;


/**
 * crée les shaders et VBO de dessin
 */
void Process::staticinit()
{
    // créer et remplir les VBOs des coordonnées
    std::vector<GLfloat> vertices { -1,-1,  +1,-1,  +1,+1,  -1,+1 };
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    std::vector<GLfloat> texcoords {  0, 0,   1, 0,   1, 1,   0, 1 };
    m_TexCoordBufferId = Utils::makeFloatVBO(texcoords, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * supprime les shaders et VBO de dessin
 */
void Process::staticdestroy()
{
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteVBO(m_TexCoordBufferId);
}


/**
 * constructeur
 * @param nom du traitement
 */
Process::Process(std::string nom)
{
    // initialisation
    m_Name = nom;
    m_ShaderId = 0;
    m_VertexLoc = 0;
    m_TexCoordLoc = 0;
    m_ColorMapLoc = 0;
}


/**
 * destructeur
 */
Process::~Process()
{
    Utils::deleteShaderProgram(m_ShaderId);
}


/**
 * retourne le source du Vertex Shader commun à tous les traitements
 * NB : le Fragment Shader est spécifique à chaque traitement et défini dans la sous-classe
 */
std::string Process::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "out vec2 frgTexCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 0.0, 1.0);\n"
        "    frgTexCoord = glTexCoord;\n"
        "}";
    return srcVertexShader;
}


/**
 * compile ou recompile le shader
 */
void Process::compileShader()
{
    // supprimer l'ancien shader s'il y en avait un
    if (m_ShaderId > 0) Utils::deleteShaderProgram(m_ShaderId);

    // compiler le shader
    std::string srcVertexShader = getVertexShader();
    std::string srcFragmentShader = getFragmentShader();
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, m_Name);

    // déterminer où sont les variables attribute
    m_VertexLoc   = glGetAttribLocation(m_ShaderId, "glVertex");
    m_TexCoordLoc = glGetAttribLocation(m_ShaderId, "glTexCoord");

    // déterminer où sont les variables uniform (spécifiques à chaque traitement)
    findUniformLocations();
}


/**
 * détermine où sont les variables uniform du traitement
 */
void Process::findUniformLocations()
{
    m_ColorMapLoc = glGetUniformLocation(m_ShaderId, "ColorMap");
}


/**
 * active le shader et les VBO pour appliquer le traitement défini dans le shader
 */
void Process::startProcess()
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // activer et lier le VBO contenant les coordonnées
    glEnableVertexAttribArray(m_VertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_VertexLoc, Utils::VEC2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le VBO contenant les coordonnées de texture
    glEnableVertexAttribArray(m_TexCoordLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
    glVertexAttribPointer(m_TexCoordLoc, Utils::VEC2, GL_FLOAT, GL_FALSE, 0, 0);
}


/**
 * désactive le shader et les VBO
 */
void Process::endProcess()
{
    // désactiver les VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(m_VertexLoc);
    glDisableVertexAttribArray(m_TexCoordLoc);

    // désactiver le shader
    glUseProgram(0);
}


/**
 * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
 * NB : le shader concerné doit être actif
 * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
 * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
 * @param bufferId : m_ColorBuffer ou m_DepthBufferId ou 0 pour désactiver
 */
void Process::setTextureUnit(GLint unit, GLint locSampler, GLuint bufferId)
{
    /*****DEBUG*****/
    if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
        throw std::invalid_argument("Process::setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
    }
    /*****DEBUG*****/
    glActiveTexture(unit);
    if (locSampler < 0 || bufferId <= 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D, bufferId);
        glUniform1i(locSampler, unit-GL_TEXTURE0);
    }
}
