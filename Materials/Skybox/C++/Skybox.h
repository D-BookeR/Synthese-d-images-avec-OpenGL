#ifndef SKYBOX_H
#define SKYBOX_H

// Définition de la classe Skybox

#include <vector>
#include <map>
#include <list>
#include <gl-matrix.h>

#include <Mesh.h>
#include <VBOset.h>

#include "SkyboxMaterial.h"


/**
 * Classe Skybox.
 */
class Skybox
{
public:

    /**
     * constructeur
     */
    Skybox();

    /** destructeur, libère le VBOset et le maillage */
    ~Skybox();

    /**
     * dessin du Skybox sur l'écran
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
