// Définition de la classe Disk

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");


/**
 * remplace chaque triangle du maillage par trois triangles appuyés sur
 * le barycentre de l'ancien triangle
 * @param mesh : maillage concerné par l'opération
 * @param height : amplitude relative du déplacement du barycentre
 */
function subdivideTriangles(mesh, height)
{
    // calculer les normales des triangles et des sommets
    mesh.computeNormals();

    // copie de la liste du maillage
    let triangles = mesh.getTriangleList().slice();

    // traiter tous les triangles
    for (let triangle of triangles) {
        // sommets du triangle courant
        let s0 = triangle.getVertex0();
        let s1 = triangle.getVertex1();
        let s2 = triangle.getVertex2();
        // rajouter un sommet au milieu des trois
        let center = mesh.addVertex("c"+s0.getName()+s1.getName()+s2.getName());
        // ses coordonnées
        let coords = vec3.clone(triangle.getCenter());
        // amplitude du déplacement
        let dist = Math.sqrt(triangle.getSurface()) * height;
        // déplacer le centre en suivant la normale
        let displacement = vec3.create();
        vec3.scale(displacement, triangle.getNormal(), dist);
        vec3.add(coords, coords, displacement);
        center.setCoord(coords);
        // supprimer l'ancien triangle
        triangle.destroy();
        // construire trois triangles
        mesh.addTriangle(center, s0, s1);
        mesh.addTriangle(center, s1, s2);
        mesh.addTriangle(center, s2, s0);
    }
}


class Disk
{
    constructor()
    {
        // créer une sorte de disque convexe

        this.m_Mesh = new Mesh("Disk");

        /// Topologie : éléments et connexions

        let N = 6;

        // rajouter des sommets
        for (let i=0; i<=N; i++) {
            this.m_Mesh.addVertex("s"+i);
        }

        // rajouter des triangles autour du premier sommet
        for (let i=0; i<N; i++) {
            this.m_Mesh.addTriangle(
                this.m_Mesh.getVertex(0),
                this.m_Mesh.getVertex(i+1),
                this.m_Mesh.getVertex((i+1)%N + 1));
        }

        /// Géométrie : position des sommets et autres infos

        // affectation des coordonnées des sommets
        let R = 1.6;
        this.m_Mesh.getVertex(0).setCoord(vec3.fromValues( 0.0,+0.5 * R, 0.0));
        this.m_Mesh.getVertex(0).setColor(vec3.fromValues(0.2, 0.5, 0.3));
        for (let i=0; i<N; i++) {
            let angle = i * 2.0 * Math.PI / N;
            this.m_Mesh.getVertex(i+1).setCoord(vec3.fromValues(R * Math.sin(angle), 0.0, R * Math.cos(angle)));
            this.m_Mesh.getVertex(i+1).setColor(vec3.fromValues(0.2, 0.5, 0.3));
        }

        // appliquer une subdivision
        subdivideTriangles(this.m_Mesh, 0.05);

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
