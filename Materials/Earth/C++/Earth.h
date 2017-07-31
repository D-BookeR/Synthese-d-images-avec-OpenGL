#ifndef TERRE_H
#define TERRE_H

// Définition de la classe Earth

#include <gl-matrix.h>
#include <Texture2D.h>

class Earth
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_NormalBufferId;

    /** coordonnées de texture */
    GLuint m_TexcoordBufferId;

    GLuint m_IndexBufferId;
    int m_IndexBufferSize;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLuint m_VertexLoc;
    GLuint m_NormalLoc;
    GLuint m_TexCoordLoc;

    GLuint m_MatMVLoc;
    GLuint m_MatPLoc;
    GLuint m_MatNLoc;
    GLuint m_LightPositionLoc;
    GLuint m_TxDiffuseDayLoc;
    GLuint m_TxDiffuseNightLoc;
    GLuint m_TxSpecularLoc;


    /** textures */
    Texture2D* m_TxDiffuseDay;
    Texture2D* m_TxDiffuseNight;
    Texture2D* m_TxSpecular;

public:

    /** constructeur, crée le VBO et le shader */
    Earth(int nblon, int nblat) throw (std::string);

    /** destructeur, libère le VBO et le shader */
    ~Earth();

    /**
     * définit la position de la lampe
     * @param position : vec4 donnant les coordonnées de la lampe
     */
    void setLightPosition(vec4& position);

    /**
     * dessiner l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);
};

#endif
