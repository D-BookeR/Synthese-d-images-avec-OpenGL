// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModule.js");
Requires("libs/Material/VBOset.js");
Requires("libs/Material/Material.js");


/**
 * Ce module rajoute des fonctions pour créer les VBO pour dessiner un maillage
 */
class MeshModuleDrawing extends MeshModule
{
    /**
     * initialise le module sur le maillage fourni
     * @param mesh maillage concerné
     */
    constructor(mesh)
    {
        super(mesh);
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangles
     * @param material : celui qu'il faut employer pour dessiner les triangles
     * @param interleaved : true s'il faut entrelacer les données
     */
    createVBOset(material, interleaved)
    {
        // quitter si le maillage n'est pas complet (voir loadObjFile)
        if (! this.m_Mesh.isReady()) return null;

        // créer les VBO demandés par le matériau
        let vboset = material.createVBOset();
        vboset.createAttributesVBO(this.m_Mesh, interleaved);

        // rassembler les indices des triangles
        let indexlist = [];
        for (let triangle of this.m_Mesh.getTriangleList()) {
            for (let i=0; i<3; i++) {
                indexlist.push(triangle.getVertex(i).getNumber());
            }
        }

        // créer le VBO des indices
        let size = vboset.createIndexedPrimitiveVBO(gl.TRIANGLES, indexlist);
        console.log(this.m_Mesh.getName()+": "+size+" indices with triangles");

        // l'objet est prêt à être dessiné
        vboset.setReady();

        return vboset;
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangle_strips
     * @param material : celui qu'il faut employer pour dessiner les triangles du ruban
     * @param interleaved : true s'il faut entrelacer les données
     */
    createStripVBOset(material, interleaved)
    {
        // quitter si le maillage n'est pas complet (voir loadObjFile)
        if (! this.m_Mesh.isReady()) return null;

        // créer les VBO demandés par le matériau
        let vboset = material.createVBOset();
        vboset.createAttributesVBO(this.m_Mesh, interleaved);

        // créer une liste de rubans
        let striplist = [];

        // parcourir tous les triangles du maillage
        for (let triangle of this.m_Mesh.getTriangleList()) {
            // tenter d'insérer le triangle dans l'un des rubans
            let ok = false;
            let stripnumber = striplist.length;
            for (let ir=0; ir<stripnumber; ir++) {
                let strip = striplist[ir];
                // le triangle peut-il être mis dans ce ruban ?
                if (strip.appendTriangle(triangle)) {
                    // voir si l'un des rubans suivants peut être concaténé au courant
                    for (let irs=ir+1; irs<stripnumber; irs++) {
                        let nextstrip = striplist[irs];
                        if (strip.concat(nextstrip)) {
                            // supprimer le ruban suivant
                            list_remove(striplist, nextstrip);
                            // ne pas continuer avec les autres rubans
                            break;
                        }
                    }
                    // c'est ok, on arrête le parcours des rubans
                    ok = true;
                    break;
                }
            }
            // si le triangle n'a pas pu être ajouté à un ruban, alors créer un autre ruban et le mettre en tête
            if (! ok) {
                striplist.splice(0,0, new TriangleStrip(triangle));
            }
        }

        // rassembler les indices des sommets des rubans, avec des codes de redémarrage entre rubans
        let indexlist = [];
        let precstrip = null;
        for (let strip of striplist) {
            // rajouter un "primitive restart"
            if (precstrip != null) {
                let prec = precstrip.getVertex(-1).getNumber();
                if ((indexlist.length % 2) == 1) indexlist.push(prec);
                indexlist.push(prec);
                let svt = strip.getVertex(0).getNumber();
                indexlist.push(svt);
            }
            // rajouter les sommets du ruban
            for (let vertex of strip.getVertexList()) {
                indexlist.push(vertex.getNumber());
            }
            precstrip = strip;
        }

        // créer le VBO des indices
        let size = vboset.createIndexedPrimitiveVBO(gl.TRIANGLE_STRIP, indexlist);

        // message d'information
        let triangle_count = this.m_Mesh.getTriangleCount();
        console.debug(this.m_Mesh.getName()+": "+size+" indices with "+striplist.length+" strips, instead of "+(triangle_count*3));

        // l'objet est prêt à être dessiné
        vboset.setReady();

        return vboset;
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner les arêtes du maillage
     * @param material : celui qu'il faut employer pour dessiner les arêtes
     */
    createEdgesVBOset(material)
    {
        // quitter si le maillage n'est pas complet (voir loadObjFile)
        if (! this.m_Mesh.isReady()) return null;

        // créer les VBO demandés par le matériau
        let vboset = material.createVBOset();
        vboset.createAttributesVBO(this.m_Mesh, true);

        // test sur le nombre d'indices
        //if (this.m_Mesh.getEdgeList().length > 32767)
        //    throw "Too many edges to draw";

        // rassembler les indices des arêtes
        let indexlist = [];
        for (let edge of this.m_Mesh.getEdgeList()) {
            // rajouter les sommets de l'arête
            indexlist.push(edge.getVertex1().getNumber());
            indexlist.push(edge.getVertex2().getNumber());
        }

        // créer le VBO des indices
        vboset.createIndexedPrimitiveVBO(gl.LINES, indexlist);

        // l'objet est prêt à être dessiné
        vboset.setReady();

        return vboset;
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner les normales des facettes du maillage
     * @param material : celui qu'il faut employer pour dessiner les normales
     * @param length : nombre donnant la longueur des normales à afficher
     */
    createFacesNormalsVBOset(material, length)
    {
        // quitter si le maillage n'est pas complet (voir loadObjFile)
        if (! this.m_Mesh.isReady()) return null;

        // créer les VBO demandés par le matériau
        let vboset = material.createVBOset();

        // remplir les VBO avec les données demandées
        let data = [];

        // sommet et coordonnées temporaires
        let vertextmp = new MeshVertex(null, "tmp");
        let coords_center = vec3.create();
        let coords_endpoint = vec3.create();

        // traiter chaque triangle
        for (let triangle of this.m_Mesh.getTriangleList()) {

            // sommets du triangle courant
            let v0 = triangle.getVertex0();
            let v1 = triangle.getVertex1();
            let v2 = triangle.getVertex2();

            // normale du sommet temporaire
            vertextmp.setNormal(triangle.getNormal());

            // centre du triangle : ajouter ses données dans le VBO
            vec3.zero(coords_center);
            vec3.add(coords_center, v0.getCoord(), v1.getCoord());
            vec3.add(coords_center, coords_center, v2.getCoord());
            vec3.scale(coords_center, coords_center, 1.0/3.0);
            vertextmp.setCoord(coords_center);

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertextmp, data);

            // extremité = centre + normale * longueur
            vec3.zero(coords_endpoint);
            vec3.scale(coords_endpoint, triangle.getNormal(), length);
            vec3.add(coords_endpoint, coords_center, coords_endpoint);
            vertextmp.setCoord(coords_endpoint);

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertextmp, data);
        }
        vertextmp.destroy();

        // créer le VBO entrelacé
        vboset.createInterleavedDataAttributesVBO(data);

        // pas de VBO d'indices car seulement des lignes à dessiner
        let triangle_count = this.m_Mesh.getTriangleCount();
        vboset.createDirectPrimitiveVBO(gl.LINES, triangle_count * 2);

        // l'objet est prêt à être dessiné
        vboset.setReady();

        return vboset;
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner les normales des sommets du maillage
     * @param material : celui qu'il faut employer pour dessiner les normales
     * @param length : nombre donnant la longueur des normales à afficher
     */
    createVertexNormalsVBOset(material, length)
    {
        // quitter si le maillage n'est pas complet (voir loadObjFile)
        if (! this.m_Mesh.isReady()) return null;

        // créer les VBO demandés par le matériau
        let vboset = material.createVBOset();

        // remplir les VBO avec les données demandées
        let data = [];

        // sommet et coordonnées temporaires
        let vertextmp = new MeshVertex(null, "tmp");
        let coords_endpoint = vec3.create();

        // traiter chaque vertex
        for (let vertex of this.m_Mesh.m_VertexList) {

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertex, data);

            // extremité = sommet + normale * longueur
            vec3.zero(coords_endpoint);
            vec3.scale(coords_endpoint, vertex.getNormal(), length);
            vec3.add(coords_endpoint, vertex.getCoord(), coords_endpoint);
            vertextmp.setCoord(coords_endpoint);

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertextmp, data);
        }
        vertextmp.destroy();

        // créer le VBO entrelacé
        vboset.createInterleavedDataAttributesVBO(data);

        // pas de VBO d'indices car seulement des lignes à dessiner
        let vertex_count = this.m_Mesh.getVertexCount();
        vboset.createDirectPrimitiveVBO(gl.LINES, vertex_count * 2);

        // l'objet est prêt à être dessiné
        vboset.setReady();

        return vboset;
    }
}

