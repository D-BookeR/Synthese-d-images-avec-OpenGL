package com.dbooker.livreopengl;


import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.utils.Utils;

public class Cow
{
    private Mesh m_Mesh;
    private SkeletonMaterial m_Material;
    private VBOset m_VBOset;
    private Joint m_BodyJoint;
    private Joint m_NeckJoint;
    private Joint m_HeadJoint;
    private JointDebug m_TailJoint1;
    private JointDebug m_TailJoint2;


    /**
     * Constructeur
     */
    public Cow() throws Exception
    {
        // créer le matériau de l'objet, la couleur Kd est définie par sommets
        vec3 Ks = vec3.fromValues(0.1, 0.1, 0.1);
        float Ns = 64;
        m_Material = new SkeletonMaterial(Ks, Ns);

        // charger le maillage
        m_Mesh = new Mesh("Cow");
        MeshModuleLoading loader = new MeshModuleLoading(m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15f);

        // définir un os pour gérer le corps
        m_BodyJoint = new JointDebug("corps", null);
        m_Material.addJoint(m_BodyJoint);
        m_BodyJoint.setRotationAxis(vec3.fromValues(1.0, 0.0, 0.0));
        m_BodyJoint.setPivot(vec3.fromValues(0.0, 1.07, -1.08));
        m_BodyJoint.setDirection(vec3.fromValues(0.0, 0.0, 1.59));
        m_BodyJoint.setRadiusMinMax(0.5f, -2.0f, -2.0f);    // inclure tous les points

        // définir un os pour gérer le cou
        m_NeckJoint = new JointDebug("cou", m_BodyJoint);
        m_Material.addJoint(m_NeckJoint);
        m_NeckJoint.setRotationAxis(vec3.fromValues(1.0, 0.0,0.0));
        m_NeckJoint.setPivot(vec3.fromValues(0.0, 1.18, 0.51));
        m_NeckJoint.setDirection(vec3.fromValues(0.0, 0.21, 0.43));
        m_NeckJoint.setRadiusMinMax(0.45f, -0.4f, +0.6f);

        // définir un os pour gérer la tête
        m_HeadJoint = new JointDebug("tête", m_NeckJoint);
        m_Material.addJoint(m_HeadJoint);
        m_HeadJoint.setRotationAxis(vec3.fromValues(1.0, 0.0, 0.0));
        m_HeadJoint.setPivot(vec3.fromValues(0.0, 1.34, 0.85));
        m_HeadJoint.setDirection(vec3.fromValues(0.0, 0.15, 0.44));
        m_HeadJoint.setRadiusMinMax(0.5f, -0.1f, 0.1f);

        // définir un os pour gérer la queue
        m_TailJoint1 = new JointDebug("queue1", m_BodyJoint);
        m_Material.addJoint(m_TailJoint1);
        m_TailJoint1.setRotationAxis(vec3.fromValues(0.0, 0.0, 1.0));
        m_TailJoint1.setPivot(vec3.fromValues(0.0, 1.40, -1.29));
        m_TailJoint1.setDirection(vec3.fromValues(0.0, -0.22, -0.06));
        m_TailJoint1.setRadiusMinMax(0.15f, -0.2f, +0.6f);

        // définir un os pour gérer la queue
        m_TailJoint2 = new JointDebug("queue2", m_TailJoint1);
        m_Material.addJoint(m_TailJoint2);
        m_TailJoint2.setRotationAxis(vec3.fromValues(0.0, 0.0, 1.0));
        m_TailJoint2.setPivot(vec3.fromValues(0.0, 1.10, -1.32));
        m_TailJoint2.setDirection(vec3.fromValues(0.0, -0.40, -0.10));
        m_TailJoint2.setRadiusMinMax(0.18f, -0.2f, +1.0f);

        // définir la couleur des sommets
        vec4 Kd = vec4.fromValues(0.2, 0.7, 1.0, 1.0);
        for (MeshVertex vertex: m_Mesh.getVertexList()) {
            vertex.setColor(Kd);
        }

        // calculer les poids (avant de créer le VBOset)
        m_Material.computeWeights(m_Mesh);

        // mise au point, choisir l'une des jointures
        //m_Material.debugPoids(m_Mesh, m_BodyJoint);
        //m_Material.debugPoids(m_Mesh, m_NeckJoint);
        //m_Material.debugPoids(m_Mesh, m_HeadJoint);
        //m_Material.debugPoids(m_Mesh, m_TailJoint1);
        //m_Material.debugPoids(m_Mesh, m_TailJoint2);

        // créer le VBOset
        MeshModuleDrawing renderer = new MeshModuleDrawing(m_Mesh);
        m_VBOset = renderer.createStripVBOset(m_Material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // lier les deux rotations
        float angle1 = (float)Math.cos(Utils.Time * 1.0);
        float angle2 = (float)Math.cos(Utils.Time * 0.3);

        // faire tourner le cou
        m_NeckJoint.identity();
        m_NeckJoint.rotate(Utils.radians(40.0f*angle1));
        m_NeckJoint.rotateSecondary(Utils.radians(40.0f*angle2));

        // faire tourner la tête
        m_HeadJoint.identity();
        m_HeadJoint.rotate(Utils.radians(30.0f*angle1));
        m_HeadJoint.rotateSecondary(Utils.radians(30.0f*angle2));

        // faire bouger la queue
        angle1 = (float)Math.cos(Utils.Time * 5.0);
        angle2 = (float)Math.cos(Utils.Time * 3.0);
        m_TailJoint1.identity();
        m_TailJoint1.rotate(Utils.radians(30.0f*angle1));
        m_TailJoint1.rotateSecondary(Utils.radians(15.0f*angle2));
        m_TailJoint2.identity();
        m_TailJoint2.rotate(Utils.radians(40.0f*angle1));
        m_TailJoint2.rotateSecondary(Utils.radians(20.0f*angle2));

        // calculer toutes les matrices
        m_Material.computeGlobalMatrices();

        // dessiner le maillage
        m_VBOset.onDraw(mat4Projection, mat4ModelView);

        // affichage des jointures (si ce sont des JointDebug)
        //m_BodyJoint.onDraw(mat4Projection, mat4ModelView);
        //m_NeckJoint.onDraw(mat4Projection, mat4ModelView);
        //m_HeadJoint.onDraw(mat4Projection, mat4ModelView);
        //m_TailJoint1.onDraw(mat4Projection, mat4ModelView);
        //m_TailJoint2.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_TailJoint2.destroy();
        m_TailJoint1.destroy();
        m_HeadJoint.destroy();
        m_NeckJoint.destroy();
        m_BodyJoint.destroy();
        m_VBOset.destroy();
        m_Mesh.destroy();
        m_Material.destroy();
    }
}
