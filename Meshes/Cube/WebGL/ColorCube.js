// Définition de la classe ColorCube

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleProcessing.js");
Requires("libs/Mesh/MeshModulePhysics.js");


class ColorCube
{
    constructor()
    {
        // créer le maillage
        this.m_Mesh = new Mesh("ColorCube");


        /// Topologie : éléments et connexions

        // rajouter des sommets
        let s1 = this.m_Mesh.addVertex("s1");
        let s2 = this.m_Mesh.addVertex("s2");
        let s3 = this.m_Mesh.addVertex("s3");
        let s4 = this.m_Mesh.addVertex("s4");
        let s5 = this.m_Mesh.addVertex("s5");
        let s6 = this.m_Mesh.addVertex("s6");
        let s7 = this.m_Mesh.addVertex("s7");
        let s8 = this.m_Mesh.addVertex("s8");

        // rajouter des triangles
        this.m_Mesh.addQuad(s1, s4, s3, s2);
        this.m_Mesh.addQuad(s1, s2, s6, s5);
        this.m_Mesh.addQuad(s2, s3, s7, s6);
        this.m_Mesh.addQuad(s3, s4, s8, s7);
        this.m_Mesh.addQuad(s4, s1, s5, s8);
        this.m_Mesh.addQuad(s5, s6, s7, s8);


        /// Géométrie : position des sommets et autres info

        // affectation des couleurs et des coordonnées des sommets
        s1.setCoord(vec3.fromValues(-1,-1,+1)); s1.setColor(vec3.fromValues(0.9,0.0,0.0));   // rouge
        s2.setCoord(vec3.fromValues(+1,-1,+1)); s2.setColor(vec3.fromValues(0.0,0.8,0.0));   // vert
        s3.setCoord(vec3.fromValues(+1,-1,-1)); s3.setColor(vec3.fromValues(0.0,0.2,1.0));   // bleu
        s4.setCoord(vec3.fromValues(-1,-1,-1)); s4.setColor(vec3.fromValues(0.5,0.0,0.5));   // violet
        s5.setCoord(vec3.fromValues(-1,+1,+1)); s5.setColor(vec3.fromValues(0.2,1.0,1.0));   // bleu clair
        s6.setCoord(vec3.fromValues(+1,+1,+1)); s6.setColor(vec3.fromValues(1.0,1.0,0.2));   // jaune
        s7.setCoord(vec3.fromValues(+1,+1,-1)); s7.setColor(vec3.fromValues(1.0,0.2,0.5));   // rose
        s8.setCoord(vec3.fromValues(-1,+1,-1)); s8.setColor(vec3.fromValues(1.0,0.6,0.0));   // orange

        // mettre à jour la géométrie : normales...
        this.m_Mesh.computeNormals();


        /// Transformations

        // module de transformation et de calculs physique
        let transformation = new MeshModuleProcessing(this.m_Mesh);
        let physique = new MeshModulePhysics(this.m_Mesh);

        // vérifier que tout est ok
        transformation.checkMesh(false);

        // calculer le volume et le centre de gravité
        this.m_Mesh.computeNormals();
        physique.setDensity(1.0);
        physique.compVolumeIntegrals();
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
