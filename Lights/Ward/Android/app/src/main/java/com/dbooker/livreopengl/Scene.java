package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.utils.Utils;



public class Scene extends TurnTableScene
{
    private WardMaterial m_Material;
    private Teapot m_Object;
    private vec4[] m_LightsPositions;
    private vec3[] m_LightsColors;


    /**
     * Constructeur
     */
    public Scene() throws Exception
    {
        super(false);

        // coordonnées des sources de lumières dans le repère global
        m_LightsPositions = new vec4[] {
            vec4.fromValues(10.0, 5.0, 0.0, 1.0)
        };
        int lights_count = m_LightsPositions.length;

        // couleurs des lampes
        m_LightsColors = new vec3[]{
            vec3.fromValues(1.5, 1.5, 1.5)
        };

        // créer le matériau de la théière
        vec3 Kd = vec3.fromValues(0.4, 0.1, 0.0);
        vec3 Ks = vec3.fromValues(0.6, 0.3, 0.1);
        vec2 alpha = vec2.fromValues(0.1, 0.8);
        m_Material = new WardMaterial(lights_count, Kd, Ks, alpha);

        // créer l'objet
        m_Object = new Teapot(m_Material);

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 30.0f;
        m_CameraAngleElevation = 50.0f;
        m_CameraDistance = 12;
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
        mat4.perspective(m_Mat4Projection, Utils.radians(10.0f), (float)width / height, 0.1f, 20.0f);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    @Override
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // fournir la position non transformée des lampes au matériau
        int lights_count = m_LightsPositions.length;
        for (int i=0; i<lights_count; i++) {
            m_Material.setLightPositionColor(i, m_LightsPositions[i], m_LightsColors[i]);
        }

        // dessiner l'objet
        m_Object.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_Object.destroy();
        m_Material.destroy();
        super.destroy();
    }
}
