package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.misc.MeshStructureDraw;
import com.dbooker.livreopengl.misc.AxesXYZ;
import com.dbooker.livreopengl.material.OmniLight;
import com.dbooker.livreopengl.material.DeferredShadingMaterial;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.utils.Utils;


public class Scene extends TurnTableScene
{
    private Disk m_Object;
    private AxesXYZ m_Axes;
    private OmniLight m_Light0;
    private OmniLight m_Light1;
    private OmniLight m_Light2;
    private Material m_Material;
    private MeshStructureDraw m_MeshStructureDraw;


    public Scene() throws Exception
    {
        super(true);

        // créer le matériau de l'objet
        vec4 Kd = vec4.fromValues(0.8, 0.4, 0.2, 1.0);
        vec3 Ks = vec3.fromValues(0.9, 0.9, 0.9);
        float Ns = 128;
        m_Material = new DeferredShadingMaterial(Kd, Ks, Ns);

        // définir une lampe directionnelle
        m_Light0 = new OmniLight();
        m_Light0.setPosition(vec4.fromValues(1, 1, 1, 0));
        m_Light0.setColor(vec3.fromValues(0.8,0.8,0.8));
        addLight(m_Light0);

        // définir une lampe positionnelle
        m_Light1 = new OmniLight();
        m_Light1.setPosition(vec4.fromValues(-10, 5, 5, 1));
        m_Light1.setColor(vec3.fromValues(100,100,100));
        addLight(m_Light1);

        // définir une lampe directionnelle
        m_Light2 = new OmniLight();
        m_Light2.setPosition(vec4.fromValues(-2, 1, -2, 0));
        m_Light2.setColor(vec3.fromValues(0.4,0.4,0.4));
        addLight(m_Light2);

        // créer les objets
        m_Object = new Disk(24);
        m_Axes = new AxesXYZ(1.0f, 1.0f, 1.0f);

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 125.0f;
        m_CameraAngleElevation = 45.0f;
        m_CameraDistance = 8.0f;

        // modes de dessin
        m_MeshStructureDraw = new MeshStructureDraw(m_Object.getMesh(), m_Material, null, null, null, 0.25f);
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
        mat4.perspective(m_Mat4Projection, Utils.radians(15.0f), (float)width / height, 0.1f, 20.0f);
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
        m_MeshStructureDraw.destroy();
        m_Light0.destroy();
        m_Light1.destroy();
        m_Light2.destroy();
        m_Material.destroy();
        m_Axes.destroy();
        m_Object.destroy();
        super.destroy();
    }
}
