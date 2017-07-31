// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModule.js");


﻿/**
 * Ce module rajoute des fonctions et des classes pour calculer des déformations sur un maillage
 *
 */
class MeshModuleAnimation extends MeshModule
{
    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage à gérer
     */
    constructor(mesh)
    {
        super(mesh);
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * associe le maillage autremaillage en tant que déformation de this->m_Mesh
     * @param othermesh : morph target
     */
    buildMorph(othermesh)
    {
        // les deux maillages doivent avoir le même nombre de sommets
        let count = this.m_Mesh.getVertexCount();
        if (count == othermesh.getVertexCount()) {
            // utiliser les coordonnées et normales de othermesh en tant que morph target
            for (let i=0; i<count; i++) {
                let vertex = this.m_Mesh.getVertex(i);
                let morph  = othermesh.getVertex(i);
                vertex.setAttribute(MeshVertex.ID_ATTR_VERTEX1, morph.getCoord());
                vertex.setAttribute(MeshVertex.ID_ATTR_NORMAL1, morph.getNormal());
            }
        } else {
            console.warn("nombre de sommets différent entre "+this.m_Mesh.getName()+" et "+othermesh.getName());
            // recopier les coordonnées et normales du maillage en tant que morph target
            for (let i=0; i<count; i++) {
                let vertex = this.m_Mesh.getVertex(i);
                vertex.setAttribute(MeshVertex.ID_ATTR_VERTEX1, vertex.getCoord());
                vertex.setAttribute(MeshVertex.ID_ATTR_NORMAL1, vertex.getNormal());
            }
        }
    }
}
