#ifndef MIROIR_H
#define MIROIR_H

// Définition de la classe Mirror

#include <gl-matrix.h>
#include <Mesh.h>
#include <ColorMaterial.h>
#include <VBOset.h>


/**
 * Classe Mirror = encadrement et surface réfléchissante
 */
class Mirror
{
public:

    Mirror();
    ~Mirror();

    /**
     * dessine la surface du miroir
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

private:

    // partie miroir
    Mesh* m_Mesh;
    VBOset* m_VBOset;
    ColorMaterial* m_Material;
};

#endif
