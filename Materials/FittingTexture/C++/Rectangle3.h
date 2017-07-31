#ifndef RECTANGLE3_H3_H
#define RECTANGLE3_H3_H

// Définition de la classe Rectangle3

#include <vector>
#include <map>
#include <list>

#include <gl-matrix.h>
#include <Texture2D.h>
#include <TextureMaterial.h>
#include <Mesh.h>
#include <VBOset.h>


/**
 * Classe Rectangle3.
 */
class Rectangle3
{
public:

    /**
     * constructeur
     */
    Rectangle3();

    /** destructeur, libère le VBOset et le maillage */
    ~Rectangle3();

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    // maillage
    Mesh* m_Mesh;

    // matériau
    Texture2D* m_Texture;
    TextureMaterial* m_Material;

    // VBOset
    VBOset* m_VBOset;

};

#endif
