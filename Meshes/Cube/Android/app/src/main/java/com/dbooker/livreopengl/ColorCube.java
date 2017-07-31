package com.dbooker.livreopengl;


import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleProcessing;
import com.dbooker.livreopengl.mesh.MeshModulePhysics;
import com.dbooker.livreopengl.mesh.MeshVertex;

public class ColorCube
{
    private Mesh m_Mesh;

    public ColorCube() throws Exception
    {
        // créer le maillage
        m_Mesh = new Mesh("ColorCube");


        /// Topologie : éléments et connexions

        // rajouter des sommets
        MeshVertex s1 = m_Mesh.addVertex("s1");
        MeshVertex s2 = m_Mesh.addVertex("s2");
        MeshVertex s3 = m_Mesh.addVertex("s3");
        MeshVertex s4 = m_Mesh.addVertex("s4");
        MeshVertex s5 = m_Mesh.addVertex("s5");
        MeshVertex s6 = m_Mesh.addVertex("s6");
        MeshVertex s7 = m_Mesh.addVertex("s7");
        MeshVertex s8 = m_Mesh.addVertex("s8");

        // rajouter des triangles
        m_Mesh.addQuad(s1, s4, s3, s2);
        m_Mesh.addQuad(s1, s2, s6, s5);
        m_Mesh.addQuad(s2, s3, s7, s6);
        m_Mesh.addQuad(s3, s4, s8, s7);
        m_Mesh.addQuad(s4, s1, s5, s8);
        m_Mesh.addQuad(s5, s6, s7, s8);

        /// Géométrie : position des sommets et autres info

        // affectation des couleurs et des coordonnées des sommets
        s1.setCoord(vec3.fromValues(-1,-1,+1)); s1.setColor(vec3.fromValues(0.9,0.0,0.0));   // rouge
        s2.setCoord(vec3.fromValues(+1,-1,+1)); s2.setColor(vec3.fromValues(0.0,0.8,0.0));   // vert
        s3.setCoord(vec3.fromValues(+1,-1,-1)); s3.setColor(vec3.fromValues(0.0,0.2,1.0));   // bleu
        s4.setCoord(vec3.fromValues(-1,-1,-1)); s4.setColor(vec3.fromValues(0.5,0.0,0.5));   // violet
        s5.setCoord(vec3.fromValues(-1,+1,+1)); s5.setColor(vec3.fromValues(0.2,1.0,1.0));   // bleu clair
        s6.setCoord(vec3.fromValues(+1,+1,+1)); s6.setColor(vec3.fromValues(1.0,1.0,0.2));   // jaune
        s7.setCoord(vec3.fromValues(+1,+1,-1)); s7.setColor(vec3.fromValues(1.0,0.2,0.5));   // rose
        s8.setCoord(vec3.fromValues(-1,+1,-1)); s8.setColor(vec3.fromValues(1.0,0.6,0.0));   // orange

        // mettre à jour la géométrie : normales...
        m_Mesh.computeNormals();


        /// Transformations

        // module de transformation et de calculs physique
        MeshModuleProcessing transformation = new MeshModuleProcessing(m_Mesh);
        MeshModulePhysics physique = new MeshModulePhysics(m_Mesh);

        // vérifier que tout est ok
        transformation.checkMesh(false);

        // calculer le volume et le centre de gravité
        m_Mesh.computeNormals();
        physique.setDensity(1.0f);
        physique.compVolumeIntegrals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    public Mesh getMesh()
    {
        return m_Mesh;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
