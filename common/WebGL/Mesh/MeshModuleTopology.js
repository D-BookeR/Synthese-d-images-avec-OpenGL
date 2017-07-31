// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModule.js");


﻿/**
 * Ce module rajoute des fonctions et des classes pour créer des maillages particuliers
 *
 */
class MeshModuleTopology extends MeshModule
{
    /**
     * initialise un générateur de nappe sur le maillage fourni
     * @param mesh : maillage à gérer
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
     * ajoute une nappe en forme de rectangle ayant nbX sommets sur l'axe X et nbZ sur l'axe Z.
     * @param nbX : nombre de sommets sur l'axe X
     * @param nbZ : nombre de sommets sur l'axe Z
     * @param names : noms des sommets, doit contenir deux %d, ex "grille%d-%d" qui sont remplacés par ix et iz
     * @param foldX : true si la grille se referme sur elle-même en X
     * @param foldZ : true si la grille se referme sur elle-même en Z
     * @return num0 : numéro du premier sommet ajouté au maillage
     * NB : le numéro d'un point (ix, iz) est ix + iz*nbX + num0
     * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
     */
    addRectangularSurface(nbX, nbZ, names, foldX, foldZ)
    {
        // nombre de sommets existants déjà dans le maillage
        let num0 = this.m_Mesh.getVertexCount();

        // créer un rectangle de sommets
        for (let iz=0; iz<nbZ; iz++) {
            for (let ix=0; ix<nbX; ix++) {
                // créer le nom du sommet en replaçant les %d par ix et iz
                let name = names.replace("%d", ix).replace("%d", iz);
                // créer un sommet dans le maillage
                let vertex = this.m_Mesh.addVertex(name);
                // coordonnées par défaut du sommet
                vertex.setCoord(vec3.fromValues(ix, 0, iz));
            }
        }

        // créer les quads de la grille
        for (let iz=0; iz<nbZ-1; iz++) {
            for (let ix=0; ix<nbX-1; ix++) {
                // récupérer les 4 sommets concernés
                let v00 = this.m_Mesh.getVertex((ix  ) + (iz  )*nbX + num0);
                let v01 = this.m_Mesh.getVertex((ix  ) + (iz+1)*nbX + num0);
                let v10 = this.m_Mesh.getVertex((ix+1) + (iz  )*nbX + num0);
                let v11 = this.m_Mesh.getVertex((ix+1) + (iz+1)*nbX + num0);
                // ajouter un quad
                this.m_Mesh.addQuad(v00, v01, v11, v10);
            }
        }

        // si repli en X, rejouter des quads sur le bord Z
        if (foldX) {
            for (let iz=0; iz<nbZ-1; iz++) {
                // récupérer les 4 sommets concernés
                let v00 = this.m_Mesh.getVertex((    0) + (iz  )*nbX + num0);
                let v01 = this.m_Mesh.getVertex((    0) + (iz+1)*nbX + num0);
                let v10 = this.m_Mesh.getVertex((nbX-1) + (iz  )*nbX + num0);
                let v11 = this.m_Mesh.getVertex((nbX-1) + (iz+1)*nbX + num0);
                // ajouter un quad
                this.m_Mesh.addQuad(v10, v11, v01, v00);
            }
        }

        // si repli en Z, rejouter des quads sur le bord X
        if (foldZ) {
            for (let ix=0; ix<nbX-1; ix++) {
                // récupérer les 4 sommets concernés
                let v00 = this.m_Mesh.getVertex((ix  ) + (    0)*nbX + num0);
                let v01 = this.m_Mesh.getVertex((ix  ) + (nbZ-1)*nbX + num0);
                let v10 = this.m_Mesh.getVertex((ix+1) + (    0)*nbX + num0);
                let v11 = this.m_Mesh.getVertex((ix+1) + (nbZ-1)*nbX + num0);
                // ajouter un quad
                this.m_Mesh.addQuad(v00, v10, v11, v01);
            }
        }

        // si repli en X et Z, rajouter un quad dans le coin
        if (foldX && foldZ) {
            // récupérer les 4 sommets concernés
            let v00 = this.m_Mesh.getVertex((    0) + (    0)*nbX + num0);
            let v01 = this.m_Mesh.getVertex((    0) + (nbZ-1)*nbX + num0);
            let v10 = this.m_Mesh.getVertex((nbX-1) + (    0)*nbX + num0);
            let v11 = this.m_Mesh.getVertex((nbX-1) + (nbZ-1)*nbX + num0);
            // ajouter un quad
            this.m_Mesh.addQuad(v00, v01, v11, v10);
        }

        // retourner le numéro du premier point
        return num0;
    }


