#ifndef RECTANGLE_H
#define RECTANGLE_H

// Définition de la classe Rectangle

#include <vector>
#include <map>
#include <list>

#include <gl-matrix.h>
#include <Texture2D.h>
#include <Material.h>
#include <VBOset.h>
#include <Mesh.h>


/**
 * Classe Rectangle.
 */
class Rectangle
{
public:

    /**
     * constructeur
     */
    Rectangle();

    /** destructeur, libère le VBOset et le maillage */
    ~Rectangle();

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    // maillage
    Mesh* m_Mesh;

    // matériau des triangles
    Texture2D* m_Texture;
    Material* m_Material;

    // VBOset des triangles
    VBOset* m_VBOset;

    // matériau des arêtes
    Material* m_EdgesMaterial;

    // VBOset des arêtes
    VBOset* m_VBOsetEdges;

};

#endif
