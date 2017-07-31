// Définition de la classe Cow

// superclasses et classes nécessaires
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Material/ColorMaterial.js");
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleProcessing.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class CowEdges
{
    constructor(material)
    {
        // créer le matériau pour les triangles
        let Kd = vec4.fromValues(0.9, 0.9, 0.9, 1.0);
        let Ks = vec3.fromValues(0.1, 0.1, 0.1);
        let Ns = 64;
        this.m_TrianglesMaterial = new DeferredShadingMaterial(Kd, Ks, Ns);
        // remarque : il n'y a pas de lampe dans la scène, alors le matériau semblera plat

        // créer le matériau pour les arêtes
        let black = vec4.fromValues(0.0, 0.0, 0.0, 1.0);
        this.m_EdgesMaterial = new ColorMaterial(black);

        // créer le maillage de la vache
        this.m_Mesh = new Mesh("CowEdges");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15, this, this.onCowEdgesLoaded, 0);

        this.m_VBOsetTriangles = null;
        this.m_VBOsetEdges = null;
    }


    onCowEdgesLoaded(that, param)
    {
        // en option : subdiviser le maillage en lissant ou non
        let transformation = new MeshModuleProcessing(that.m_Mesh);
        that.m_Mesh.computeNormals();
        transformation.subdivideAll(that.m_Mesh.getTriangleList(), 1, 1.25);

        // module de dessin
        let renderer = new MeshModuleDrawing(that.m_Mesh);

        // finaliser l'objet : construire ses VBOs
        that.m_Mesh.computeNormals();
        that.m_VBOsetTriangles = renderer.createStripVBOset(that.m_TrianglesMaterial, true);

        // créer la liste d'arêtes et les VBOs pour les afficher
        that.m_VBOsetEdges = renderer.createEdgesVBOset(that.m_EdgesMaterial);
    }


    onDraw(mat4Projection, mat4ModelView)
    {
        let triangles = true;            // si true : remplit les triangles => opaques
        let background_color = false;    // si true : les triangles sont de la couleur du fond

        if (this.m_VBOsetTriangles == null) return;
        if (this.m_VBOsetEdges == null) aretes = false;

        // mettre en place un décalage de polygone
        gl.enable(gl.POLYGON_OFFSET_FILL);
        gl.polygonOffset(1.0, 1.0);

        // dessiner les triangles uniquement dans le depth buffer
        if (triangles) {

            // en option : bloquer les dessins visibles, mais garder l'écriture du depth buffer
            // ça permet d'avoir une vache toujours de la même couleur que le fond
            if (background_color) gl.colorMask(false, false, false, false);

            // dessiner les triangles mais ils seront invisibles
            this.m_VBOsetTriangles.onDraw(mat4Projection, mat4ModelView);

            // ré-auroriser les dessins sur l'écran
            gl.colorMask(true, true, true, true);
        }

        // supprimer le décalage
        gl.polygonOffset(0.0, 0.0);
        gl.disable(gl.POLYGON_OFFSET_FILL);

        // dessiner les arêtes
        this.m_VBOsetEdges.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_VBOsetTriangles.destroy();
        this.m_VBOsetEdges.destroy();
        this.m_Mesh.destroy();
        this.m_EdgesMaterial.destroy();
        this.m_TrianglesMaterial.destroy();
    }
}
