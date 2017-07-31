// Définition de la classe Pumpkin

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Mesh/MeshModuleAnimation.js");
Requires("MorphMaterial");


class Pumpkin
{
    /**
     * Constructeur
     */
    constructor()
    {
        // VBOset et matériaux
        this.m_VBOsets = {};
        this.m_Materials = {};

        // maillages à cause de l'asynchronisme
        this.m_Meshes = {};
        this.m_MaillagesMorph1 = {};

        // définir les matériaux
        let Ks = vec3.fromValues(0.165, 0.165, 0.165);
        this.m_Materials["Pumpkin01"] = new MorphMaterial("data/models/Pumpkin/Pumpkin01.jpg", Ks, 64.0);
        this.m_Materials["Pumpkin02"] = new MorphMaterial("data/models/Pumpkin/Pumpkin02.jpg", Ks, 64.0);
        this.m_Materials["Stalk01"] = new MorphMaterial("data/models/Pumpkin/Stalk01.jpg", Ks, 64.0);
        this.m_Materials["Stalk02"] = new MorphMaterial("data/models/Pumpkin/Stalk02.jpg", Ks, 64.0);

        // module de chargement
        let loader = new MeshModuleLoading();

        // charger les maillages, un par matériau
        for (let matname in this.m_Materials) {
            // charger le maillage correspondant à ce matériau
            let mesh = new Mesh("Pumpkin");
            this.m_Meshes[matname] = mesh;
            mesh.setReady(false);
            loader.setMesh(mesh);
            loader.loadObjFile("data/models/Pumpkin/pumpkin.obj", matname, 2.0, this, this.onObjLoaded, matname);
            // les VBOset seront créés quand les maillages seront chargés
            this.m_VBOsets[matname] = null;

            // charger le maillage du morph n°1
            let morph = new Mesh("Pumpkin_morph1");
            this.m_MaillagesMorph1[matname] = morph;
            morph.setReady(false);
            loader.setMesh(morph);
            loader.loadObjFile("data/models/Pumpkin/pumpkin_morph1.obj", matname, 2.0, this, this.onObjLoaded, matname);
        }
    }


    /**
     * cette fonction est appelée quand l'un des maillages a été chargé
     * @param that : instance de l'objet qui est en cours de chargement
     * @param matname : nom du matériau correspondant au maillage
     */
    onObjLoaded(that, matname)
    {
        // il faut que les deux maillages soient chargés
        let mesh = that.m_Meshes[matname];
        let morph = that.m_MaillagesMorph1[matname];
        if (mesh.isReady() && morph.isReady()) {
            // construire le morph
            let animation = new MeshModuleAnimation(mesh);
            animation.buildMorph(morph);

            // construire le VBOset pour dessiner ce maillage
            let renderer = new MeshModuleDrawing(mesh);
            that.m_VBOsets[matname] = renderer.createStripVBOset(that.m_Materials[matname], true);

            // supprimer les maillages
            mesh.destroy();
            morph.destroy();
        }
    }


    /**
     * définit la valeur du morph pour changer la forme de l'objet
     * @param coef entre 0 et 1
     */
    setMorphCoef(coef)
    {
        // fournir le coefficient à tous les matériaux
        for (let matname in this.m_Materials) {
            this.m_Materials[matname].setMorphCoef(coef);
        }
    }


    /**
     * dessine l'objet
     * @param mat4Projection
     * @param mat4ModelView
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // les matériaux ont-ils été chargés
        if (this.m_Materials == null) return;

        // dessiner les maillages
        for (let matname in this.m_VBOsets) {
            let vboset = this.m_VBOsets[matname];
            if (vboset != null) {
                vboset.onDraw(mat4Projection, mat4ModelView);
            }
        }
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        for (let matname in this.m_VBOsets) {
            this.m_VBOsets[matname].destroy();
        }
        for (let matname in this.m_Materials) {
            this.m_Materials[matname].destroy();
        }
    }
}
