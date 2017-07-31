// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModuleUtils.js");


/**
 * Ce module rajoute des fonctions de transformations géométriques
 * NB: penser à appeler creerVBO() avant et après chaque transformation
 */
class MeshModuleProcessing extends MeshModuleUtils
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
     * Cette méthode fait une homothétie du triangle autour de son centre
     * @param triangle auquel on applique la transformation
     * @param scale : rapport d'agrandissement (si > 1.0) ou de réduction (si < 1.0)
     * @return triangle : le triangle auquel on applique cette méthode
     */
    static homothety(triangle, scale)
    {
        // traiter tous les sommets
        for (let i=0; i<3; i++) {
            let vertex = triangle.getVertex(i);
            let coords = vertex.getCoord();
            vec3.subtract(coords, coords, triangle.getCenter());
            vec3.scale(   coords, coords, scale);
            vec3.add(     coords, coords, triangle.getCenter());
        }
        return triangle;
    }


    /**
     * Cette méthode casse l'angle au niveau du sommet indiqué. Elle
     * enfonce le sommet de la distance selon la direction fournie en paramètre.
     * @param vertex : sommet qu'il faut tronquer
     * @param distance : distance où vont aller les points intermédiaires
     * @param direction : vec3 indiquant l'axe d'enfoncement des points
     * @return liste des sommets du contour résultant de la découpe
     */
    bevelVertex(vertex, distance, direction)
    {
        // normaliser la direction
        vec3.normalize(direction, direction);

        // liste des sommets formant le contour
        let border = [];

        // couper en deux chaque triangle entourant le sommet
        for (let triangle of vertex.getTrianglesOrderedAround()) {
            // faire tourner la liste des sommets du triangle pour mettre this en premier
            if (!triangle.cycleVertexFirst(vertex)) {
                console.error("bug dans le mesh");
                continue;
            }

            // découper de chaque côté
            let quad = [];
            quad.push(triangle.getVertex1());
            quad.push(triangle.getVertex2());
            for (let is=1; is<=2; is++) {
                let other = triangle.getVertex(is);
                let midname = MeshModuleUtils.getMidName(vertex, other);
                let mid = this.m_Mesh.getVertexByName(midname);
                if (mid == null) {
                    mid = this.m_Mesh.addVertex(midname);
                    // calculer la distance relative
                    let edge = vec3.create();
                    vec3.subtract(edge, vertex.getCoord(), other.getCoord());
                    let k = distance / vec3.dot(direction, edge);
                    // positionner le point intermédiaire à cette distance
                    mid.lerp(vertex, other, k);
                    // ajouter ce point au contour
                    border.push(mid);
                }
                // mémoriser le point intermédiaire
                quad.push(mid);
            }

            // supprimer le triangle
            triangle.destroy();

            // créer un quad entre s1, s2, m1 et m2 pour remplacer le triangle
            this.m_Mesh.addQuad(quad[0], quad[1], quad[3], quad[2]);

            // créer un triangle entre le sommet, m1 et m2
            this.m_Mesh.addTriangle(vertex, quad[2], quad[3]);
        }

        /* deux choix possibles : garder l'ancien sommet et ses anciens triangles, ou refaire un polygone */

        /* choix 1 : on garde l'ancien sommet mais on le met dans le plan du contour */
        /*

        // déplacer le sommet dans le plan de ceux du contour
        let translation = vec3.clone(direction);
        vec3.scale(translation, translation, -distance);
        vec3.add(vertex.getCoord(), vertex.getCoord(), translation);

        */
        /* choix 2 : on supprime l'ancien sommet et on recrée un polygone dans le contour */

        // supprimer le sommet
        vertex.destroy();

        // créer un polygone pour remplir le trou
        this.m_Mesh.addPolygon(border, direction);

        // recalculer les normales
        this.m_Mesh.computeNormals();

        return border;
    }


    /**
     * Cette méthode clone les sommets spécifiés dans le contour afin
     * de la rendre dure (séparer deux groupes de lissage de part et d'autre
     * NB: ce sont les sommets des triangles "de gauche" qui sont clonés
     * @param border : liste de sommets (l'ordre est important), il en faut au moins 2
     */
    splitBorder(border)
    {
        // si la liste est trop courte, sortir sans rien faire
        let vertex_count = border.length;
        if (vertex_count < 2) return;

        // liste des triangles dont les sommets vont être clonés
        let halfedges = this.getHalfEdgesAlongBorder(border);
        let triangles = this.getTrianglesInsideBorder(halfedges);

        // liste des clones, il y en a autant que de sommets dans l'arête
        let clones = [];
        for (let i=0; i<vertex_count; i++) clones.push(null);

        // remplacer les sommets de l'arête dans tous les triangles
        for (let triangle of triangles) {
            // remplacer tous les sommets qui sont dans l'arête par leurs clones
            for (let is=0; is<vertex_count; is++) {
                let vertex = border[is];
                let clone  = clones[is];
                // le triangle contient-il ce sommet ?
                if (triangle.containsVertex(vertex)) {
                    // cloner le sommet, sauf s'il l'est déjà
                    if (clone == null) {
                        clone = vertex.clone("clone");
                        clones[is] = clone;
                    }
                    // remplacer le sommet par son clone
                    triangle.replaceVertex(vertex, clone);
                }
            }
        }
    }


    /**
     * Cette méthode fait une extrusion des triangles contenus dans le contour
     * et de la distance indiquée
     * @param border : liste de sommets, délimitent les triangles concernés qui sont à gauche du contour
     * @param distance : longueur de l'extrusion
     * @return liste des clones des sommets de tous les triangles inclus dans le contour
     */
    extrudePolygon(border, distance)
    {
        // liste des triangles dont les sommets vont être clonés
        let halfedges = this.getHalfEdgesAlongBorder(border);
        let triangles = this.getTrianglesInsideBorder(halfedges);
        if (triangles.length == 0) return [];
        let vertex_count = border.length;

        // direction de l'extrusion : moyenne normalisée des normales des triangles
        let direction = MeshModuleUtils.averageNormals(triangles);

        // définir le décalage à partir de la direction et la distance
        let offset = vec3.create();
        vec3.scale(offset, direction, distance);

        // liste des clones, il y en a autant que de sommets dans le contour
        let clones = [];
        for (let vertex of border) {
            let clone = vertex.clone("clone");
            clones.push(clone);
        }

        // remplacer les sommets par leurs clones dans tous les triangles
        for (let triangle of triangles) {
            // remplacer tous les sommets par leurs clones
            for (let iv=0; iv<vertex_count; iv++) {
                // sommet courant et son clone
                let vertex = border[iv];
                let clone = clones[iv];
                // remplacer le sommet par son clone (si sommet est dans ce triangle)
                triangle.replaceVertex(vertex, clone);
            }
        }

        // construire la liste des sommets contenus dans ces triangles
        let vertex_list = MeshModuleUtils.getVerticesFromTriangles(triangles);

        // décaler les sommets
        for (let vertex of vertex_list) {
            // décaler le sommet
            vec3.add(vertex.getCoord(), vertex.getCoord(), offset);
        }

        // construire des quads avec chaque couple de sommets du contour
        for (let ic=0; ic<vertex_count; ic++) {
            // sommet courant et son clone
            let A = border[ic];
            let A1 = clones[ic];
            // sommet suivant et son clone
            let B = border[(ic+1)%vertex_count];
            let B1 = clones[(ic+1)%vertex_count];
            // construire un quad sur ces 4 sommets
            this.m_Mesh.addQuad(A, B, B1, A1);
        }

        return clones;
    }


    /**
     * Cette méthode fait une extrusion du triangle indiqué, le long de sa normale
     * et de la distance indiquée
     * @param triangle : celui qu'il faut extruder
     * @param distance : longueur de l'extrusion
     * @return nouveau triangle résultat de l'extrusion
     */
    extrudeTriangle(triangle, distance)
    {
        // sommets du triangle existant
        let A = triangle.getVertex0();
        let B = triangle.getVertex1();
        let C = triangle.getVertex2();

        // définir le décalage à partir de la normale et la distance
        let offset = vec3.create();
        vec3.scale(offset, triangle.getNormal(), distance);

        // décaler les sommets dans la direction de la normale
        let A1 = A.clone("clone");
        vec3.add(A1.getCoord(), A.getCoord(), offset);
        let B1 = B.clone("clone");
        vec3.add(B1.getCoord(), B.getCoord(), offset);
        let C1 = C.clone("clone");
        vec3.add(C1.getCoord(), C.getCoord(), offset);

        // remplacer les sommets par leurs clones dans le triangle (A,B,C)
        triangle.replaceVertex(A, A1);
        triangle.replaceVertex(B, B1);
        triangle.replaceVertex(C, C1);

        // créer un quad sur chacun des trois flancs
        this.m_Mesh.addQuad(A, B, B1, A1);
        this.m_Mesh.addQuad(B, C, C1, B1);
        this.m_Mesh.addQuad(C, A, A1, C1);

        // retourner le triangle modifié
        return triangle;
    }


    /**
     * Cette méthode subdivise le triangle en sous-triangles
     * @param triangle auquel on applique la transformation
     * @param steps : fournir un entier 1..raisonnable
     * @param smooth : déplace ou non les points midpoints vers l'intérieur ou l'extérieur
     * @return liste de triangles qui doivent remplacer celui-ci
     */
    subdivide(triangle, steps, smooth)
    {
        let result = [];

        // fin de la récursivité ?
        if (steps <= 0) {
            // ajouter le triangle au résultat et le retourner
            return [ triangle ];
        }
        steps -= 1;

        // noter les trois sommets du triangle
        let vertex_list = [
                triangle.getVertex0(),
                triangle.getVertex1(),
                triangle.getVertex2()
        ];

        // sommets intermédiaires (dans l'ordre des sommets)
        let midpoints = [];

        // traiter chaque arête
        for (let is=0; is<3; is++) {
            // extrémités de l'arête
            let s0 = triangle.getVertex(is);
            let s1 = triangle.getVertex((is+1)%3);
            // milieu entre s0 et s1
            let name01 = MeshModuleUtils.getMidName(s0, s1);
            let m01 = this.m_Mesh.getVertexByName(name01);
            if (m01 == null) {
                m01 = new MeshVertex(this.m_Mesh, name01);
                // lissage ?
                if (smooth > 0.0) {
                    // vecteur s0s1
                    let s0s1 = vec3.create();
                    vec3.subtract(s0s1, s1.getCoord(), s0.getCoord());
                    vec3.scale(s0s1, s0s1, smooth);
                    // normale, binormale et tangente en s0
                    let n0 = s0.getNormal();
                    let b0 = vec3.create();
                    vec3.cross(b0, s0s1, n0);
                    let t0 = vec3.create();
                    vec3.cross(t0, n0, b0);
                    // normale, binormale et tangente en s1
                    let n1 = s1.getNormal();
                    let b1 = vec3.create();
                    vec3.cross(b1, s0s1, n1);
                    let t1 = vec3.create();
                    vec3.cross(t1, n1, b1);
                    // interpolation cubique spline de Hermite
                    m01.hermite(s0, t0, s1, t1, 0.5);
                } else {
                    // simple interpolation linéaire
                    m01.lerp(s0, s1, 0.5);
                }
            }
            midpoints.push(m01);
        }

        // supprimer le triangle
        triangle.destroy();

        // créer 3 triangles à l'intérieur de this (sens trigo)
        for (let is=0; is<3; is++) {
            let s0 = vertex_list[is];
            let m01 = midpoints[is];
            let m20 = midpoints[(is+2)%3];
            let triangle_tmp = new MeshTriangle(this.m_Mesh, s0, m01, m20);
            result = result.concat(this.subdivide(triangle_tmp, steps, smooth));
        }

        // triangle central
        let triangle_tmp = new MeshTriangle(this.m_Mesh, midpoints[0], midpoints[1], midpoints[2]);
        result = result.concat(this.subdivide(triangle_tmp, steps, smooth));

        return result;
    }


    /**
     * Cette méthode subdivise les triangles indiqués
     * @param triangles : liste des triangles concernés par la subdivision
     * @param steps : fournir un entier 1..raisonnable
     * @param smooth : déplace ou non les points milieux vers l'intérieur ou l'extérieur
     * @return liste de triangles qui doivent remplacer celui-ci
     */
    subdivideAll(triangles, steps, smooth)
    {
        // mémoriser les triangles qui sont subdivisés
        let result = [];

        // liste des triangles dont les sommets vont être clonés
        let nbtriangles = triangles.length;
        if (nbtriangles == 0) return result;

        // faut-il faire quelque chose ?
        if (steps <= 0) return triangles;

        // si la liste qui est fournie est celle des triangles de this, alors la cloner parce qu'on va la modifier
        if (triangles === this.m_Mesh.getTriangleList()) {
            triangles = this.m_Mesh.getTriangleList().slice();
        }

        // subdiviser tous les triangles
        for (let triangle of triangles) {
            result = result.concat(this.subdivide(triangle, steps, smooth));
        }
        return result;
    }


    /**
     * Cette méthode applique une matrice à tous les sommets du maillage
     * @param matrix : la transformation qu'il faut appliquer sur les sommets
     */
    transform(matrix)
    {
        for (let vertex of this.m_Mesh.getVertexList()) {
            vec3.transformMat4(vertex.getCoord(), vertex.getCoord(), matrix);
        }
    }
}
