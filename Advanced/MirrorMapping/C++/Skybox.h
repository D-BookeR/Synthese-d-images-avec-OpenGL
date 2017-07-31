#ifndef SKYBOX_H
#define SKYBOX_H

// Définition de la classe Skybox

#include <vector>
#include <map>
#include <list>

#include <gl-matrix.h>
#include <SkyboxMaterial.h>
#include <Mesh.h>
#include <VBOset.h>
#include <TextureCube.h>

#include "SkyboxMaterial.h"


/**
 * Classe Skybox.
 */
class Skybox
{
public:

    /**
     * Cette fonction définit la classe Skybox = maillage recouvert par une texture.
     * @param texture : instance de TextureCube
     */
    Skybox(TextureCube* texture);

    /** destructeur, libère le VBOset et le maillage */
    ~Skybox();

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
    SkyboxMaterial* m_Material;

    // VBOset
    VBOset* m_VBOset;

};

#endif
