package com.dbooker.livreopengl.misc;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.material.DeferredShadingMaterial;
import com.dbooker.livreopengl.material.Material;

/**
 * Ce module permet de dessiner les arêtes et normales d'un maillage
 *
 */


@SuppressWarnings("unused")
public class MeshStructureDraw
{
    private static final String TAG = "LivreOpenGL";

    private Mesh m_Mesh;
    private Material m_MatTriangles;
    private float m_NormalsLength;
    private int m_DrawTriangles;
    private boolean m_DrawEdges;
    private int m_DrawNormals;
    private VBOset m_VBOsetTriangles;
    private VBOset m_VBOsetEdges;
    private VBOset m_VBOsetFaceNormals;
    private VBOset m_VBOsetVertexNormals;
    private Material m_EdgesMaterial;
    private Material m_FaceNormalsMaterial;
    private Material m_VertexNormalsMaterial;


    /**
     * Définition de la classe MeshStructureDraw
     * Cette classe permet de dessiner la structure d'un maillage
     * - triangles : visibles, opaques ou absents
     * - arêtes : visibles ou non
     * - normales de triangles et de sommets : visibles ou non
     * Voir les méthodes nextTrianglesMode, nextEdgesMode et nextNormalsMode
     * @param mesh : maillage à gérer avec ce renderer
     * @param trianglesMaterial : matériau pour dessiner les triangles
     * @param edgesColor : vec3 donnant la couleur pour dessiner les arêtes, peut être null
     * @param faceNormalsColor : peut être null, vec3 donnant la couleur des normales de triangles
     * @param vertexNormalsColor : peut être null, vec3 donnant la couleur des normales de sommets
     * @param length : longueur des normales, dépend de la dimension du maillage, 1.0 par défaut
     * @throws Exception
     */
    public MeshStructureDraw(Mesh mesh, Material trianglesMaterial, vec4 edgesColor, vec4 faceNormalsColor, vec4 vertexNormalsColor, float length) throws Exception
    {
        // maillage et matériau gérés
        m_Mesh = mesh;
        m_MatTriangles = trianglesMaterial;
        m_NormalsLength = length;

        // options initiales
        m_DrawTriangles = 1;
        m_DrawEdges = false;
        m_DrawNormals = 0;

        // créer le matériau pour les arêtes
        if (edgesColor == null) {
            m_EdgesMaterial = new DeferredShadingMaterial(vec4.fromValues(1.0, 1.0, 1.0, 1.0));
        } else {
            m_EdgesMaterial = new DeferredShadingMaterial(edgesColor);
        }

        // créer le matériau pour les normales de triangles
        if (faceNormalsColor == null) {
            m_FaceNormalsMaterial = new DeferredShadingMaterial(vec4.fromValues(1.0, 0.0, 0.0, 1.0));
        } else {
            m_FaceNormalsMaterial = new DeferredShadingMaterial(faceNormalsColor);
        }

        // créer le matériau pour les normales de sommets
        if (vertexNormalsColor == null) {
            m_VertexNormalsMaterial = new DeferredShadingMaterial(vec4.fromValues(0.0, 1.0, 0.0, 1.0));
        } else {
            m_VertexNormalsMaterial = new DeferredShadingMaterial(vertexNormalsColor);
        }

        // VBOset pour dessiner les triangles, les arêtes et les normales, voir onDraw
        // la création est retardée pour ne créer que le strict nécessaire
        m_VBOsetTriangles = null;
        m_VBOsetEdges = null;
        m_VBOsetFaceNormals = null;
        m_VBOsetVertexNormals = null;
    }


    /**
     * Cette méthode indique d'utiliser le mode suivant pour les triangles
     * - triangles dessinés normalement
     * - triangles comme s'ils étaient opaques mais pas dessinés en couleur
     * - triangles pas dessinés du tout
     */
    public void nextTrianglesMode()
    {
        // incrémenter le mode modulo 3 : 0, 1, 2, 0...
        m_DrawTriangles = (m_DrawTriangles + 1) % 3;
        // forcer le mode 1 s'il n'y a pas d'arêtes
        if (!m_DrawEdges) m_DrawTriangles = 1;
    }


