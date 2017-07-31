// Définition de la classe Pyramid

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("ColorPerVertexMaterial");
Requires("RedMaterial");
Requires("GreenMaterial");
Requires("ProceduralMaterial");


class Pyramid
{
    /**
     * Constructeur
     */
    constructor()
    {
        // matériaux utilisés
        this.m_ColorMaterial = new ColorPerVertexMaterial();
        this.m_RedMaterial = new RedMaterial();
        this.m_GreenMaterial = new GreenMaterial();
        this.m_ProceduralMaterial = new ProceduralMaterial();

        // liste des VBOsets de l'objet
        this.m_VBOsets = [];

        // module de dessin
        let renderer = new MeshModuleDrawing();

        // coordonnées des sommets
        let coordsA = vec3.fromValues( 0.0, 1.5, 0.0);
        let coordsB = vec3.fromValues( 1.0,-1.0, 1.5);
        let coordsC = vec3.fromValues( 1.0,-1.0,-1.5);
        let coordsD = vec3.fromValues(-1.0,-1.0, 0.0);

        // couleurs des sommets (pour le matériau ColorPerVertexMaterial)
        let colorA = vec3.fromValues(0.0, 1.0, 1.0);
        let colorB = vec3.fromValues(0.0, 0.0, 1.0);
        let colorC = vec3.fromValues(1.0, 0.0, 1.0);
        let colorD = vec3.fromValues(1.0, 0.0, 0.0);

        // coordonnées de texture (pour le matériau ProceduralMaterial)
        let texcoords00 = vec2.fromValues(0.0, 0.0);
        let texcoords10 = vec2.fromValues(1.0, 0.0);
        let texcoords11 = vec2.fromValues(0.5, 1.0);

        // variables temporaires représentant les sommets
        let A,B,C,D;

        let colorMatMesh = new Mesh("CouleurVertex");
        A = colorMatMesh.addVertex("a").setCoord(coordsA).setColor(colorA);
        B = colorMatMesh.addVertex("b").setCoord(coordsB).setColor(colorB);
        C = colorMatMesh.addVertex("c").setCoord(coordsC).setColor(colorC);
        colorMatMesh.addTriangle(A, B, C);
        renderer.setMesh(colorMatMesh);
        this.m_VBOsets.push(renderer.createStripVBOset(this.m_ColorMaterial, true));

        let redMatMesh = new Mesh("Rouge");
        D = redMatMesh.addVertex("d").setCoord(coordsD);
        C = redMatMesh.addVertex("c").setCoord(coordsC);
        B = redMatMesh.addVertex("b").setCoord(coordsB);
        redMatMesh.addTriangle(D, C, B);
        renderer.setMesh(redMatMesh);
        this.m_VBOsets.push(renderer.createStripVBOset(this.m_RedMaterial, true));

        let greenMatMesh = new Mesh("Vert");
        D = greenMatMesh.addVertex("d").setCoord(coordsD);
        B = greenMatMesh.addVertex("b").setCoord(coordsB);
        A = greenMatMesh.addVertex("a").setCoord(coordsA);
        greenMatMesh.addTriangle(D, B, A);
        renderer.setMesh(greenMatMesh);
        this.m_VBOsets.push(renderer.createStripVBOset(this.m_GreenMaterial, true));

        let proceduralMatMesh = new Mesh("Procedural");
        D = proceduralMatMesh.addVertex("d").setCoord(coordsD).setTexCoord(texcoords00);
        A = proceduralMatMesh.addVertex("a").setCoord(coordsA).setTexCoord(texcoords10);
        C = proceduralMatMesh.addVertex("c").setCoord(coordsC).setTexCoord(texcoords11);
        proceduralMatMesh.addTriangle(D, A, C);
        renderer.setMesh(proceduralMatMesh);
        this.m_VBOsets.push(renderer.createStripVBOset(this.m_ProceduralMaterial, true));

        // suppression des objets temporaires dynamiquement alloués
        colorMatMesh.destroy();
        redMatMesh.destroy();
        greenMatMesh.destroy();
        proceduralMatMesh.destroy();
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner tous ses VBOsets
        for (let vboset of this.m_VBOsets) {
            vboset.onDraw(mat4Projection, mat4ModelView);
        }
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        for (let vboset of this.m_VBOsets) {
            vboset.destroy();
        }
        this.m_ProceduralMaterial.destroy();
        this.m_GreenMaterial.destroy();
        this.m_RedMaterial.destroy();
        this.m_ColorMaterial.destroy();
    }
}
