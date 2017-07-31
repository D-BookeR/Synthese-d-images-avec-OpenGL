// Définition de la classe JointDebug
// Ses instances sont des jointures dans un squelette
// biblio https://www.opengl.org/wiki/Skeletal_Animation
// biblio https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/skeletalanimation/Tutorial%209%20-%20Skeletal%20Animation.pdf
// biblio http://dev.theomader.com/skeletal-animation-reviewed/
// biblio http://dev.theomader.com/skinning-reviewed/
// biblio http://dev.theomader.com/dual-quaternion-skinning/

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <SkeletonMaterial.h>
#include <VBOset.h>
#include <JointDebug.h>


/**
 * Constructeur
 * @param nom de la jointure
 * @param parent jointure parente
 * @param os indique s'il faut dessiner l'os ou les poids
 */
JointDebug::JointDebug(std::string nom, Joint* parent, bool os):
    Joint(nom, parent)
{

    /// ajouter le shader et les VBOs permettant de dessiner la jointure

    // construire le vertex shader
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec3 glColor;\n"
        "out vec3 frgColor;\n"
        "uniform mat4 matMV;\n"
        "uniform mat4 matProjection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = matProjection * matMV * vec4(glVertex, 1.0);\n"
        "    frgColor = glColor;\n"
        "}";

    // construire le fragment shader
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 frgColor;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = vec4(frgColor, 1.0);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "JointDebug");

    // déterminer où sont les variables attribute et uniform
    m_VertexLoc        = glGetAttribLocation( m_ShaderId, "glVertex");
    m_ColorLoc         = glGetAttribLocation( m_ShaderId, "glColor");
    m_MatProjectionLoc = glGetUniformLocation(m_ShaderId, "matProjection");
    m_MatModelViewLoc  = glGetUniformLocation(m_ShaderId, "matMV");

    // créer et remplir les buffers des coordonnées et des couleurs
    std::vector<float> vertices;
    std::vector<float> colors;

    m_IsBone = os;
    if (m_IsBone) {

        // Ce mode permet de voir les os du squelette, il affiche des sortes
        // de pointes de flèches colorées de la taille des os

        // dessiner 4 triangles formant une sorte de pointe de flèche
        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(0.0);
        colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0);
        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(-1.0);
        colors.push_back(0.0); colors.push_back(1.0); colors.push_back(0.0);
        vertices.push_back(0.0); vertices.push_back(0.1); vertices.push_back(-0.1);
        colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0);

        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(0.0);
        colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0);
        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(-1.0);
        colors.push_back(0.0); colors.push_back(1.0); colors.push_back(0.0);
        vertices.push_back(0.0); vertices.push_back(-0.1);vertices.push_back(-0.1);
        colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0);

        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(0.0);
        colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0);
        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(-1.0);
        colors.push_back(0.0); colors.push_back(1.0); colors.push_back(0.0);
        vertices.push_back(0.1); vertices.push_back(0.0); vertices.push_back(-0.1);
        colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0);

        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(0.0);
        colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0);
        vertices.push_back(0.0); vertices.push_back(0.0); vertices.push_back(-1.0);
        colors.push_back(0.0); colors.push_back(1.0); colors.push_back(0.0);
        vertices.push_back(-0.1);vertices.push_back(0.0); vertices.push_back(-0.1);
        colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0);

        m_TriangleCount = 4;

    } else {

        // Ce mode permet d'ajuster les zones m_Radius, m_Min et m_Max d'influence des jointures
        // Il affiche une sorte de cylindre dont la longueur dépend de Min et Max et le rayon
        // dépend de m_Radius. La couleur bleue indique des poids nuls, la couleur rouge un poids 1

        // dessiner un cylindre dirigé vers -Z
        const int nbs = 12;
        for (int i=0; i<nbs; i++) {
            // angle, cosinus et sinus du point courant
            float a = Utils::radians(360.0 * (i+0.0) / nbs);
            float cosa = cos(a);
            float sina = sin(a);
            // angle, cosinus et sinus du point suivant
            float b = Utils::radians(360.0 * (i+0.5) / nbs);
            float cosb = cos(b);
            float sinb = sin(b);
            // angle, cosinus et sinus du point suivant
            float c = Utils::radians(360.0 * (i+1.0) / nbs);
            float cosc = cos(c);
            float sinc = sin(c);
            // triangles
            vertices.push_back(cosa); vertices.push_back(sina); vertices.push_back(0.0);
            colors.push_back(0.0);  colors.push_back(0.0);  colors.push_back(1.0);

            vertices.push_back(cosb); vertices.push_back(sinb); vertices.push_back(-1.0);
            colors.push_back(1.0);  colors.push_back(0.0);  colors.push_back(0.0);

            vertices.push_back(cosc); vertices.push_back(sinc); vertices.push_back(0.0);
            colors.push_back(0.0);  colors.push_back(0.0);  colors.push_back(1.0);
        }
        m_TriangleCount = nbs;
    }

    // créer les VBOs des coordonnées et des couleurs
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    m_ColorBufferId  = Utils::makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * Cette méthode supprime les ressources allouées
 */
JointDebug::~JointDebug()
{
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteVBO(m_ColorBufferId);
    Utils::deleteShaderProgram(m_ShaderId);
}


/**
 * dessine une représentation de la jointure
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void JointDebug::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // afficher tous les triangles
    GLboolean cullFacePrec = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    // les os doivent apparaître devant le maillage, mais pas les poids
    GLboolean depthTestPrec = glIsEnabled(GL_DEPTH_TEST);
    if (m_IsBone) glDisable(GL_DEPTH_TEST);

    // activer le shader
    glUseProgram(m_ShaderId);

    // décaler l'origine au pivot de la jointure
    mat4 mat4ModelView = mat4::create();
    vec3 target = vec3::create();
    vec3::add(target, m_Pivot, m_Direction);
    mat4::lookAt(mat4ModelView, m_Pivot, target, vec3::fromValues(0,1,0));
    mat4::invert(mat4ModelView, mat4ModelView);

    // modifier la taille de la jointure (impossible de faire ça dans le constructeur)
    if (m_IsBone) {
        mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(m_Length, m_Length, m_Length));
    } else {
        mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(m_Radius, m_Radius, m_Length*(m_Max-m_Min)));
        mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.0, 0.0, m_Length*m_Min));
    }

    // transformations par la matrice globale et celle de vue
    mat4::multiply(mat4ModelView, m_MatGlobal, mat4ModelView);
    mat4::multiply(mat4ModelView, mat4View, mat4ModelView);

    // fournir les matrices P et MV
    mat4::glUniformMatrix(m_MatProjectionLoc,  mat4Projection);
    mat4::glUniformMatrix(m_MatModelViewLoc, mat4ModelView);

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_VertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_VertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les couleurs
    glEnableVertexAttribArray(m_ColorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
    glVertexAttribPointer(m_ColorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // dessiner le maillage
    glDrawArrays(GL_TRIANGLES, 0, m_TriangleCount*3);

    // désactiver les buffers et le shader
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(m_VertexLoc);
    glDisableVertexAttribArray(m_ColorLoc);
    glUseProgram(0);

    if (cullFacePrec) glEnable(GL_CULL_FACE);
    if (depthTestPrec) glEnable(GL_DEPTH_TEST);
}
