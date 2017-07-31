// Définition de la classe Rectangle2

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("TextureMaterial");


class Rectangle2
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le maillage
        this.m_Mesh = new Mesh("Rectangle2");

        // sommets
        let B = this.m_Mesh.addVertex("B");
        B.setCoord(vec3.fromValues(-1.0, -1.0, 0.0));
        B.setTexCoord(vec2.fromValues(0.25, 0.0));

        let C = this.m_Mesh.addVertex("C");
        C.setCoord(vec3.fromValues(+1.0, -1.0, 0.0));
        C.setTexCoord(vec2.fromValues(0.75, 0.0));

        let F = this.m_Mesh.addVertex("F");
        F.setCoord(vec3.fromValues(+1.0, +1.0, 0.0));
        F.setTexCoord(vec2.fromValues(0.75, 1.0));

        let G = this.m_Mesh.addVertex("G");
        G.setCoord(vec3.fromValues(-1.0, +1.0, 0.0));
        G.setTexCoord(vec2.fromValues(0.25, 1.0));

        // quadrilatère
        this.m_Mesh.addQuad(B, C, F, G);

        // créer le matériau du rectangle
        this.m_Texture = new Texture2D("data/textures/1024px-LandscapeArchPano.jpg", gl.LINEAR, gl.REPEAT);
        this.m_Material = new TextureMaterial(this.m_Texture);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Material.destroy();
        this.m_Texture.destroy();
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
    }
}
