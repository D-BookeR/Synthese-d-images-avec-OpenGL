#ifndef MISC_STAR_H
#define MISC_STAR_H

// Définition de la classe Star

#include <gl-matrix.h>

/**
 * Classe Star.
 */
class Star
{
public:

    /**
     * constructeur
     * @param size : longueur des branches
     * @param alpha : transparence
     * @param width : largeur des lignes
     */
    Star(float size=1.0, float alpha=1.0, float width=1.0);

    /** destructeur, libère le VBO et le shader */
    ~Star();

    /**
     * dessine l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param actif : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    void setClipPlane(bool actif, vec4 plane);

    /**
     * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
     * @param actif : true s'il faut garder le code gérant le plan de coupe dans le shader
     */
    void setClipPlane(bool actif);

    /**
     * désactive le plan de coupe
     */
    void resetClipPlane();


private:

    std::string getVertexShader();
    std::string getFragmentShader();
    void compileShader();

private:

    /** caractéristiques des lignes */
    float m_Size;
    float m_Alpha;
    float m_Width;

    /** plan de coupe */
    bool m_ClipPlaneOn;
    vec4 m_ClipPlane;

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;
    int m_DebugLineCount;

    /** identifiants liés au shader */
    GLint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_MatPloc;
    GLint m_MatMVloc;
    GLint m_ClipPlaneLoc;

};

#endif
