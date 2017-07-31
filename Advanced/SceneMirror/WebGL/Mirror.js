// Définition de la classe Mirror

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/ColorMaterial.js");


/**
 * Classe Mirror = encadrement et surface réfléchissante
 */
class Mirror
{
    /**
     * Constructeur
     */
    constructor()
    {
        // module pour créer les VBOsets
        let renderer = new MeshModuleDrawing();

        /// Partie miroir

        // créer le maillage du miroir
        this.m_MirrorMesh = new Mesh("miroir");

        // sommets
        let P0m = this.m_MirrorMesh.addVertex("P0");
        P0m.setCoord(vec3.fromValues(-1.0, 0, 0));
        let P1m = this.m_MirrorMesh.addVertex("P1");
        P1m.setCoord(vec3.fromValues(+1.0, 0, 0));
        let P2m = this.m_MirrorMesh.addVertex("P2");
        P2m.setCoord(vec3.fromValues(+1.0, +2.5, 0));
        let P3m = this.m_MirrorMesh.addVertex("P3");
        P3m.setCoord(vec3.fromValues(-1.0, +2.5, 0));

        // quadrilatère
        this.m_MirrorMesh.addQuad(P0m, P1m, P2m, P3m);

        // créer le matériau du rectangle
        this.m_MirrorMaterial = new ColorMaterial(vec4.fromValues(0.1, 0.1, 0.2, 1.0));

        // créer les VBO pour afficher ce maillage
        renderer.setMesh(this.m_MirrorMesh);
        this.m_VBOsetMirror = renderer.createStripVBOset(this.m_MirrorMaterial, true);

        /// Partie Cadre

        // créer le maillage du cadre
        this.m_FrameMesh = new Mesh("cadre");

        // coordonnées des sommets
        let P0c = vec3.fromValues(-1.0, 0, -0.01);
        let P1c = vec3.fromValues(+1.0, 0, -0.01);
        let P2c = vec3.fromValues(+1.0, +2.5, -0.01);
        let P3c = vec3.fromValues(-1.0, +2.5, -0.01);
        let P4c = vec3.fromValues(-1.0, 0, -0.2);
        let P5c = vec3.fromValues(+1.0, 0, -0.2);
        let P6c = vec3.fromValues(+1.0, +2.5, -0.2);
        let P7c = vec3.fromValues(-1.0, +2.5, -0.2);

        // sorte de cube
        this.addFrameQuad("droit",  vec3.fromValues(+1,0,0), P1c, P5c, P6c, P2c);
        this.addFrameQuad("haut",   vec3.fromValues(0,+1,0), P2c, P6c, P7c, P3c);
        this.addFrameQuad("gauche", vec3.fromValues(-1,0,0), P3c, P7c, P4c, P0c);
        this.addFrameQuad("bas",    vec3.fromValues(0,-1,0), P0c, P4c, P5c, P1c);
        this.addFrameQuad("avant",  vec3.fromValues(0,0,-1), P5c, P4c, P7c, P6c);
        this.addFrameQuad("dos",    vec3.fromValues(0,0,+1), P0c, P1c, P2c, P3c);

        // créer le matériau du cadre
        this.m_FrameMaterial = new ColorMaterial(vec4.fromValues(0.6, 0.4, 0.2, 1.0));

        // créer les VBO pour afficher ce maillage
        renderer.setMesh(this.m_FrameMesh);
        this.m_VBOsetFrame = renderer.createStripVBOset(this.m_FrameMaterial, true);
    }


    /**
     * ajout d'une face (a,b,c,d) au maillage du cadre
     * @param name : nom de la face à créer
     * @param normal : vec3 à affecter aux 4 sommets
     * @param a : coordonnées
     * @param b : coordonnées
     * @param c : coordonnées
     * @param d : coordonnées
     */
    addFrameQuad(name, normal, a,b,c,d)
    {
        let A = this.m_FrameMesh.addVertex(name+"A");
        A.setCoord(a);
        A.setNormal(normal);

        let B = this.m_FrameMesh.addVertex(name+"B");
        B.setCoord(b);
        B.setNormal(normal);

        let C = this.m_FrameMesh.addVertex(name+"C");
        C.setCoord(c);
        C.setNormal(normal);

        let D = this.m_FrameMesh.addVertex(name+"D");
        D.setCoord(d);
        D.setNormal(normal);

        this.m_FrameMesh.addQuad(A, B, C, D);
    }


    /**
     * dessine le cadre du miroir
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDrawFrame(mat4Projection, mat4ModelView)
    {
        this.m_VBOsetFrame.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * dessine la surface du miroir
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDrawSurface(mat4Projection, mat4ModelView)
    {
        this.m_VBOsetMirror.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    setClipPlane(active=false, plane=null)
    {
        // fournir le plan de coupe aux matériaux
        this.m_MirrorMaterial.setClipPlane(active, plane);
        this.m_FrameMaterial.setClipPlane(active, plane);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_VBOsetFrame.destroy();
        this.m_FrameMaterial.destroy();
        this.m_FrameMesh.destroy();
        this.m_VBOsetMirror.destroy();
        this.m_MirrorMaterial.destroy();
        this.m_MirrorMesh.destroy();
    }
}