    /**
     * Cette classe représente un triangle strip
     */
class TriangleStrip
{
    /**
     * Cette classe représente un triangle strip
     * NB: l'ordre des sommets du triangle est important pour commencer un ruban !
     * @param triangle : le premier triangle d'un ruban
     */
    constructor(triangle)
    {
        // allouer la liste des sommets du ruban
        this.m_VertexList = [];

        // copie de la liste des sommets du triangle
        this.m_VertexList.push(triangle.getVertex0());
        this.m_VertexList.push(triangle.getVertex1());
        this.m_VertexList.push(triangle.getVertex2());
    }


    /**
     * retourne la longueur du ruban, son nombre de sommets
     * @return nombre de sommets du ruban
     */
    length()
    {
        return this.m_VertexList.length;
    }


    /**
     * retourne la liste des sommets du ruban
     * @return liste des sommets
     */
    getVertexList()
    {
        return this.m_VertexList;
    }


    /**
     * retourne l'un des sommets du ruban, désigné par son numéro
     * @param i = le numéro du sommet, si <0 alors par rapport à la fin du tableau
     * @return le ie sommet du ruban
     */
    getVertex(i)
    {
        if (i < 0) {
            return this.m_VertexList[this.m_VertexList.length+i];
        } else {
            return this.m_VertexList[i];
        }
    }


