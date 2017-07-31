// Définition de la classe Cow

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleRedux.js");
Requires("libs/Mesh/MeshModulePhysics.js");
Requires("SkeletonMaterial.js");
Requires("Joint.js");
Requires("JointDebug.js");


class Cow
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le matériau de l'objet, la couleur Kd est définie par sommets
        let Ks = vec3.fromValues(0.1, 0.1, 0.1);
        let Ns = 64;
        this.m_Material = new SkeletonMaterial(Ks, Ns);

        // charger le maillage
        this.m_Mesh = new Mesh("Cow");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15, this, this.onCowLoaded);
        this.m_VBOset = null;

        // définir un os pour gérer le corps
        this.m_BodyJoint = new JointDebug("corps", null);
        this.m_Material.addJoint(this.m_BodyJoint);
        this.m_BodyJoint.setRotationAxis(vec3.fromValues(1.0, 0.0, 0.0));
        this.m_BodyJoint.setPivot(vec3.fromValues(0.0, 1.07, -1.08));
        this.m_BodyJoint.setDirection(vec3.fromValues(0.0, 0.0, 1.59));
        this.m_BodyJoint.setRadiusMinMax(0.5, -2.0, -2.0);    // inclure tous les points

        // définir un os pour gérer le cou
        this.m_NeckJoint = new JointDebug("cou", this.m_BodyJoint);
        this.m_Material.addJoint(this.m_NeckJoint);
        this.m_NeckJoint.setRotationAxis(vec3.fromValues(1.0, 0.0, 0.0));
        this.m_NeckJoint.setPivot(vec3.fromValues(0.0, 1.18, 0.51));
        this.m_NeckJoint.setDirection(vec3.fromValues(0.0, 0.21, 0.43));
        this.m_NeckJoint.setRadiusMinMax(0.45, -0.4, +0.6);

        // définir un os pour gérer la tête
        this.m_HeadJoint = new JointDebug("tête", this.m_NeckJoint);
        this.m_Material.addJoint(this.m_HeadJoint);
        this.m_HeadJoint.setRotationAxis(vec3.fromValues(1.0, 0.0, 0.0));
        this.m_HeadJoint.setPivot(vec3.fromValues(0.0, 1.34, 0.85));
        this.m_HeadJoint.setDirection(vec3.fromValues(0.0, 0.15, 0.44));
        this.m_HeadJoint.setRadiusMinMax(0.5, -0.1, 0.1);

        // définir un os pour gérer la queue
        this.m_TailJoint1 = new JointDebug("queue1", this.m_BodyJoint);
        this.m_Material.addJoint(this.m_TailJoint1);
        this.m_TailJoint1.setRotationAxis(vec3.fromValues(0.0, 0.0, 1.0));
        this.m_TailJoint1.setPivot(vec3.fromValues(0.0, 1.40, -1.29));
        this.m_TailJoint1.setDirection(vec3.fromValues(0.0, -0.22, -0.06));
        this.m_TailJoint1.setRadiusMinMax(0.15, -0.2, +0.6);

        // définir un os pour gérer la queue
        this.m_TailJoint2 = new JointDebug("queue2", this.m_TailJoint1);
        this.m_Material.addJoint(this.m_TailJoint2);
        this.m_TailJoint2.setRotationAxis(vec3.fromValues(0.0, 0.0, 1.0));
        this.m_TailJoint2.setPivot(vec3.fromValues(0.0, 1.10, -1.32));
        this.m_TailJoint2.setDirection(vec3.fromValues(0.0, -0.40, -0.10));
        this.m_TailJoint2.setRadiusMinMax(0.18, -0.2, +1.0);
    }


    /**
     * cette méthode est appelée quand le maillage est chargé
     * @param that : objet Cow en cours de chargement, this ne pouvant pas être affecté
     */
    onCowLoaded(that)
    {
        // définir la couleur des sommets
        let Kd = vec4.fromValues(0.2, 0.7, 1.0, 1.0);
        for (let vertex of that.m_Mesh.getVertexList()) {
            vertex.setColor(Kd);
        }

        // calculer les poids (avant de créer le VBOset)
        that.m_Material.computeWeights(that.m_Mesh);

        // mise au point, choisir l'une des jointures
        //that.m_Material.debugPoids(that.m_Mesh, that.m_BodyJoint);
        //that.m_Material.debugPoids(that.m_Mesh, that.m_NeckJoint);
        //that.m_Material.debugPoids(that.m_Mesh, that.m_HeadJoint);
        //that.m_Material.debugPoids(that.m_Mesh, that.m_TailJoint1);
        //that.m_Material.debugPoids(that.m_Mesh, that.m_TailJoint2);

        // créer le VBOset
        let renderer = new MeshModuleDrawing(that.m_Mesh);
        that.m_VBOset = renderer.createStripVBOset(that.m_Material, true);
    }


    /**
     * Fonction de dessin de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner le VBO s'il est prêt
        if (this.m_VBOset != null) {

            // lier les deux rotations
            let angle1 = Math.cos(Utils.Time * 1.0);
            let angle2 = Math.cos(Utils.Time * 0.3);

            // faire tourner le cou
            this.m_NeckJoint.identity();
            this.m_NeckJoint.rotate(Utils.radians(40.0*angle1));
            this.m_NeckJoint.rotateSecondary(Utils.radians(40.0*angle2));

            // faire tourner la tête
            this.m_HeadJoint.identity();
            this.m_HeadJoint.rotate(Utils.radians(30.0*angle1));
            this.m_HeadJoint.rotateSecondary(Utils.radians(30.0*angle2));

            // faire bouger la queue
            angle1 = Math.cos(Utils.Time * 5.0);
            angle2 = Math.cos(Utils.Time * 3.0);
            this.m_TailJoint1.identity();
            this.m_TailJoint1.rotate(Utils.radians(30.0*angle1));
            this.m_TailJoint1.rotateSecondary(Utils.radians(15.0*angle2));
            this.m_TailJoint2.identity();
            this.m_TailJoint2.rotate(Utils.radians(40.0*angle1));
            this.m_TailJoint2.rotateSecondary(Utils.radians(20.0*angle2));

            // calculer toutes les matrices
            this.m_Material.computeGlobalMatrices();

            // dessiner le maillage
            this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
        }

        // affichage des jointures (si ce sont des JointDebug)
        //this.m_BodyJoint.onDraw(mat4Projection, mat4ModelView);
        //this.m_NeckJoint.onDraw(mat4Projection, mat4ModelView);
        //this.m_HeadJoint.onDraw(mat4Projection, mat4ModelView);
        //this.m_TailJoint1.onDraw(mat4Projection, mat4ModelView);
        //this.m_TailJoint2.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_TailJoint2.destroy();
        this.m_TailJoint1.destroy();
        this.m_HeadJoint.destroy();
        this.m_NeckJoint.destroy();
        this.m_BodyJoint.destroy();
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
        this.m_Material.destroy();
    }
}
