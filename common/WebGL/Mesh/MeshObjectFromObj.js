// Définition de la classe MeshObjectFromObj

Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshObject.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/Material.js");


/**
 * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
 */
class MeshObjectFromObj extends MeshObject
{
    /**
     * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
     * @param folder : chemin d'accès aux deux fichiers obj et mtl
     * @param objfilename : nom du fichier .obj
     * @param material : nom du fichier .mtl ou un matériau
     * @param scale : échelle à appliquer sur le maillage lors du chargement
     * @param callback fonction à appeler quand le maillage sera prêt, param et this lui seront passés en paramètre
     * @param param : paramètre à passer à la callback
     */
    constructor(folder, objfilename, material, scale=1.0, callback=null, param=null)
    {
        super();

        // paramètres
        this.m_Folder = folder;
        this.m_ObjFilename = objfilename;
        this.m_ScaleFactor = scale;
        this.m_Callback = callback;
        this.m_CallbackParam = param;

        // si matériau est une chaîne, alors c'est le nom du fichier MTL à charger avant les maillages
        if (typeof(material) == "string") {

            this.m_Materials = null;

            // lance le chargement asynchrone des matériaux du fichier MTL
            Material.loadMTL(this.m_Folder, material, this.onMtlLoaded, this);
            // quand le chargement sera fini, ça lancera le chargement des maillages

        } else {

            this.m_Materials = {};

            // on a fourni un matériau, le seul du maillage, charger le maillage
            let matname = material.getName();
            this.m_Materials[matname] = material;
            // créer un maillage pour ce matériau
            let mesh = new Mesh(this.m_ObjFilename+"@"+matname);
            this.m_Meshes[matname] = mesh;
            // module de chargement
            let loader = new MeshModuleLoading(mesh);
            loader.loadObjFile(this.m_Folder+"/"+this.m_ObjFilename, "", this.m_ScaleFactor, this, this.onObjLoaded, matname);
            // les VBOset seront créés quand les maillages seront chargés
            this.m_VBOsets[matname] = null;
        }
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * cette fonction est appelée quand les matériaux ont été chargés
     * @param that : instance de l'objet qui est en cours de chargement
     * @param materials : collection de (nom, matériau) à appliquer au maillage
     */
    onMtlLoaded(that, materials)
    {
        // charger les maillages, un par matériau
        that.m_Materials = materials;
        for (let matname in materials) {
            // créer un maillage pour ce matériau
            let material = materials[matname];
            let mesh = new Mesh(that.m_ObjFilename+"@"+matname);
            that.m_Meshes[matname] = mesh;
            // module de chargement
            let loader = new MeshModuleLoading(mesh);
            loader.loadObjFile(that.m_Folder+"/"+that.m_ObjFilename, matname, that.m_ScaleFactor, that, that.onObjLoaded, matname);
            // son VBOset sera créé quand le maillage sera chargé
            that.m_VBOsets[matname] = null;
        }
    }


    /**
     * cette fonction est appelée quand un maillage a été chargé
     * @param that : instance de l'objet qui est en cours de chargement
     * @param matname : nom du matériau en cours de chargement
     */
    onObjLoaded(that, matname)
    {
        // maillage
        let mesh = that.m_Meshes[matname];

        // module de dessin
        let renderer = new MeshModuleDrawing(mesh);

        // créer les VBOsets
        that.m_VBOsets[matname] = renderer.createStripVBOset(that.m_Materials[matname], true);

        // l'objet est-il prêt : tous ses maillages chargés ?
        that.m_Ready = true;
        for (let matname in that.m_VBOsets) {
            if (that.m_VBOsets[matname] == null) that.m_Ready = false;
        }

        // appeler la callback s'il y en a une et si le maillage est prêt
        if (that.m_Ready && that.m_Callback != null) {
            that.m_Callback(that.m_CallbackParam, that);
        }
    }
}
