// Définition de la classe Cube3Mats

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("RedMaterial");
Requires("GreenMaterial");
Requires("ProceduralMaterial");


class Cube3Mats
{
    /**
     * Constructeur
     */
    constructor()
    {
        /// matériaux utilisés
        this.m_RedMaterial = new RedMaterial();
        this.m_GreenMaterial = new GreenMaterial();
        this.m_ProceduralMaterial = new ProceduralMaterial();

        // module de dessin
        let renderer = new MeshModuleDrawing();

        // maillage et VBOset pour la gauche et droite
        let greenMatMesh = new Mesh("Vert");
        this.addFace(greenMatMesh,  1);
        this.addFace(greenMatMesh, -1);
        renderer.setMesh(greenMatMesh);
        this.m_VBOsetVert = renderer.createStripVBOset(this.m_GreenMaterial, true);

        // maillage et VBOset pour le dessus et le dessous
        let proceduralMatMesh = new Mesh("Procedural");
        this.addFace(proceduralMatMesh,  2);
        this.addFace(proceduralMatMesh, -2);
        renderer.setMesh(proceduralMatMesh);
        this.m_VBOsetProcedural = renderer.createStripVBOset(this.m_ProceduralMaterial, true);

        // maillage et VBOset pour l'avant et l'arrière
        let redMatMesh = new Mesh("Rouge");
        this.addFace(redMatMesh,  3);
        this.addFace(redMatMesh, -3);
        renderer.setMesh(redMatMesh);
        this.m_VBOsetRed = renderer.createStripVBOset(this.m_RedMaterial, true);

        // suppression des objets temporaire dynamiquement alloués
        greenMatMesh.destroy();
        proceduralMatMesh.destroy();
        redMatMesh.destroy();
    }


    /**
     * Cette fonction ajoute une face perpendiculaire à l'axe au maillage fourni
     * @param mesh : maillage auquel on rajoute la face
     * @param axis : n° de la coordonnée + 1 qui donne la normale, ex: 2 pour Y, son signe donne le sens
     */
    addFace(mesh, axis)
    {
        let sign_axis = Math.sign(axis);
        axis = Math.abs(axis) - 1;

        // variables temporaires représentant les sommets
        let A,B,C,D, coords;

        // sommet A
        A = mesh.addVertex("a");
        coords = permutation([sign_axis, -1, -1], axis);
        A.setCoord(vec3.fromValues(coords[0], coords[1], coords[2]));
        A.setTexCoord(vec2.fromValues(0,0));

        // sommet B
        B = mesh.addVertex("b");
        coords = permutation([sign_axis, +1, -1], axis);
        B.setCoord(vec3.fromValues(coords[0], coords[1], coords[2]));
        B.setTexCoord(vec2.fromValues(1,0));

        // sommet C
        C = mesh.addVertex("c");
        coords = permutation([sign_axis, +1, +1], axis);
        C.setCoord(vec3.fromValues(coords[0], coords[1], coords[2]));
        C.setTexCoord(vec2.fromValues(1,1));

        // sommet D
        D = mesh.addVertex("d");
        coords = permutation([sign_axis, -1, +1], axis);
        D.setCoord(vec3.fromValues(coords[0], coords[1], coords[2]));
        D.setTexCoord(vec2.fromValues(0,1));

        // ajout d'un quad sur ces 4 sommets
        if (sign_axis > 0) {
            mesh.addQuad(A, B, C, D);
        } else {
            mesh.addQuad(D, C, B, A);
        }
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        this.m_VBOsetRed.onDraw(mat4Projection, mat4ModelView);
        this.m_VBOsetVert.onDraw(mat4Projection, mat4ModelView);
        this.m_VBOsetProcedural.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_ProceduralMaterial.destroy();
        this.m_GreenMaterial.destroy();
        this.m_RedMaterial.destroy();
    }
}


/**
 * Cette fonction fait passer n fois le dernier élément de v tout au début
 * @param v liste à permuter, ex: [ a, b, c, d ]
 * @param n nombre de fois qu'il faut répéter la permutation
 * @return [ d, a, b, c ] si n = 1
 */
function permutation(v, n)
{
    for (let i=0; i<n; i++) {
        let last = v.pop();
        v.unshift(last)
    }
    return v;
}
