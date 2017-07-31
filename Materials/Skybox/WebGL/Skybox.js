// Définition de la classe Skybox

// superclasses et classes nécessaires
Requires("SkyboxMaterial");
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class Skybox
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le maillage du cube
        this.m_Mesh = new Mesh("Skybox");

        // rajouter les sommets
        let i = 0;
        for (let z=-1; z<=1; z+=2) {
            for (let y=-1; y<=1; y+=2) {
                for (let x=-1; x<=1; x+=2) {
                    let v = this.m_Mesh.addVertex("v"+i);
                    i++;
                    v.setCoord(vec3.fromValues(x,y,z));
                }
            }
        }

        // rajouter les 6 faces vues de l'intérieur
        let faces = [ [3,2,0,1], [6,7,5,4], [4,5,1,0], [7,6,2,3], [5,7,3,1], [0,2,6,4] ];
        for (let i=0; i<faces.length; i++) {
            let face = faces[i];
            this.m_Mesh.addQuad(
                this.m_Mesh.getVertex(face[0]),
                this.m_Mesh.getVertex(face[1]),
                this.m_Mesh.getVertex(face[2]),
                this.m_Mesh.getVertex(face[3]));
        }

        // créer le matériau du cube
        this.m_Material = new SkyboxMaterial("data/textures/Teide");

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // désactiver depth test et modification du depth buffer
        //gl.disable(gl.DEPTH_TEST);
        gl.depthMask(false);

        // dessiner le cube avec son matériau
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);

        // réactiver le depth buffer
        //gl.enable(gl.DEPTH_TEST);
        gl.depthMask(true);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Material.destroy();
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
    }
}
