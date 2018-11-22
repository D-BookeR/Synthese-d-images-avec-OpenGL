// Définition de la classe MeshObject

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>

#include <MeshObject.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>


/**
 * Crée un ensemble de maillages et de matériaux
 */
MeshObject::MeshObject()
{
}


/**
 * destructeur
 */
MeshObject::~MeshObject()
{
    for (auto const& it: m_VBOsets) {
        delete it.second;
    }
    for (auto const& it: m_Meshes) {
        delete it.second;
    }
    for (auto const& it: m_Materials) {
        delete it.second;
    }
}


/**
 * transforme l'objet
 * @param matrix matrice de transformation
 */
void MeshObject::transform(mat4& matrix)
{
}


/**
 * dessine l'objet
 * @param mat4Projection matrice de projection
 * @param mat4ModelView matrice qui positionne l'objet devant la caméra
 */
void MeshObject::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner les maillages
    for (auto const& it: m_VBOsets) {
        VBOset* vboset = it.second;
        vboset->onDraw(mat4Projection, mat4ModelView);
    }
}


/**
 * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
 * @param active : true s'il faut compiler un shader gérant le plan de coupe
 * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
 */
void MeshObject::setClipPlane(bool active, vec4 plane)
{
    // fournir le plan de coupe aux matériaux
    for (auto const& it: m_Materials) {
        Material* material = it.second;
        material->setClipPlane(active, plane);
    }
}


/**
 * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
 * @param active : true s'il faut compiler un shader gérant le plan de coupe
 */
void MeshObject::setClipPlane(bool active)
{
    // fournir le plan de coupe aux matériaux
    for (auto const& it: m_Materials) {
        Material* material = it.second;
        material->setClipPlane(active);
    }
}