    /**
     * Cette méthode fait utiliser le mode suivant pour les arêtes
     * - pas d'arêtes, dans ce cas, ça force l'affichage des triangles normalement
     * - arêtes dessinées devant les triangles.
     */
    public void nextEdgesMode()
    {
        // inverser l'affichage des arêtes
        m_DrawEdges = !m_DrawEdges;
        // forcer le mode triangles s'il n'y a pas d'arêtes
        if (!m_DrawEdges) m_DrawTriangles = 1;
    }


    /** Cette méthode demande de dessiner les normales
     * - aucune normale
     * - normales de triangles (en rouge)
     * - normales de sommets (en vert)
     * - les deux types de normales
     */
    public void nextNormalsMode()
    {
        // incrémenter le mode de dessin des normales modulo 4 : 0, 1, 2, 3, 0...
        m_DrawNormals = (m_DrawNormals + 1) % 4;
    }


    /**
     * dessin avec les modes configurés
     * @param mat4Projection : mat4 de projection des sommets
     * @param mat4ModelView : mat4 de positionnement de l'objet dans la scène
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // module de dessin
        MeshModuleDrawing renderer = new MeshModuleDrawing(m_Mesh);

        // créer les VBO qui manquent par rapport aux modes de dessin demandés
        if (m_DrawTriangles > 0 && m_VBOsetTriangles == null) {
            m_VBOsetTriangles = renderer.createStripVBOset(m_MatTriangles, true);
            if (m_VBOsetTriangles == null) return;
        }
        if (m_DrawEdges && m_VBOsetEdges == null) {
            // créer le VBO de dessin des arêtes
            m_VBOsetEdges = renderer.createEdgesVBOset(m_EdgesMaterial);
            if (m_VBOsetEdges == null) return;
        }
        if (m_DrawNormals%2 == 1 && m_VBOsetFaceNormals == null) {
            // créer le VBO de dessin des normales de facettes
            m_VBOsetFaceNormals = renderer.createFacesNormalsVBOset(m_FaceNormalsMaterial, m_NormalsLength);
            if (m_VBOsetFaceNormals == null) return;
        }
        if (m_DrawNormals >= 2 && m_VBOsetVertexNormals == null) {
            // créer le VBO de dessin des normales de sommets
            m_VBOsetVertexNormals = renderer.createVertexNormalsVBOset(m_VertexNormalsMaterial, m_NormalsLength);
            if (m_VBOsetVertexNormals == null) return;
        }

        // préparer le dessin des arêtes
        if (m_DrawEdges) {
            // mettre en place un décalage de polygone
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f, 1.0f);
        }

        // selon le mode de dessin des triangles
        switch (m_DrawTriangles) {
            case 1:
                // dessiner les triangles
                m_VBOsetTriangles.onDraw(mat4Projection, mat4ModelView);
                break;
            case 2:
                // ne pas dessiner sur l'écran (seulement sur le Depth Buffer)
                glColorMask(false, false, false, false);
                // dessiner les triangles
                m_VBOsetTriangles.onDraw(mat4Projection, mat4ModelView);
                // dessiner à nouveau sur l'écran
                glColorMask(true, true, true, true);
                break;
        }

        if (m_DrawEdges) {
            // supprimer le décalage
            glPolygonOffset(0, 0);
            glDisable(GL_POLYGON_OFFSET_FILL);

            // dessiner les arêtes
            glLineWidth(1.2f);
            m_VBOsetEdges.onDraw(mat4Projection, mat4ModelView);
        }

        if (m_DrawNormals%2 == 1 && m_VBOsetFaceNormals != null) {
            m_VBOsetFaceNormals.onDraw(mat4Projection, mat4ModelView);
        }

        if (m_DrawNormals >= 2 && m_VBOsetVertexNormals != null) {
            m_VBOsetVertexNormals.onDraw(mat4Projection, mat4ModelView);
        }
    }


    /** destructeur */
    public void destroy()
    {
        if (m_EdgesMaterial != null)
            m_EdgesMaterial.destroy();
        if (m_FaceNormalsMaterial != null)
            m_FaceNormalsMaterial.destroy();
        if (m_VertexNormalsMaterial != null)
            m_VertexNormalsMaterial.destroy();

        if (m_VBOsetTriangles != null)
            m_VBOsetTriangles.destroy();
        if (m_VBOsetEdges != null)
            m_VBOsetEdges.destroy();
        if (m_VBOsetFaceNormals != null)
            m_VBOsetFaceNormals.destroy();
        if (m_VBOsetVertexNormals != null)
            m_VBOsetVertexNormals.destroy();
    }
}