    /**
     * Cette méthode tente d'ajouter le triangle au ruban
     * @param triangle à rajouter
     * @return true si le triangle a pu être mis dans le ruban, false sinon
     */
    appendTriangle(triangle)
    {
        // nombre de sommets du ruban
        let vertex_number = this.m_VertexList.length;

        // cas particulier : le ruban ne compte qu'un seul triangle
        if (vertex_number == 3) {
            // sommets du ruban
            let v0 = this.m_VertexList[0];
            let v1 = this.m_VertexList[1];
            let v2 = this.m_VertexList[2];
            // NB: les tests sont dans l'ordre le plus favorable si les triangles ont été créés par addNappeRectangulaire
            // le triangle contient-il l'arête C-B ?
            if (triangle.containsEdge(v2, v1)) {
                // le triangle se colle à l'arête B-C du ruban
                this.m_VertexList = [ v0, v1, v2, triangle.getVertex(2) ];
                return true;
            }
            // le triangle contient-il l'arête B-A ?
            if (triangle.containsEdge(v1, v0)) {
                // le triangle se colle à l'arête A-B du ruban
                this.m_VertexList = [ v2, v0, v1, triangle.getVertex(2) ];
                return true;
            }
            // le triangle contient-il l'arête A-C ?
            if (triangle.containsEdge(v0, v2)) {
                // le triangle se colle à l'arête C-A du ruban
                this.m_VertexList = [ v1, v2, v0, triangle.getVertex(2) ];
                return true;
            }
            // aucun des trois côtés ne convient
            return false;
        }

        // si le nombre de sommets du ruban est pair, on peut le retourner si besoin
        if ((vertex_number % 2) == 0) {
            // test d'insertion en fin de ruban
            if (triangle.containsEdge(this.m_VertexList[vertex_number-2], this.m_VertexList[vertex_number-1])) {
                this.m_VertexList.push(triangle.getVertex(2));
                return true;
            }
            // test d'insertion en debut de ruban
            if (triangle.containsEdge(this.m_VertexList[1], this.m_VertexList[0])) {
                // retourner le ruban
                this.m_VertexList.reverse();
                this.m_VertexList.push(triangle.getVertex(2));
                return true;
            }
        } else {
            // le nombre de sommets du ruban est impair, on ne peut insérer qu'en fin
            if (triangle.containsEdge(this.m_VertexList[vertex_number-1], this.m_VertexList[vertex_number-2])) {
                this.m_VertexList.push(triangle.getVertex(2));
                return true;
            }
        }

        // aucune solution n'est satisfaisante, le triangle ne peut pas être ajouté
        return false;
    }


