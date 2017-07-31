package com.dbooker.livreopengl;


import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.mesh.MeshTriangle;
import com.dbooker.livreopengl.mesh.MeshVertex;


public class CowPeaks
{
    private Mesh m_Mesh;

    /**
     * remplace chaque triangle du maillage par trois triangles appuyés sur
     * le barycentre de l'ancien triangle
     * @param mesh : maillage concerné par l'opération
     * @param height : amplitude relative du déplacement du barycentre
     */
    private void subdivideTriangles(Mesh mesh, float height) throws Exception
    {
        // calculer les normales des triangles et des sommets
        mesh.computeNormals();

        // copie de la liste du maillage
        @SuppressWarnings("unchecked")
        ArrayList<MeshTriangle> triangles = (ArrayList<MeshTriangle>) mesh.getTriangleList().clone();

        // traiter tous les triangles
        for (MeshTriangle triangle: triangles) {
            // triangle courant et ses sommets
            MeshVertex s0 = triangle.getVertex0();
            MeshVertex s1 = triangle.getVertex1();
            MeshVertex s2 = triangle.getVertex2();
            // rajouter un sommet au milieu des trois
            MeshVertex center = mesh.addVertex("c"+s0.getName()+s1.getName()+s2.getName());
            // ses coordonnées
            vec3 coords = vec3.clone(triangle.getCenter());
            // amplitude du déplacement
            float dist = (float) (Math.sqrt(triangle.getSurface()) * height);
            // déplacer le centre en suivant la normale
            vec3 displacement = vec3.create();
            vec3.scale(displacement, triangle.getNormal(), dist);
            vec3.add(coords, coords, displacement);
            center.setCoord(coords);
            // supprimer l'ancien triangle
            triangle.destroy();
            // construire trois triangles
            mesh.addTriangle(center, s0, s1);
            mesh.addTriangle(center, s1, s2);
            mesh.addTriangle(center, s2, s0);
        }
    }

    public CowPeaks() throws Exception
    {
        // créer le maillage de la vache
        m_Mesh = new Mesh();
        MeshModuleLoading loader = new MeshModuleLoading(m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15f);

        // calculer les normales des triangles et des sommets
        m_Mesh.computeNormals();

        // appliquer une subdivision
        subdivideTriangles(m_Mesh, 1.0f);

        // définir la couleur de tous les sommets
        for (int i=0; i<m_Mesh.getVertexCount(); i++) {
            m_Mesh.getVertex(i).setColor(vec3.fromValues(0.9, 1.0, 0.2));
        }

        // calculer les normales (elles ne sont pas bonnes car interpolées)
        m_Mesh.computeNormals();
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
