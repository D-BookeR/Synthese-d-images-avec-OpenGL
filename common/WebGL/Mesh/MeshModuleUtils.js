// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModule.js");


/**
 * Ce module rajoute des fonctions diverses pour les maillages
 *
 */
class MeshModuleUtils extends MeshModule
{
    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
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
     * Cette méthode produit le nom du milieu entre p1 et p2, c'est la concaténation
     * du nom de p1 et de p2 mais dans l'ordre lexicographique afin que
     * nomMilieu(p1, p2) == nomMilieu(p2, p1) quels que soient p1 et p2
     * @param p1 : vertex
     * @param p2 : vertex
     * @return "m" + nom du plus petit sommet + nom du plus grand
     */
    static getMidName(p1, p2)
    {
        // définir le nom du milieu à partir des noms de p1 et p2
        if (p1.getName() < p2.getName()) {
            // p1 a le plus petit nom dans l'ordre lexicographique
            return "m"+p1.getName()+p2.getName();
        } else {
            // p2 a le plus petit nom dans l'ordre lexicographique
            return "m"+p2.getName()+p1.getName();
        }
    }


    /**
     * calcule la moyenne des normales des triangles fournis
     * @param triangles : liste des triangles à considérer
     * @return moyenne des normales
     */
    static averageNormals(triangles)
    {
        // accumuler les normales puis normaliser
        let sum = vec3.create();
        for (let triangle of triangles) {
            // normale du triangle courant
            let n = vec3.clone(triangle.getNormal());
            // prendre en compte la surface du triangle
            vec3.scale(n, n, triangle.getSurface());
            // cumuler les normales pondérées
            vec3.add(sum, sum, n);
        }
        // normaliser le résultat
        vec3.normalize(sum, sum);
        return sum;
    }


    /**
     * calcule la moyenne des tangentes des triangles fournis
     * @param triangles : liste des triangles à considérer
     * @return moyenne des tangentes
     */
    static averageTangents(triangles)
    {
        // accumuler les tangentes puis normaliser
        let sum = vec3.create();
        for (let triangle of triangles) {
            // tangente du triangle courant
            let t = vec3.clone(triangle.getTangent());
            // prendre en compte la surface du triangle
            vec3.scale(t, t, triangle.getSurface());
            // cumuler les tangentes pondérées
            vec3.add(sum, sum, t);
        }
        // normaliser le résultat
        vec3.normalize(sum, sum);
        return sum;
    }


    /**
     * Cette fonction retourne la liste des sommets qui sont contenus dans les triangles fournis.
     * @param triangles : liste de triangles
     * @return tableau des sommets
     */
    static getVerticesFromTriangles(triangles)
    {
        let result = [];
        for (let triangle of triangles) {
            for (let vertex of triangle.getVertexList()) {
                if (result.indexOf(vertex) < 0) {
                    result.push(vertex);
                }
            }
        }
        return result;
    }


    /**
     * Détermine les (deux) triangles qui sont à gauche de la demi-arête mais sans sortir
     * de la zone délimitée par le contour. Rajoute aussi tous les triangles qui sont en contact
     * avec les triangles déterminés au premier coup (appels récursifs).
     * En fait, c'est un algo de remplissage de tache appliqué à un maillage de triangles.
     * @param trianglelist : liste à laquelle sont rajoutés les triangles
     * @param halfedge : demi-arête de départ
     * @param border : liste de demi-arêtes qui délimitent les triangles intéressants
     */
    appendNeighborTriangles(trianglelist, halfedge, border)
    {
        // triangle à gauche de cette demi-arête
        let triangle = halfedge.getTriangle();
        if (triangle == null) return;

        // ajouter ce triangle à la liste sauf s'il y est déjà
        if (trianglelist.indexOf(triangle) < 0) {
            trianglelist.push(triangle);

            // demi-arête suivante et son opposée
            halfedge = halfedge.getNext();
            let opposite = halfedge.getOpposite();

            // si elle n'est pas dans le contour, on va voir à sa droite
            if (border.indexOf(halfedge) < 0 && opposite != null) {
                this.appendNeighborTriangles(trianglelist, opposite, border);
            }

            // demi-arête suivante et son opposée
            halfedge = halfedge.getNext();
            opposite = halfedge.getOpposite();

            // si elle n'est pas dans le contour, on va voir à sa droite
            if (border.indexOf(halfedge) < 0 && opposite != null) {
                this.appendNeighborTriangles(trianglelist, opposite, border);
            }
        }
    }


