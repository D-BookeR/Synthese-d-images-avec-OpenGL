/**
 * Définition de la classe SpotLight, un type de lampe plus sophistiqué
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <SpotLightDebug.h>
#include <SceneBase.h>


/**
 * constructeur
 * @param maxangle : angle en degrés d'ouverture total de la lampe
 * @param minangle : angle dans lequel l'intensité est 100%, il y a un dégradé entre minangle et maxangle
 * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
 * @param near : distance la plus proche dans la shadowmap
 * @param far : distance la plus lointaine dans la shadowmap
 * @param offsetfill : true si la shadow map doit activer le décalage de polygones
 * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
 */
SpotLightDebug::SpotLightDebug(float maxangle, float minangle, int shadowmapsize, float near, float far, bool offsetfill, GLenum cullface):
    SpotLight(maxangle, minangle, shadowmapsize, near, far, offsetfill, cullface)
{
    init();
}
SpotLightDebug::SpotLightDebug(float maxangle, float minangle, int shadowmapsize, float near, float far):
    SpotLight(maxangle, minangle, shadowmapsize, near, far, true, GL_BACK)
{
    init();
}
SpotLightDebug::SpotLightDebug(float maxangle, float minangle):
    SpotLight(maxangle, minangle, 0, 1.0f, 100.0f, true, GL_BACK)
{
    init();
}

/** initialisations communes à tous les constructeurs */
void SpotLightDebug::init()
{
    m_Name = "SpotLightDebug";

    /// ajouter le shader et le VBO permettant de dessiner la lampe

    // construire le vertex shader
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "uniform mat4 matMV;\n"
        "uniform mat4 matProjection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = matProjection * (matMV * vec4(glVertex, 1.0));\n"
        "}";

    // construire le fragment shader
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
        "}";

    // compiler le shader de dessin
    m_DebugShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "SpotLightDebug");

    // déterminer où sont les variables attribute et uniform
    m_DebugVertexLoc        = glGetAttribLocation( m_DebugShaderId, "glVertex");
    m_DebugMatProjectionLoc = glGetUniformLocation(m_DebugShaderId, "matProjection");
    m_DebugMatModelViewLoc  = glGetUniformLocation(m_DebugShaderId, "matMV");

    // créer et remplir le buffer des coordonnées
    std::vector<float> vertices;
    // dessiner un cône dirigé vers -Z
    float rFar = tan(m_MaxAngle * 0.5) * m_Far;
    float rNear = tan(m_MinAngle * 0.5) * m_Near;
    const int nbs = 12;
    for (int i=0; i<nbs; i++) {
        // angle, cosinus et sinus du point courant
        float a = Utils::radians(360.0 * (i+0.0) / nbs);
        float cosa = cos(a);
        float sina = sin(a);
        // angle, cosinus et sinus du point suivant
        float b = Utils::radians(360.0 * (i+1.0) / nbs);
        float cosb = cos(b);
        float sinb = sin(b);
        // cercle à la distance Near et correspondant à minangle
        vertices.push_back(rNear*cosa); vertices.push_back(rNear*sina); vertices.push_back(-m_Near);
        vertices.push_back(rNear*cosb); vertices.push_back(rNear*sinb); vertices.push_back(-m_Near);
        // rayons du spot vers la distance Far et correspondant à maxangle
        vertices.push_back(0.0);        vertices.push_back(0.0);        vertices.push_back(0.0);
        vertices.push_back(rFar*cosa);  vertices.push_back(rFar*sina);  vertices.push_back(-m_Far);
        // cercle à la distance Far et correspondant à maxangle
        vertices.push_back(rFar*cosa);  vertices.push_back(rFar*sina);  vertices.push_back(-m_Far);
        vertices.push_back(rFar*cosb);  vertices.push_back(rFar*sinb);  vertices.push_back(-m_Far);
    }
    m_DebugLineCount = vertices.size() / 3;

    // créer le VBO des coordonnées
    m_DebugVertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * destructeur
 */
SpotLightDebug::~SpotLightDebug()
{
    Utils::deleteVBO(m_DebugVertexBufferId);
    Utils::deleteShaderProgram(m_DebugShaderId);
}


/**
 * dessine une représentation de la lampe
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue, ne sert pas car on prend directement les positions caméra
 */
void SpotLightDebug::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // activer le shader
    glUseProgram(m_DebugShaderId);

    // décaler l'origine à la position caméra de la lampe
    mat4 mat4ModelView = mat4::create();
    mat4::lookAt(mat4ModelView, vec3::fromVec(m_PositionCamera), vec3::fromVec(m_TargetCamera), vec3::fromValues(0,1,0));
    mat4::invert(mat4ModelView, mat4ModelView);

    // fournir les matrices MV et P
    mat4::glUniformMatrix(m_DebugMatModelViewLoc, mat4ModelView);
    mat4::glUniformMatrix(m_DebugMatProjectionLoc, mat4Projection);

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_DebugVertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_DebugVertexBufferId);
    glVertexAttribPointer(m_DebugVertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // dessiner le maillage
    glDrawArrays(GL_LINES, 0, m_DebugLineCount);

    // désactiver les buffers et le shader
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(m_DebugVertexLoc);
    glUseProgram(0);
}


/**
 * dessine la shadowmap de cette lampe sur l'écran (remplit tout l'écran)
 */
void SpotLightDebug::onDrawShadowMap()
{
    // s'il n'y a pas de shadowmap, alors sortir sans rien faire
    if (m_ShadowMap == nullptr) return;

    m_ShadowMap->onDraw();
}