    /**
     * Cette méthode tente d'ajouter un ruban de 3 sommets au ruban
     * @param other : autre ruban à rajouter
     * @return true si l'autre ruban a pu être mis dans le ruban, false sinon
     */
    appendStrip3(other)
    {
        // nombre de sommets du ruban
        let vertex_number = this.m_VertexList.length;

        // cas particulier : le ruban ne compte qu'un seul triangle
        if (vertex_number == 3) {
            // sommets du ruban
            let v0 = this.m_VertexList[0];
            let v1 = this.m_VertexList[1];
            let v2 = this.m_VertexList[2];
            // NB: les tests sont dans l'ordre le plus favorable si les triangles ont été créés par addNappeRectangulaire
            // l'autre ruban contient-il l'arête C-B ?
            if (other.containsEdge(v2, v1)) {
                // l'autre ruban se colle à l'arête B-C du ruban
                this.m_VertexList = [ v0, v1, v2, other.m_VertexList[2] ];
                return true;
            }
            // l'autre ruban contient-il l'arête B-A ?
            if (other.containsEdge(v1, v0)) {
                // l'autre ruban se colle à l'arête A-B du ruban
                this.m_VertexList = [ v2, v0, v1, other.m_VertexList[2] ];
                return true;
            }
            // l'autre ruban contient-il l'arête A-C ?
            if (other.containsEdge(v0, v2)) {
                // l'autre ruban se colle à l'arête C-A du ruban
                this.m_VertexList = [ v1, v2, v0, other.m_VertexList[2] ];
                return true;
            }
            // aucun des trois côtés ne convient
            return false;
        }

        // si le nombre de sommets du ruban est pair, on peut le retourner si besoin
        if ((vertex_number % 2) == 0) {
            // test d'insertion en fin de ruban
            if (other.containsEdge(this.m_VertexList[vertex_number-2], this.m_VertexList[vertex_number-1])) {
                this.m_VertexList.push(other.m_VertexList[2]);
                return true;
            }
            // test d'insertion en debut de ruban
            if (other.containsEdge(this.m_VertexList[1], this.m_VertexList[0])) {
                // retourner le ruban
                this.m_VertexList.reverse();
                this.m_VertexList.push(other.m_VertexList[2]);
                return true;
            }
        } else {
            // le nombre de sommets du ruban est impair, on ne peut insérer qu'en fin
            if (other.containsEdge(this.m_VertexList[vertex_number-1], this.m_VertexList[vertex_number-2])) {
                this.m_VertexList.push(other.m_VertexList[2]);
                return true;
            }
        }

        // aucune solution n'est satisfaisante, le triangle ne peut pas être ajouté
        return false;
    }