    /**
     * retourne la liste des triangles qui sont délimités par une succession de demi-arêtes
     * Le contour doit être refermé sur lui-même ou constituer une frontière d'un bord
     * à l'autre du maillage, sinon tous les triangles seront sélectionnés.
     * Attention à l'ordre de spécification des sommets du contour : ce sont les triangles
     * à gauche qui sont spécifiés (tourner dans le sens trigo pour entourer une zone).
     * @param border : liste des demi-arêtes. Les triangles sont à gauche d'elles
     * @return liste des triangles compris dans le contour
     */
    getTrianglesInsideBorder(border)
    {
        // résultat, initialisé à vide
        let trianglelist = [];

        // si la liste est trop courte, sortir sans rien faire
        if (border.length < 3) return triangles;

        // partir de la première arête
        let halfedge = border[0];

        // appels récursifs pour rajouter les triangles qui sont en contact avec cette arête
        this.appendNeighborTriangles(trianglelist, halfedge, border);

        return trianglelist;
    }


    /**
     * retourne la liste des demi-arêtes correspondant à une liste de vertex formant un contour fermé
     * @param border : liste de sommets classés dans l'ordre, le dernier doit être lié au premier
     * @return liste des demi-arêtes
     * @throws Exception
     */
    getHalfEdgesAlongBorder(border)
    {
        let halfedgeslist = [];

        let prec = null;
        for (let v of border) {
            if (prec != null) {
                let halfedge = prec.getHalfEdgeTo(v);
                if (halfedge == null) throw "Vertex "+prec.toString()+" has no half-edge to "+v.toString();
                halfedgeslist.push(halfedge);
            }
            prec = v;
        }
        // dernier sommet à lier avec le premier
        if (prec != null) {
            let v = border[0];
            let halfedge = prec.getHalfEdgeTo(v);
            if (halfedge == null) throw "Vertex "+prec.toString()+" has no half-edge to "+v.toString();
            halfedgeslist.push(halfedge);
        }
        return halfedgeslist;
    }


    /**
     * Cette méthode calcule la valeur minimum des coordonnées
     * @return vec3 contenant min(x),min(y),min(z) des coordonnées des sommets
     */
    computeMinAABB()
    {
        // déterminer les bornes min du maillage
        let vmin = vec3.clone(this.m_Mesh.getVertex(0).getCoord());
        for (let vertex of this.m_Mesh.getVertexList()) {
            vec3.min(vmin, vmin, vertex.getCoord());
        }
        return vmin;
    }


    /**
     * Cette méthode calcule la valeur maximale des coordonnées
     * @return vec3 contenant max(x),max(y),max(z) des coordonnées des sommets
     */
    computeMaxAABB()
    {
        // déterminer les bornes max du maillage
        let vmax = vec3.clone(this.m_Mesh.getVertex(0).getCoord());
        for (let vertex of this.m_Mesh.getVertexList()) {
            vec3.max(vmax, vmax, vertex.getCoord());
        }
        return vmax;
    }


    /**
     * Cette méthode construit la boîte englobante du maillage this
     * @return maillage en forme de parallépipède rectangle
     */
    createAABB()
    {
        // déterminer les bornes du maillage
        let vmin = vec3.clone(this.m_Mesh.getVertex(0).getCoord());
        let vmax = vec3.clone(this.m_Mesh.getVertex(0).getCoord());
        for (let vertex of this.m_Mesh.getVertexList()) {
            vec3.min(vmin, vmin, vertex.getCoord());
            vec3.max(vmax, vmax, vertex.getCoord());
        }

        // maillage en retour
        let aabb = new Mesh("AABB");

        // sommets de ce maillage
        let xyz = aabb.addVertex("xyz");
        let Xyz = aabb.addVertex("Xyz");
        let xYz = aabb.addVertex("xYz");
        let XYz = aabb.addVertex("XYz");
        let xyZ = aabb.addVertex("xyZ");
        let XyZ = aabb.addVertex("XyZ");
        let xYZ = aabb.addVertex("xYZ");
        let XYZ = aabb.addVertex("XYZ");

        // coordonnées des coins
        xyz.setCoord(vec3.fromValues(vmin[0], vmin[1], vmin[2]));
        Xyz.setCoord(vec3.fromValues(vmax[0], vmin[1], vmin[2]));
        xYz.setCoord(vec3.fromValues(vmin[0], vmax[1], vmin[2]));
        XYz.setCoord(vec3.fromValues(vmax[0], vmax[1], vmin[2]));
        xyZ.setCoord(vec3.fromValues(vmin[0], vmin[1], vmax[2]));
        XyZ.setCoord(vec3.fromValues(vmax[0], vmin[1], vmax[2]));
        xYZ.setCoord(vec3.fromValues(vmin[0], vmax[1], vmax[2]));
        XYZ.setCoord(vec3.fromValues(vmax[0], vmax[1], vmax[2]));

        // construire les quads de la boîte
        aabb.addQuad(XyZ,Xyz,XYz,XYZ);
        aabb.addQuad(Xyz,xyz,xYz,XYz);
        aabb.addQuad(xyz,xyZ,xYZ,xYz);
        aabb.addQuad(xyZ,XyZ,XYZ,xYZ);
        aabb.addQuad(XYZ,XYz,xYz,xYZ);
        aabb.addQuad(Xyz,XyZ,xyZ,xyz);

        // finaliser cette boîte
        aabb.computeNormals();

        return aabb;
    }


