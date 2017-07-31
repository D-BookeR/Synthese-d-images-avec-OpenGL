// Définition de la classe CubeOK

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");


class CubeOK
{
    constructor()
    {
        this.m_Mesh = new Mesh("CubeOK");
        let color = vec3.fromValues(0.2, 0.5, 0.3);

        /// dessus

        let s1a = this.m_Mesh.addVertex("s1a");
        let s2a = this.m_Mesh.addVertex("s2a");
        let s3a = this.m_Mesh.addVertex("s3a");
        let s4a = this.m_Mesh.addVertex("s4a");
        this.m_Mesh.addQuad(s1a, s2a, s3a, s4a);
        s1a.setCoord(vec3.fromValues(-1,+1,+1)); s1a.setColor(color);
        s2a.setCoord(vec3.fromValues(+1,+1,+1)); s2a.setColor(color);
        s3a.setCoord(vec3.fromValues(+1,+1,-1)); s3a.setColor(color);
        s4a.setCoord(vec3.fromValues(-1,+1,-1)); s4a.setColor(color);

        /// dessous

        let s1b = this.m_Mesh.addVertex("s1b");
        let s2b = this.m_Mesh.addVertex("s2b");
        let s3b = this.m_Mesh.addVertex("s3b");
        let s4b = this.m_Mesh.addVertex("s4b");
        this.m_Mesh.addQuad(s4b, s3b, s2b, s1b);
        s1b.setCoord(vec3.fromValues(-1,-1,+1)); s1b.setColor(color);
        s2b.setCoord(vec3.fromValues(+1,-1,+1)); s2b.setColor(color);
        s3b.setCoord(vec3.fromValues(+1,-1,-1)); s3b.setColor(color);
        s4b.setCoord(vec3.fromValues(-1,-1,-1)); s4b.setColor(color);

        /// devant

        let s1c = this.m_Mesh.addVertex("s1c");
        let s2c = this.m_Mesh.addVertex("s2c");
        let s3c = this.m_Mesh.addVertex("s3c");
        let s4c = this.m_Mesh.addVertex("s4c");
        this.m_Mesh.addQuad(s1c, s2c, s3c, s4c);
        s1c.setCoord(vec3.fromValues(-1,-1,+1)); s1c.setColor(color);
        s2c.setCoord(vec3.fromValues(+1,-1,+1)); s2c.setColor(color);
        s3c.setCoord(vec3.fromValues(+1,+1,+1)); s3c.setColor(color);
        s4c.setCoord(vec3.fromValues(-1,+1,+1)); s4c.setColor(color);

        /// derrière

        let s1d = this.m_Mesh.addVertex("s1d");
        let s2d = this.m_Mesh.addVertex("s2d");
        let s3d = this.m_Mesh.addVertex("s3d");
        let s4d = this.m_Mesh.addVertex("s4d");
        this.m_Mesh.addQuad(s4d, s3d, s2d, s1d);
        s1d.setCoord(vec3.fromValues(-1,-1,-1)); s1d.setColor(color);
        s2d.setCoord(vec3.fromValues(+1,-1,-1)); s2d.setColor(color);
        s3d.setCoord(vec3.fromValues(+1,+1,-1)); s3d.setColor(color);
        s4d.setCoord(vec3.fromValues(-1,+1,-1)); s4d.setColor(color);

        /// droit

        let s1e = this.m_Mesh.addVertex("s1e");
        let s2e = this.m_Mesh.addVertex("s2e");
        let s3e = this.m_Mesh.addVertex("s3e");
        let s4e = this.m_Mesh.addVertex("s4e");
        this.m_Mesh.addQuad(s1e, s2e, s3e, s4e);
        s1e.setCoord(vec3.fromValues(+1,-1,+1)); s1e.setColor(color);
        s2e.setCoord(vec3.fromValues(+1,-1,-1)); s2e.setColor(color);
        s3e.setCoord(vec3.fromValues(+1,+1,-1)); s3e.setColor(color);
        s4e.setCoord(vec3.fromValues(+1,+1,+1)); s4e.setColor(color);

        /// gauche

        let s1f = this.m_Mesh.addVertex("s1f");
        let s2f = this.m_Mesh.addVertex("s2f");
        let s3f = this.m_Mesh.addVertex("s3f");
        let s4f = this.m_Mesh.addVertex("s4f");
        this.m_Mesh.addQuad(s4f, s3f, s2f, s1f);
        s1f.setCoord(vec3.fromValues(-1,-1,+1)); s1f.setColor(color);
        s2f.setCoord(vec3.fromValues(-1,-1,-1)); s2f.setColor(color);
        s3f.setCoord(vec3.fromValues(-1,+1,-1)); s3f.setColor(color);
        s4f.setCoord(vec3.fromValues(-1,+1,+1)); s4f.setColor(color);

        // calculer les normales
        this.m_Mesh.computeNormals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    getMesh()
    {
        return this.m_Mesh;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
