#ifndef RECTANGLE2_H2_H
#define RECTANGLE2_H2_H

// Définition de la classe Rectangle2

#include <vector>
#include <map>
#include <list>

#include <gl-matrix.h>
#include <Texture2D.h>
#include <TextureMaterial.h>
#include <Mesh.h>
#include <VBOset.h>


/**
 * Classe Rectangle2.
 */
class Rectangle2
{
public:

    /**
     * constructeur
     */
    Rectangle2();

    /** destructeur, libère le VBOset et le maillage */
    ~Rectangle2();

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
