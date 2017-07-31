#ifndef MESH_MESHOBJECT_H
#define MESH_MESHOBJECT_H

// Définition de la classe MeshObject

#include <map>

#include <Mesh.h>
#include <VBOset.h>
#include <Material.h>
#include <SceneElement.h>


class MeshObject: public Drawable
{
public:

    /**
     * Crée un ensemble de maillages et de matériaux
     */
    MeshObject();

    /** Destructeur */
    virtual ~MeshObject();

    /** dessin du maillage sur l'écran */
    virtual void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

    /** transformation de l'objet par une matrice */
    virtual void transform(mat4& matrix);

    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    void setClipPlane(bool active, vec4 plane);

    /**
     * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     */
    void setClipPlane(bool active);


protected:

    // dictionnaire des maillages (nom_matériau, maillage)
    std::map<std::string, Mesh*> m_Meshes;

    // dictionnaire des matériaux (nom_matériau, matériau)
    std::map<std::string, Material*> m_Materials;

    // dictionnaire des VBOsets (nom_matériau, VBOset)
    std::map<std::string, VBOset*> m_VBOsets;
};

#endif