    /**
     * retourne true si le ruban contient 3 sommets, dont v1 suivi de v2
     * Si true, alors au retour, v1 et v2 sont dans les deux premières places du ruban
     * @param vertex0 : premier sommet de l'arête (elle est orientée de 1 à 2)
     * @param vertex1 : second sommet
     * @return true si oui, false si l'arête n'est pas dans le triangle
     */
    containsEdge(vertex0, vertex1)
    {
        if (this.m_VertexList.length != 3) return false;
        let v0 = this.m_VertexList[0];
        let v1 = this.m_VertexList[1];
        let v2 = this.m_VertexList[2];

        if (v0 == vertex0 && v1 == vertex1) return true;

        if (v1 == vertex0 && v2 == vertex1) {
            // mettre v2 en premier
            this.m_VertexList = [ v1, v2, v0 ];
            return true;
        }

        if (v2 == vertex0 && v0 == vertex1) {
            // mettre v2 en premier
            this.m_VertexList = [ v2, v0, v1 ];
            return true;
        }

        return false;
    }


    /**
     * Cette méthode tente d'ajouter le ruban fourni au ruban this
     * @param strip : ruban à rajouter
     * @return true si le ruban a pu être concaténé à this, false sinon
     */
    concat(strip)
    {
        // nombre de sommets des rubans
        let thisvertexnumber  = this.m_VertexList.length;
        let stripvertexnumber = strip.m_VertexList.length;

        // cas particulier d'un triangle, appeler l'autre méthode
        if (stripvertexnumber == 3) this.appendStrip3(strip);

        // si this a un nombre de sommets pair, alors tenter
        if ((thisvertexnumber % 2) == 0) {
            // il faut que la dernière arête de this = la première de ruban
            if (this.m_VertexList[thisvertexnumber-2] == strip.m_VertexList[0] &&
                this.m_VertexList[thisvertexnumber-1] == strip.m_VertexList[1]) {
                this.m_VertexList = this.m_VertexList.concat(strip.m_VertexList.slice(2));
                return true;
            }
            // si l'autre ruban a un nombre pair de sommets, on peut le retourner
            if ((stripvertexnumber % 2) == 0) {
                // tenter de mettre l'autre ruban à l'envers en fin de this
                if (this.m_VertexList[thisvertexnumber-2] == strip.m_VertexList[stripvertexnumber-1] &&
                    this.m_VertexList[thisvertexnumber-1] == strip.m_VertexList[stripvertexnumber-2]) {
                    strip.m_VertexList.reverse();
                    this.m_VertexList = this.m_VertexList.concat(strip.m_VertexList.slice(2));
                    return true;
                }
                // tenter de metre l'autre ruban au début de this
                if (this.m_VertexList[0] == strip.m_VertexList[stripvertexnumber-2] &&
                    this.m_VertexList[1] == strip.m_VertexList[stripvertexnumber-1]) {
                    this.m_VertexList = strip.m_VertexList.concat(this.m_VertexList.slice(2));
                    return true;
                }
            }
        }

        // si ruban a un nombre de sommets pair
        if ((stripvertexnumber % 2) == 0) {
            // tenter de mettre this à la fin de ruban
            if (this.m_VertexList[0] == strip.m_VertexList[stripvertexnumber-2] &&
                this.m_VertexList[1] == strip.m_VertexList[stripvertexnumber-1]) {
                this.m_VertexList = strip.m_VertexList.concat(this.m_VertexList.slice(2));
                return true;
            }
            // tenter de mettre this en début de ruban (en inversant ruban)
            if (this.m_VertexList[0] == strip.m_VertexList[1] &&
                this.m_VertexList[1] == strip.m_VertexList[0]) {
                strip.m_VertexList.reverse();
                this.m_VertexList = strip.m_VertexList.concat(this.m_VertexList.slice(2));
                return true;
            }
        }

        // aucune solution n'est satisfaisante, le ruban ne peut pas être ajouté
        return false;
    }
}
