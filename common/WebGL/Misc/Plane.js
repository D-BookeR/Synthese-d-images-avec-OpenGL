/**
 * Cette classe permet de dessiner un mesh en forme de plan
 */

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/VBOset.js");


/**
 * crée un plan
 * @param material : matériau à appliquer sur le plan
 */
class Plane
{
    constructor(material)
    {
        let mesh = new Mesh("Plane");

        let s1 = mesh.addVertex("s1");
        let s2 = mesh.addVertex("s2");
        let s3 = mesh.addVertex("s3");
        let s4 = mesh.addVertex("s4");
        mesh.addQuad(s1, s2, s3, s4);
        s1.setCoord(vec3.fromValues(-1,0,+1)); s1.setTexCoord(vec2.fromValues(0.0, 1.0)); s1.setNormal(vec3.fromValues(0,+1,0));
        s2.setCoord(vec3.fromValues(+1,0,+1)); s2.setTexCoord(vec2.fromValues(1.0, 1.0)); s2.setNormal(vec3.fromValues(0,+1,0));
        s3.setCoord(vec3.fromValues(+1,0,-1)); s3.setTexCoord(vec2.fromValues(1.0, 0.0)); s3.setNormal(vec3.fromValues(0,+1,0));
        s4.setCoord(vec3.fromValues(-1,0,-1)); s4.setTexCoord(vec2.fromValues(0.0, 0.0)); s4.setNormal(vec3.fromValues(0,+1,0));

        // calculer les normales
        mesh.computeNormals();

        // préparer le VBOset pour le dessiner
        let renderer = new MeshModuleDrawing(mesh);
        this.m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /** destructeur */
    destroy()
    {
        this.m_VBOset.destroy();
    }
}
