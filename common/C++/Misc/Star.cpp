// Définition de la classe Star


#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <utils.h>

#include <Star.h>


/**
 * constructeur
 * @param size : longueur des branches
 * @param alpha : transparence
 * @param width : largeur des lignes
 */
Star::Star(float size, float alpha, float width)
{
    // paramètres
    m_Size = size;
    m_Alpha = alpha;
    m_Width = width;

    // plan de coupe par défaut à l'infini
    m_ClipPlaneOn = false;
    m_ClipPlane = vec4::fromValues(0,0,1,-1e38);

    // compiler le shader
    m_ShaderId = 0;
    compileShader();

    // créer et remplir le buffer des coordonnées
    std::vector<float> vertices;
    const float s = m_Size;    // taille des branches

    // dessiner une sorte d'étoile sur tous les axes (lampe omnidirectionnelle)
    vertices.push_back(-s); vertices.push_back( 0.0); vertices.push_back( 0.0);
    vertices.push_back(+s); vertices.push_back( 0.0); vertices.push_back( 0.0);

    vertices.push_back( 0.0); vertices.push_back(-s); vertices.push_back( 0.0);
    vertices.push_back( 0.0); vertices.push_back(+s); vertices.push_back( 0.0);

    vertices.push_back( 0.0); vertices.push_back( 0.0); vertices.push_back(-s);
    vertices.push_back( 0.0); vertices.push_back( 0.0); vertices.push_back(+s);

    const float k = 0.577 * s; // c'est à dire 1/racine carrée de 3, pour faire une longueur unitaire
    vertices.push_back(-k); vertices.push_back(-k); vertices.push_back(-k);
    vertices.push_back(+k); vertices.push_back(+k); vertices.push_back(+k);

    vertices.push_back(-k); vertices.push_back(+k); vertices.push_back(-k);
    vertices.push_back(+k); vertices.push_back(-k); vertices.push_back(+k);

    vertices.push_back(-k); vertices.push_back(-k); vertices.push_back(+k);
    vertices.push_back(+k); vertices.push_back(+k); vertices.push_back(-k);

    vertices.push_back(-k); vertices.push_back(+k); vertices.push_back(+k);
    vertices.push_back(+k); vertices.push_back(-k); vertices.push_back(-k);

    // créer les VBOs des coordonnées et des couleurs
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    m_DebugLineCount = vertices.size() / 3;
}


/**
 * retourne le source du Vertex Shader
 */
std::string Star::getVertexShader()
{
    std::ostringstream srcVertexShader;
    srcVertexShader.setf(std::ios::fixed, std::ios::floatfield);
    srcVertexShader << "#version 300 es\n";
    srcVertexShader << "in vec3 glVertex;\n";
    srcVertexShader << "out vec4 frgPosition;\n";
    srcVertexShader << "uniform mat4 mat4ModelView;\n";
    srcVertexShader << "uniform mat4 mat4Projection;\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    frgPosition = mat4ModelView * vec4(glVertex*"<<m_Size<<", 1.0);\n";
    srcVertexShader << "    gl_Position = mat4Projection * frgPosition;\n";
    srcVertexShader << "}";
    return srcVertexShader.str();
}


/**
 * retourne le source du Fragment Shader
 */
std::string Star::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.precision(3);
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "uniform vec4 ClipPlane;\n";
    }
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
    }
    srcFragmentShader << "    glFragData[0] = vec4(1.0, 1.0, 0.0, "<<m_Alpha<<");\n";
    srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(1.0, 1.0, 1.0, 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader
 */
void Star::compileShader()
{
    // supprimer l'ancien shader s'il y en avait un
    if (m_ShaderId > 0) Utils::deleteShaderProgram(m_ShaderId);

    // construire le vertex shader
    std::string srcVertexShader = getVertexShader();

    // construire le fragment shader
    std::string srcFragmentShader = getFragmentShader();

    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "Star");

    // déterminer où sont les variables attribute et uniform
    m_VertexLoc    = glGetAttribLocation( m_ShaderId, "glVertex");
    m_MatPloc      = glGetUniformLocation(m_ShaderId, "mat4Projection");
    m_MatMVloc     = glGetUniformLocation(m_ShaderId, "mat4ModelView");
    m_ClipPlaneLoc = glGetUniformLocation(m_ShaderId, "ClipPlane");
}


/** destructeur */
Star::~Star()
{
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteShaderProgram(m_ShaderId);
}


/**
 * dessine l'objet
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Star::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // fournir les matrices P et MV
    mat4::glUniformMatrix(m_MatPloc, mat4Projection);
    mat4::glUniformMatrix(m_MatMVloc, mat4ModelView);

    // si le plan de coupe est actif, alors le fournir
    if (m_ClipPlaneOn) {
        vec4::glUniform(m_ClipPlaneLoc, m_ClipPlane);
    }

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_VertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_VertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // dessiner les lignes
    glLineWidth(m_Width);
    glDrawArrays(GL_LINES, 0, m_DebugLineCount);

    // désactiver les buffers et le shader
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(m_VertexLoc);
    glUseProgram(0);
}


/**
 * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
 * @param active : true s'il faut compiler un shader gérant le plan de coupe
 * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
 */
void Star::setClipPlane(bool active, vec4 plane)
{
    // il faut recompiler s'il y a un changement d'état
    bool recompile = (active != m_ClipPlaneOn);

    m_ClipPlaneOn = active;
    m_ClipPlane = plane;

    // recompiler le shader
    if (recompile) compileShader();
}


/**
 * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
 */
void Star::setClipPlane(bool active)
{
    // il faut recompiler s'il y a un changement d'état
    bool recompile = (active != m_ClipPlaneOn);

    // nouvel état et plan à l'infini
    m_ClipPlaneOn = active;
    m_ClipPlane = vec4::fromValues(0,0,1,-1e38);

    // recompiler le shader
    if (recompile) compileShader();
}


/**
 * désactive le plan de coupe
 */
void Star::resetClipPlane()
{
    bool recompile = m_ClipPlaneOn;
    m_ClipPlaneOn = false;
    m_ClipPlane = vec4::fromValues(0,0,1,-1e38);
    if (recompile) compileShader();
}
