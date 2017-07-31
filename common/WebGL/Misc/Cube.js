// Définition de la classe Cube

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class Cube
{
    /**
     * Constructeur
     * @param material matériau à employer
     */
    constructor(material)
    {
        let mesh = new Mesh("Cube");

        /// dessus

        let s1a = mesh.addVertex("s1a");
        let s2a = mesh.addVertex("s2a");
        let s3a = mesh.addVertex("s3a");
        let s4a = mesh.addVertex("s4a");
        mesh.addQuad(s1a, s2a, s3a, s4a);
        s1a.setCoord(vec3.fromValues(-1,+1,+1)); s1a.setTexCoord(vec2.fromValues(0,0)); s1a.setNormal(vec3.fromValues(0,+1,0));
        s2a.setCoord(vec3.fromValues(+1,+1,+1)); s2a.setTexCoord(vec2.fromValues(1,0)); s2a.setNormal(vec3.fromValues(0,+1,0));
        s3a.setCoord(vec3.fromValues(+1,+1,-1)); s3a.setTexCoord(vec2.fromValues(1,1)); s3a.setNormal(vec3.fromValues(0,+1,0));
        s4a.setCoord(vec3.fromValues(-1,+1,-1)); s4a.setTexCoord(vec2.fromValues(0,1)); s4a.setNormal(vec3.fromValues(0,+1,0));

        /// dessous

        let s1b = mesh.addVertex("s1b");
        let s2b = mesh.addVertex("s2b");
        let s3b = mesh.addVertex("s3b");
        let s4b = mesh.addVertex("s4b");
        mesh.addQuad(s4b, s3b, s2b, s1b);
        s1b.setCoord(vec3.fromValues(-1,-1,+1)); s1b.setTexCoord(vec2.fromValues(0,1)); s1b.setNormal(vec3.fromValues(0,-1,0));
        s2b.setCoord(vec3.fromValues(+1,-1,+1)); s2b.setTexCoord(vec2.fromValues(1,1)); s2b.setNormal(vec3.fromValues(0,-1,0));
        s3b.setCoord(vec3.fromValues(+1,-1,-1)); s3b.setTexCoord(vec2.fromValues(1,0)); s3b.setNormal(vec3.fromValues(0,-1,0));
        s4b.setCoord(vec3.fromValues(-1,-1,-1)); s4b.setTexCoord(vec2.fromValues(0,0)); s4b.setNormal(vec3.fromValues(0,-1,0));

        /// devant

        let s1c = mesh.addVertex("s1c");
        let s2c = mesh.addVertex("s2c");
        let s3c = mesh.addVertex("s3c");
        let s4c = mesh.addVertex("s4c");
        mesh.addQuad(s1c, s2c, s3c, s4c);
        s1c.setCoord(vec3.fromValues(-1,-1,+1)); s1c.setTexCoord(vec2.fromValues(0,0)); s1c.setNormal(vec3.fromValues(0,0,+1));
        s2c.setCoord(vec3.fromValues(+1,-1,+1)); s2c.setTexCoord(vec2.fromValues(1,0)); s2c.setNormal(vec3.fromValues(0,0,+1));
        s3c.setCoord(vec3.fromValues(+1,+1,+1)); s3c.setTexCoord(vec2.fromValues(1,1)); s3c.setNormal(vec3.fromValues(0,0,+1));
        s4c.setCoord(vec3.fromValues(-1,+1,+1)); s4c.setTexCoord(vec2.fromValues(0,1)); s4c.setNormal(vec3.fromValues(0,0,+1));

        /// derrière

        let s1d = mesh.addVertex("s1d");
        let s2d = mesh.addVertex("s2d");
        let s3d = mesh.addVertex("s3d");
        let s4d = mesh.addVertex("s4d");
        mesh.addQuad(s4d, s3d, s2d, s1d);
        s1d.setCoord(vec3.fromValues(-1,-1,-1)); s1d.setTexCoord(vec2.fromValues(1,0)); s1d.setNormal(vec3.fromValues(0,0,-1));
        s2d.setCoord(vec3.fromValues(+1,-1,-1)); s2d.setTexCoord(vec2.fromValues(0,0)); s2d.setNormal(vec3.fromValues(0,0,-1));
        s3d.setCoord(vec3.fromValues(+1,+1,-1)); s3d.setTexCoord(vec2.fromValues(0,1)); s3d.setNormal(vec3.fromValues(0,0,-1));
        s4d.setCoord(vec3.fromValues(-1,+1,-1)); s4d.setTexCoord(vec2.fromValues(1,1)); s4d.setNormal(vec3.fromValues(0,0,-1));

        /// droit

        let s1e = mesh.addVertex("s1e");
        let s2e = mesh.addVertex("s2e");
        let s3e = mesh.addVertex("s3e");
        let s4e = mesh.addVertex("s4e");
        mesh.addQuad(s1e, s2e, s3e, s4e);
        s1e.setCoord(vec3.fromValues(+1,-1,+1)); s1e.setTexCoord(vec2.fromValues(0,0)); s1e.setNormal(vec3.fromValues(+1,0,0));
        s2e.setCoord(vec3.fromValues(+1,-1,-1)); s2e.setTexCoord(vec2.fromValues(1,0)); s2e.setNormal(vec3.fromValues(+1,0,0));
        s3e.setCoord(vec3.fromValues(+1,+1,-1)); s3e.setTexCoord(vec2.fromValues(1,1)); s3e.setNormal(vec3.fromValues(+1,0,0));
        s4e.setCoord(vec3.fromValues(+1,+1,+1)); s4e.setTexCoord(vec2.fromValues(0,1)); s4e.setNormal(vec3.fromValues(+1,0,0));

        /// gauche

        let s1f = mesh.addVertex("s1f");
        let s2f = mesh.addVertex("s2f");
        let s3f = mesh.addVertex("s3f");
        let s4f = mesh.addVertex("s4f");
        mesh.addQuad(s4f, s3f, s2f, s1f);
        s1f.setCoord(vec3.fromValues(-1,-1,+1)); s1f.setTexCoord(vec2.fromValues(1,0)); s1f.setNormal(vec3.fromValues(-1,0,0));
        s2f.setCoord(vec3.fromValues(-1,-1,-1)); s2f.setTexCoord(vec2.fromValues(0,0)); s2f.setNormal(vec3.fromValues(-1,0,0));
        s3f.setCoord(vec3.fromValues(-1,+1,-1)); s3f.setTexCoord(vec2.fromValues(0,1)); s3f.setNormal(vec3.fromValues(-1,0,0));
        s4f.setCoord(vec3.fromValues(-1,+1,+1)); s4f.setTexCoord(vec2.fromValues(1,1)); s4f.setNormal(vec3.fromValues(-1,0,0));

        // calculer les normales
        mesh.computeNormals();

        // créer les VBO pour dessiner ce maillage
        let renderer = new MeshModuleDrawing(mesh);
        this.m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
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
        this.m_VBOset.destroy();
    }
}
