#ifndef MISC_GRIDXZ_H
#define MISC_GRIDXZ_H

// Définition de la classe GridXZ

#include <gl-matrix.h>

/**
 * Classe GridXZ.
 */
class GridXZ
{
public:

    /**
     * constructeur
     * @param count : nombre de graduations de part et d'autre de zéro
     * @param step : écart entre deux lignes
     * @param alpha : transparence de la grille
     * @param width : largeur des lignes
     */
    GridXZ(int count=5, float step=1.0, float alpha=1.0, float width=1.0);

    /** destructeur, libère le VBO et le shader */
    ~GridXZ();

    /**
     * dessine l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    void setClipPlane(bool active, vec4 plane);

    /**
     * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
     * @param active : true s'il faut garder le code gérant le plan de coupe dans le shader
     */
    void setClipPlane(bool active);

    /**
     * désactive le plan de coupe
     */
    void resetClipPlane();


private:

    std::string getVertexShader();
    std::string getFragmentShader();
    void compileShader();

private:

    /** épaisseur des lignes et nombre de lignes */
    float m_Width;
    int m_VertexCount;
    float m_Alpha;

    /** plan de coupe */
    bool m_ClipPlaneOn;
    vec4 m_ClipPlane;

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;
    GLuint m_ColorBufferId;

    /** identifiants liés au shader */
    GLint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_ColorLoc;
    GLint m_MatPloc;
    GLint m_MatMVloc;
    GLint m_ClipPlaneLoc;

};

#endif
