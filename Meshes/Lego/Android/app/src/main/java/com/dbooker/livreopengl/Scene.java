package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.misc.MeshStructureDraw;
import com.dbooker.livreopengl.material.Light;
import com.dbooker.livreopengl.material.DeferredShadingMaterial;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.utils.Utils;


public class Scene extends TurnTableScene
{
    private static final String TAG = "LivreOpenGL";

    // objets de la scène
    private Lego4 m_Lego4;
    private Lego8 m_Lego8;
    private Material m_Material;
    private Light m_Light;
    private MeshStructureDraw m_MeshStructureLego4;
    private MeshStructureDraw m_MeshStructureLego8;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        super(false);

        // créer le matériau de l'objet
        vec3 Ks = vec3.fromValues(0.1, 0.1, 0.1);
        float Ns = 64;
        m_Material = new DeferredShadingMaterial(Ks, Ns);

        // définir une lampe ambiante
        m_Light = new Light();
        m_Light.setColor(vec3.fromValues(1.0,1.0,1.0));
        addLight(m_Light);

        // créer les objets à dessiner
        m_Lego4 = new Lego4();
        m_Lego8 = new Lego8();

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 40.0f;
        m_CameraAngleElevation = 40.0f;
        m_CameraDistance = 10.0f;

        // modes de dessin : dessiner les arêtes par défaut
        m_MeshStructureLego4 = new MeshStructureDraw(m_Lego4.getMesh(), m_Material, null, null, null, 0.5f);
        m_MeshStructureLego4.nextEdgesMode();
        m_MeshStructureLego8 = new MeshStructureDraw(m_Lego8.getMesh(), m_Material, null, null, null, 0.5f);
        m_MeshStructureLego8.nextEdgesMode();
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    @Override
    public void onSurfaceChanged(int width, int height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(m_Mat4Projection, Utils.radians(20.0), (float)width / height, 0.1f, 20.0f);
    }


    /** appelée quand on appuie sur une touche du clavier **/
    @Override
    public void onKeyDown(char code)
    {
        switch (code) {
        case 'T':
            m_MeshStructureLego4.nextTrianglesMode();
            m_MeshStructureLego8.nextTrianglesMode();
            break;
        case 'Y':
            m_MeshStructureLego4.nextEdgesMode();
            m_MeshStructureLego8.nextEdgesMode();
            break;
        case 'U':
            m_MeshStructureLego4.nextNormalsMode();
            m_MeshStructureLego8.nextNormalsMode();
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    @Override
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // matrice MV pour dessiner l'objet 1
        mat4 mat4ModelViewObject1 = mat4.create();
        mat4.translate(mat4ModelViewObject1, mat4ModelView, vec3.fromValues(-1.2, 0.0, 0.0));
        mat4.scale(mat4ModelViewObject1, mat4ModelViewObject1, vec3.fromValues(0.6, 0.6, 0.6));

        // dessiner l'objet selon les modes demandés
        m_MeshStructureLego4.onDraw(mat4Projection, mat4ModelViewObject1);

        // matrice MV pour dessiner l'objet 2
        mat4 mat4ModelViewObject2 = mat4.create();
        mat4.translate(mat4ModelViewObject2, mat4ModelView, vec3.fromValues(+1.2, 0.0, 0.0));
        mat4.scale(mat4ModelViewObject2, mat4ModelViewObject2, vec3.fromValues(0.4, 0.4, 0.4));

        // dessiner l'objet selon les modes demandés
        m_MeshStructureLego8.onDraw(mat4Projection, mat4ModelViewObject2);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_MeshStructureLego8.destroy();
        m_MeshStructureLego4.destroy();
        m_Light.destroy();
        m_Material.destroy();
        m_Lego8.destroy();
        m_Lego4.destroy();
        super.destroy();
    }
}