    /**
     * ajoute une nappe de triangles équilatéraux ayant nbX sommets sur l'axe X et nbZ sur l'axe Z.
     * @param nbX : nombre de sommets sur l'axe X
     * @param nbZ : nombre de sommets sur l'axe Z
     * @param names : noms des sommets, doit contenir deux %d, ex "grille%d-%d" qui sont remplacés par ix et iz
     * @param foldX : true si la grille se referme sur elle-même en X
     * @param foldZ : true si la grille se referme sur elle-même en Z
     * @return num0 : numéro du premier sommet ajouté au maillage
     * NB : le numéro d'un point (ix, iz) est ix + iz*nbX + num0
     * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
     */
    addHexagonalSurface(nbX, nbZ, names, foldX, foldZ)
    {
        // nombre de sommets existants déjà dans le maillage
        let num0 = this.m_Mesh.getVertexCount();

        // créer un rectangle de sommets
        for (let iz=0; iz<nbZ; iz++) {
            for (let ix=0; ix<nbX; ix++) {
                // créer le nom du sommet en replaçant les %d par ix et iz
                let name = names.replace("%d", ix).replace("%d", iz);
                // créer un sommet dans le maillage
                let vertex = this.m_Mesh.addVertex(name);
                // coordonnées par défaut du sommet
                vertex.setCoord(vec3.fromValues(ix-0.5*(iz%2), 0, iz * Math.sqrt(3.0)/2.0));
            }
        }

        // créer les triangles de la grille
        for (let iz=0; iz<nbZ-1; iz++) {
            for (let ix=0; ix<nbX-1; ix++) {
                // récupérer les 4 sommets concernés
                let v00 = this.m_Mesh.getVertex((ix  ) + (iz  )*nbX + num0);
                let v01 = this.m_Mesh.getVertex((ix  ) + (iz+1)*nbX + num0);
                let v10 = this.m_Mesh.getVertex((ix+1) + (iz  )*nbX + num0);
                let v11 = this.m_Mesh.getVertex((ix+1) + (iz+1)*nbX + num0);
                // selon la parité de iz, créer deux triangles dans un sens ou dans l'autre
                if (iz%2 == 0) {
                    this.m_Mesh.addTriangle(v00, v01, v11);
                    this.m_Mesh.addTriangle(v00, v11, v10);
                } else {
                    this.m_Mesh.addTriangle(v00, v01, v10);
                    this.m_Mesh.addTriangle(v10, v01, v11);
                }
            }
        }

        // si repli en X, rejouter des triangles sur le bord Z
        if (foldX) {
            /// FIXME : selon la parité du nombre de sommets en Z
            for (let iz=0; iz<nbZ-1; iz++) {
                // récupérer les 4 sommets concernés
                let v00 = this.m_Mesh.getVertex((    0) + (iz  )*nbX + num0);
                let v01 = this.m_Mesh.getVertex((    0) + (iz+1)*nbX + num0);
                let v10 = this.m_Mesh.getVertex((nbX-1) + (iz  )*nbX + num0);
                let v11 = this.m_Mesh.getVertex((nbX-1) + (iz+1)*nbX + num0);
                // selon la parité de iz, créer deux triangles dans un sens ou dans l'autre
                if (iz%2 == 0) {
                    this.m_Mesh.addTriangle(v10, v11, v01);
                    this.m_Mesh.addTriangle(v10, v01, v00);
                } else {
                    this.m_Mesh.addTriangle(v10, v11, v00);
                    this.m_Mesh.addTriangle(v00, v11, v01);
                }
            }
        }

        // si repli en Z, rejouter des quads sur le bord X
        if (foldZ) {
            // afficher un avertissement si nbZ n'est pas pair
            if (nbZ%2 != 0) console.warn("ATTENTION: addHexagonalSurface, le nombre nbZ n'est pas pair : "+nbZ);
            for (let ix=0; ix<nbX-1; ix++) {
                // récupérer les 4 sommets concernés
                let v00 = this.m_Mesh.getVertex((ix  ) + (    0)*nbX + num0);
                let v01 = this.m_Mesh.getVertex((ix  ) + (nbZ-1)*nbX + num0);
                let v10 = this.m_Mesh.getVertex((ix+1) + (    0)*nbX + num0);
                let v11 = this.m_Mesh.getVertex((ix+1) + (nbZ-1)*nbX + num0);
                this.m_Mesh.addTriangle(v01, v00, v11);
                this.m_Mesh.addTriangle(v11, v00, v10);
            }
        }

        // si repli en X et Z, rajouter un quad dans le coin
        if (foldX && foldZ) {
            // récupérer les 4 sommets concernés
            let v00 = this.m_Mesh.getVertex((    0) + (    0)*nbX + num0);
            let v01 = this.m_Mesh.getVertex((    0) + (nbZ-1)*nbX + num0);
            let v10 = this.m_Mesh.getVertex((nbX-1) + (    0)*nbX + num0);
            let v11 = this.m_Mesh.getVertex((nbX-1) + (nbZ-1)*nbX + num0);
            this.m_Mesh.addTriangle(v00, v01, v10);
            this.m_Mesh.addTriangle(v10, v01, v11);
        }

        // retourner le numéro du premier point
        return num0;
    }


