#ifndef TERRAIN_H
#define TERRAIN_H

// Définition de la classe Ground

#include <gl-matrix.h>
#include <Texture2D.h>
#include <Mesh.h>
#include <TextureMaterial.h>
#include <VBOset.h>


class Ground
{
public:

    /**
     * Constructeur
     */
    Ground();

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Ground();

    /**
     * dessin de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;

    Texture2D* m_Texture;
    TextureMaterial* m_Material;

    VBOset* m_VBOset;
};

#endif
