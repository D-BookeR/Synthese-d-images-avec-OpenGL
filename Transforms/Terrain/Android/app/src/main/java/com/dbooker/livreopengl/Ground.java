package com.dbooker.livreopengl;

import android.util.Log;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe Ground
public class Ground
{
    private static final String TAG = "LivreOpenGL";

    private Mesh m_Mesh;
    private VBOset m_VBOset;

    /**
     * Cette fonction définit la classe Ground.
     * @param points_count : nombre de points à créer en X et en Z
     * @param material : matériau déformant à employer
     * @throws Exception
     */
    public Ground(int points_count, GroundMaterial material) throws Exception
    {
        // test sur la taille du terrain
        // WebGL pose une grosse limite sur les paramètres de gl.drawElements : des ushort uniquement, pas de uint
        if (points_count > 256) {
            Log.w(TAG, "Ground : nombre de points trop grand, pas plus de 256");
            points_count = 256;
        }

        // créer le maillage : une grille hexagonale
        m_Mesh = new Mesh("Ground");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addHexagonalSurface(points_count, points_count, "terrain %d-%d", false, false);

        // parcourir les sommets pour définir leurs coordonnées
        for (int ix=0; ix<points_count; ix++) {
            for (int iz=0; iz<points_count; iz++) {
                // numéro du vertex
                int num = ix + iz*points_count + num0;
                // récupérer le sommet concerné
                MeshVertex vertex = m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                vertex.setCoord(getCoord(points_count, ix, iz));
                vertex.setTexCoord(getTexCoord(points_count, ix, iz));
            }
        }

        // créer le VBOset pour dessiner cet objet (attention, peut être très lent)
        MeshModuleDrawing renderer = new MeshModuleDrawing(m_Mesh);
        m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
     * @param points_count : nombre de points en X et en Z
     * @param ix : numéro du point en X
     * @param iz : numéro du point en Z
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(int points_count, int ix, int iz)
    {
        // le terrain est légèrement rectangulaire à cause de la maille
        float K = (float)Math.sqrt(3.0f)/2.0f;

        // décalage en X selon la parité de iz
        float offset_x = (iz%2) * -0.5f;

        // coordonnées du sommet
        float x = (ix+offset_x) / points_count - 0.5f;
        float y = 0;
        float z = ((iz+0.0f) / points_count - 0.5f) * K;
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode retourne les coordonnées de texture d'un point identifié par (ix, iz)
     * @param points_count : nombre de points en X et en Z
     * @param ix : numéro du point en X
     * @param iz : numéro du point en Z
     * @return vec3 contenant les coordonnées
     */
    private vec2 getTexCoord(int points_count, int ix, int iz)
    {
        // décalage en X selon la parité de iz
        float offset_x = (iz%2) * -0.5f;

        float u = (ix+offset_x)/points_count;
        float v = 1.0f - (iz+0.0f)/points_count;
        return vec2.fromValues(u, v);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_VBOset.destroy();
        m_Mesh.destroy();
    }
}