    /**
     * ajoute une nappe en forme de toile d'araignée avec un point central. Cette toile contient un sommet central
     * de numéro num0 (retourné par cette méthode). Ensuite les sommets sont agencés en cercles concentriques.
     * Un sommet est noté (ir, is) avec ir = numéro de rayon (angle autour du centre) et is = distance du centre.
     * @param spokes_count : nombre de rayons => angle = 360°/spokesNumber
     * @param segments_count : nombre de segments sur les rayons, c'est le nombre de cercles de sommets autour du centre,
     * il n'y a que le centre si ce nombre est nul
     * @param names : noms des sommets, doit contenir deux %d, ex: "disque%d-%d" qui sont remplacés par ir et is
     * @return num0 : numéro du premier sommet ajouté au maillage, c'est le centre du disque
     * NB : le numéro d'un point (ir, is) est ir*nbSegments + is + num0+1, le centre est num0
     * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
     */
    addRevolutionSurface(spokes_count, segments_count, names)
    {
        // nombre de sommets existants déjà dans le maillage
        let num0 = this.m_Mesh.getVertexCount();

        // créer le vertex du centre
        let nom = names.replace("%d", "C").replace("%d", "C");
        let center = this.m_Mesh.addVertex(nom);
        center.setCoord(vec3.fromValues(0,0,0));

        // définir les vertices des cercles autour du centre
        for (let ir=0; ir<spokes_count; ir++) {
            let angle = ir/spokes_count*Math.PI*2.0;
            for (let is=0; is<segments_count; is++) {
                // créer le nom du sommet en replaçant le %d par is
                let name = names.replace("%d", ir).replace("%d", is)
                // créer un sommet dans le maillage
                let vertex = this.m_Mesh.addVertex(name);
                // coordonnées par défaut du sommet
                vertex.setCoord(vec3.fromValues((is+1.0)*Math.cos(angle), 0, (is+1.0)*Math.sin(angle)));
            }
        }

        // rajouter des triangles afin de pouvoir créer des rubans rapidement
        for (let ir=0; ir<spokes_count; ir++) {
            // rajouter les triangles juste autour du centre
            if (segments_count > 0) {
                let vertex0 = this.m_Mesh.getVertex(((ir  )%spokes_count)*segments_count + 0 + num0+1);
                let vertex1 = this.m_Mesh.getVertex(((ir+1)%spokes_count)*segments_count + 0 + num0+1);
                this.m_Mesh.addTriangle(center, vertex0, vertex1);
            }

            // rajouter les quads autour des triangles du centre
            for (let is=0; is<segments_count-1; is++) {
                // récupérer les 4 sommets concernés
                let v00 = this.m_Mesh.getVertex(((ir  )%spokes_count)*segments_count + (is  ) + num0+1);
                let v10 = this.m_Mesh.getVertex(((ir+1)%spokes_count)*segments_count + (is  ) + num0+1);
                let v01 = this.m_Mesh.getVertex(((ir  )%spokes_count)*segments_count + (is+1) + num0+1);
                let v11 = this.m_Mesh.getVertex(((ir+1)%spokes_count)*segments_count + (is+1) + num0+1);
                // ajouter un quad (ordre adéquat pour créer des rubans rapidement)
                this.m_Mesh.addQuad(v00, v01, v11, v10);
            }
        }

        // retourner le numéro du premier point
        return num0;
    }
}
