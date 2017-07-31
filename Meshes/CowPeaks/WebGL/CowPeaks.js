// Définition de la classe CowPeaks

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");


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
    let nbtriangles = triangles.length;
    for (let it=0; it<nbtriangles; it++) {
        // triangle courant et ses sommets
        let triangle = triangles[it];
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


class CowPeaks
{
    constructor(material)
    {
        // créer le maillage de la vache
        this.m_Mesh = new Mesh("CowPeaks");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15, this, this.onCowPeaksLoaded, 0);
    }


    onCowPeaksLoaded(that, param)
    {
        // calculer les normales des triangles et des sommets
        that.m_Mesh.computeNormals();

        // appliquer une subdivision
        subdivideTriangles(that.m_Mesh, 1.0);

        // définir la couleur de tous les sommets
        for (let i=0; i<that.m_Mesh.getVertexCount(); i++) {
            that.m_Mesh.getVertex(i).setColor(vec3.fromValues(0.9, 1.0, 0.2));
        }

        // calculer les normales (elles ne sont pas bonnes car interpolées)
        that.m_Mesh.computeNormals();
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
