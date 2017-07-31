package com.dbooker.livreopengl;


import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshTriangle;
import com.dbooker.livreopengl.mesh.MeshVertex;

public class Disk
{
    private Mesh m_Mesh;


    public Disk() throws Exception
    {
        // créer une sorte de disque convexe

        m_Mesh = new Mesh();

        /// Topologie : éléments et connexions

        final int N = 6;

        // rajouter des sommets
        for (int i=0; i<=N; i++) {
            m_Mesh.addVertex("s"+i);
        }

        // rajouter des triangles autour du premier sommet
        for (int i=0; i<N; i++) {
            m_Mesh.addTriangle(
                    m_Mesh.getVertex(0),
                    m_Mesh.getVertex(i+1),
                    m_Mesh.getVertex((i+1)%N + 1));
        }

        /// Géométrie : position des sommets et autres infos

        // affectation des coordonnées des sommets
        float R = 1.6f;
        m_Mesh.getVertex(0).setCoord(vec3.fromValues( 0.0,+0.5 * R, 0.0));
        for (int i=0; i<N; i++) {
            float angle = (float) (i * 2.0f * Math.PI / N);
            m_Mesh.getVertex(i+1).setCoord(vec3.fromValues(R * Math.sin(angle), 0.0, R * Math.cos(angle)));
        }
        m_Mesh.getVertex(0).setColor(vec3.fromValues(0.2, 0.5, 0.3));

        // appliquer une subdivision
        subdivideTriangles(m_Mesh, 0.05f);

        // calculer les normales
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


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
