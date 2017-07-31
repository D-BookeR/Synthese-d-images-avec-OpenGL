#ifndef ROCHER_H
#define ROCHER_H

// Définition de la classe Rock

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>
#include <Texture2D.h>
#include <TextureMaterial.h>


class Rock
{
public:

    /**
     * Constructeur
     * @param texture Texture2D à appliquer sur l'objet
     */
    Rock(Texture2D* texture);

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Rock();

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
