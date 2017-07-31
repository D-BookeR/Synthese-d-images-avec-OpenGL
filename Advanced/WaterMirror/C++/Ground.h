#ifndef GROUND_H
#define GROUND_H

// Définition de la classe Ground

#include <gl-matrix.h>
#include <Mesh.h>
#include <Texture2D.h>
#include <TextureMaterial.h>
#include <VBOset.h>


/**
 * Classe Ground
 */
class Ground
{
public:

    /**
     * Constructeur
     * @param profondeur à affecter à la coordonnée y des sommets
     */
    Ground(float depth);
    ~Ground();

    /**
     * dessine le fond
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;
    VBOset* m_VBOset;
    Texture2D* m_Texture;
    TextureMaterial* m_Material;

};

#endif
