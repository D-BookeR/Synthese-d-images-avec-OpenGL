package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.misc.TurnTableScene;
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
    private CubeCut m_Object;
    private Material m_Material;
    private Light m_Light;
    private MeshStructureDraw m_MeshStructureDraw;


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

        // créer l'objet à dessiner
        m_Object = new CubeCut();

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 300.0f;
        m_CameraAngleElevation = 30.0f;
        m_CameraDistance = 8.0f;

        // modes de dessin : dessiner les arêtes par défaut
        m_MeshStructureDraw = new MeshStructureDraw(m_Object.getMesh(), m_Material, null, null, null, 0.5f);
        m_MeshStructureDraw.nextEdgesMode();
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
        mat4.perspective(m_Mat4Projection, Utils.radians(25.0), (float)width / height, 0.1f, 20.0f);
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    @Override
    public void onKeyDown(char code)
    {
        switch (code) {
        case 'T':
            m_MeshStructureDraw.nextTrianglesMode();
            break;
        case 'Y':
            m_MeshStructureDraw.nextEdgesMode();
            break;
        case 'U':
            m_MeshStructureDraw.nextNormalsMode();
            break;
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
        // dessiner l'objet selon les modes demandés
        m_MeshStructureDraw.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_Object.destroy();
        m_MeshStructureDraw.destroy();
        m_Material.destroy();
        m_Light.destroy();
        super.destroy();
    }
}
