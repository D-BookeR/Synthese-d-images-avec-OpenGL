#ifndef FORME_H
#define FORME_H

// Définition de la classe Shape

#include <vector>
#include <map>
#include <list>

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>

#include "ColorPerVertexMaterial.h"


/**
 * Classe Shape.
 */
class Shape
{
public:

    /**
     * constructeur
     */
    Shape();

    /** destructeur, libère le VBOset et le maillage */
    ~Shape();

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
    ColorPerVertexMaterial* m_Material;

    // VBOset
    VBOset* m_VBOset;

};

#endif
