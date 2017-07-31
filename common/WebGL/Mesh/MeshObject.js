// Définition de la classe MeshObject

Requires("libs/Mesh/Mesh.js");
Requires("libs/Material/Material.js");


/**
 * Crée un ensemble de maillages et de matériaux
 */
class MeshObject
{
    /**
     * Crée un ensemble de maillages et de matériaux
     */
    constructor()
    {
        // paramètres
        this.m_Ready = false;

        // maillages et VBOset
        this.m_Meshes = {};
        this.m_VBOsets = {};
    }


    /** destructeur */
    destroy()
    {
        for (let matname in this.m_VBOsets) {
            this.m_VBOsets[matname].destroy();
        }
        for (let matname in this.m_Meshes) {
            this.m_Meshes[matname].destroy();
        }
        for (let matname in this.m_Materials) {
            this.m_Materials[matname].destroy();
        }
    }


    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // les matériaux ont-ils été chargés ?
        if (this.m_Materials == null) return;

        // dessiner les maillages
        for (let matname in this.m_VBOsets) {
            let vboset = this.m_VBOsets[matname];
            if (vboset != null) {
                vboset.onDraw(mat4Projection, mat4ModelView);
            }
        }
    }


    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    setClipPlane(active=false, plane=null)
    {
        // si l'objet n'est pas chargé, on ne fait rien
        if (this.m_Materials == null) return;

        // fournir le plan de coupe aux matériaux
        for (let matname in this.m_Materials) {
            this.m_Materials[matname].setClipPlane(active, plane);
        }
    }
}