    /**
     * Cette méthode vérifie la cohérence des informations du maillage.
     * La référence est la liste des sommets et la liste des triangles
     * NB : cette fonction ne teste pas toutes les erreurs possibles, mais
     * seulement quelques unes des plus graves.
     * @param repair : true s'il faut tenter de réparer les erreurs, ex: vertex non utilisé
     * @return true si le maillage semble correct, false s'il y a une grave erreur
     */
    checkMesh(repair)
    {
        // vérifier les triangles
        let triangle_count = this.m_Mesh.getTriangleCount();
        for (let it=0; it<triangle_count; it++) {
            let triangle = this.m_Mesh.getTriangleList()[it];
            // le triangle doit être du maillage
            if (triangle.getMesh() != this.m_Mesh) {
                console.error("Triangle #"+it+" does not belong to the mesh");
                return false;
            }
            // la demi-arête doit être dans le maillage
            if (this.m_Mesh.getHalfEdgeList().indexOf(triangle.getHalfEdge()) < 0) {
                console.error("Half-edge of Triangle #"+it+" does not belong to the mesh");
                return false;
            }
            // sa demi-arête doit le désigner
            if (triangle.getHalfEdge().getTriangle() != triangle) {
                console.error("Half-edge of Triangle #"+it+" does not point to it");
                return false;
            }
            // vérifier que les points sont tous différents
            if (triangle.getVertex0() == triangle.getVertex1() ||
                triangle.getVertex0() == triangle.getVertex2() ||
                triangle.getVertex1() == triangle.getVertex2()) {
                console.error("Vertices of triangle #"+it+" are not all distincts");
                return false;
            }
            // vérifier les points
            for (let i=0; i<3; i++) {
                let vertex = triangle.getVertex(i);
                // ce sommet doit être dans la liste des sommets du maillage
                if (this.m_Mesh.getVertexList().indexOf(vertex) < 0) {
                    console.error("Vertex #"+i+"="+vertex.getName()+" of triangle #"+it+" does not belong to the mesh");
                    return false;
                }
            }
        }

        // liste des sommets à supprimer
        let suppr = [];

        // vérifier les sommets
        let vertex_count = this.m_Mesh.getVertexCount();
        for (let iv=0; iv<vertex_count; iv++) {
            let vertex = this.m_Mesh.getVertexList()[iv];
            // le sommet doit être du maillage
            if (vertex.getMesh() != this.m_Mesh) {
                console.error("Vertex #"+iv+"="+vertex.getName()+" does not belong to the mesh");
                return false;
            }
            // la demi-arête doit être null ou dans le maillage
            if (vertex.getHalfEdge() != null && this.m_Mesh.getHalfEdgeList().indexOf(vertex.getHalfEdge()) < 0) {
                console.error("Half-edge of vertex #"+iv+"="+vertex.getName()+" does not belong to the mesh");
                return false;
            }
            // sa demi-arête doit le désigner
            if (vertex.getHalfEdge() != null && vertex.getHalfEdge().getOrigin() != vertex) {
                console.error("Half-edge of vertex #"+iv+"="+vertex.getName()+" does not point to it");
                return false;
            }
            // ce sommet doit être dans l'un des triangles du maillage
            let present = false;
            for (let it=0; it<triangle_count && !present; it++) {
                let triangle = this.m_Mesh.getTriangleList()[it];
                for (let i=0; i<3; i++) {
                    if (triangle.getVertex(i) == vertex) {
                        present = true;
                        break;
                    }
                }
            }
            if (! present) {
                if (repair) {
                    suppr.add(vertex);
                } else {
                    console.warn("Vertex "+vertex.toString()+" does not belong to any triangle of mesh");
                    //return false;
                }
            }
            // vérifier le chaînage m_Sibling
            let halfedge = vertex.getHalfEdge();
            while (halfedge != null) {
                // l'origine de cette demi-arête doit être le sommet
                if (halfedge.getOrigin() != vertex) {
                    console.error("Half-edge "+halfedge+" is chained to vertex "+vertex+" but does not point to it");
                    return false;
                }
                // il ne faut pas qu'il y ait un cycle dans le chaînage
                let other = vertex.getHalfEdge();
                while (other != null && other != halfedge) {
                    if (halfedge.getSibling() == other) {
                        console.error("Half-edge "+halfedge+" belongs to a cycle in siblings of vertex "+vertex);
                    }
                    other = other.getSibling();
                }
                halfedge = halfedge.getSibling();
            }
        }

        // supprimer les sommets qui ne sont dans aucun triangle (si l'option repair a été activée)
        for (let vertex of suppr) {
            vertex.destroy();
        }

        // vérifier les demi-arêtes
        let halfedge_count = this.m_Mesh.getHalfEdgeList().length;
        for (let id=0; id<halfedge_count; id++) {
            let halfedge = this.m_Mesh.getHalfEdgeList()[id];
            // la demi-arête doit être du maillage
            if (halfedge.getMesh() != this.m_Mesh) {
                console.error("Half-Edge #"+id+" does not belong to the mesh");
                return false;
            }
            // son origine doit être un des sommets du maillage
            if (this.m_Mesh.getVertexList().indexOf(halfedge.getOrigin()) < 0) {
                console.error("Origin of Opposite Half-edge of #"+id+" does not belong to the mesh");
                return false;
            }
            // vérifier l'opposée de la demi-arête
            if (halfedge.getOpposite() != null) {
                // son opposée doit être dans le maillage
                if (this.m_Mesh.getHalfEdgeList().indexOf(halfedge.getOpposite()) < 0) {
                    console.error("Opposite Half-edge of #"+id+" does not belong to the mesh");
                    return false;
                }
                // l'opposée de son opposée doit être elle-même
                if (halfedge.getOpposite().getOpposite() != halfedge) {
                    console.error("Opposite of opposite Half-edge of #"+id+" is not itself");
                    return false;
                }
            }
            // vérifier le chaînage m_Next
            if (halfedge.getNext() == null) {
                console.error("Next half-edge of #"+id+" is null");
                return false;
            }
            if (this.m_Mesh.getHalfEdgeList().indexOf(halfedge.getNext()) < 0) {
                console.error("Next half-edge of #"+id+" does not belong to the mesh");
                return false;
            }
            // vérifier le chaînage m_Next.m_Next
            if (halfedge.getNext().getNext() == null) {
                console.error("Second next of half-edge #"+id+" is null");
                return false;
            }
            if (this.m_Mesh.getHalfEdgeList().indexOf(halfedge.getNext().getNext()) < 0) {
                console.error("Second next of half-edge #"+id+" does not belong to the mesh");
                return false;
            }
            // vérifier le chaînage m_Next.m_Next.m_Next
            if (halfedge.getNext().getNext().getNext() == null) {
                console.error("Third next of half-edge #"+id+" is null");
                return false;
            }
            if (halfedge.getNext().getNext().getNext() != halfedge) {
                console.error("Third next of half-edge #"+id+" n'est pas elle-même");
                return false;
            }
        }

        // vérifier les arêtes
        let edge_count = this.m_Mesh.getEdgeList().length;
        for (let ia=0; ia<edge_count; ia++) {
            let edge = this.m_Mesh.getEdgeList()[ia];
            // l'arête doit être du maillage
            if (edge.getMesh() != this.m_Mesh) {
                console.error("Edge #"+ia+" does not belong to the mesh");
                return false;
            }
        }

        // bilan positif
        console.log("Mesh checked OK");

        // succès, mais attention, ça ne valide que ce qui a été testé
        return true;
    }


    /**
     * Cette méthode affiche la totalité des structures de données du maillage
     */
    dump()
    {
        // sommets
        for (let vertex of this.m_Mesh.getVertexList())
            console.log(vertex.toFullString());
        // arêtes
        //for (let edge of this.m_Mesh.getEdgeList())
        //    console.log(edge.toFullString());
        // triangles
        for (let triangle of this.m_Mesh.getTriangleList())
            console.log(triangle.toFullString());
        // demi-arêtes
        for (let halfedge of this.m_Mesh.getHalfEdgeList())
            console.log(halfedge.toFullString());
    }
}
