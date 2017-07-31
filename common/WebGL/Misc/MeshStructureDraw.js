/**
 * Ce module permet de dessiner les arêtes et normales d'un maillage
 *
 */


// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/DeferredShadingMaterial.js");



class MeshStructureDraw
{
    /**
     * Définition de la classe MeshStructureDraw
     * Cette classe permet de dessiner la structure d'un maillage
     * - triangles : visibles, opaques ou absents
     * - arêtes : visibles ou non
     * - normales de triangles et de sommets : visibles ou non
     * Voir les méthodes nextTrianglesMode, nextEdgesMode et nextNormalsMode
     * @param mesh : maillage à gérer avec ce renderer
     * @param trianglesMaterial : matériau pour dessiner les triangles
     * @param edgesColor : vec3 donnant la couleur pour dessiner les arêtes, peut être null
     * @param faceNormalsColor : peut être null, vec3 donnant la couleur des normales de triangles
     * @param vertexNormalsColor : peut être null, vec3 donnant la couleur des normales de sommets
     * @param length : longueur des normales, dépend de la dimension du maillage, 1.0 par défaut
     */
    constructor(mesh, trianglesMaterial, edgesColor, faceNormalsColor, vertexNormalsColor, length)
    {
        // maillage et matériau gérés
        this.m_Mesh = mesh;
        this.m_MatTriangles = trianglesMaterial;
        this.m_NormalsLength = length;

        // options initiales
        this.m_DrawTriangles = 1;
        this.m_DrawEdges = false;
        this.m_DrawNormals = 0;

        // créer le matériau pour les arêtes
        if (edgesColor == null) {
            this.m_EdgesMaterial = new DeferredShadingMaterial(vec4.fromValues(1.0, 1.0, 1.0, 1.0));
        } else {
            this.m_EdgesMaterial = new DeferredShadingMaterial(edgesColor);
        }

        // créer le matériau pour les normales de triangles
        if (faceNormalsColor == null) {
            this.m_FaceNormalsMaterial = new DeferredShadingMaterial(vec4.fromValues(1.0, 0.0, 0.0, 1.0));
        } else {
            this.m_FaceNormalsMaterial = new DeferredShadingMaterial(faceNormalsColor);
        }

        // créer le matériau pour les normales de sommets
        if (vertexNormalsColor == null) {
            this.m_VertexNormalsMaterial = new DeferredShadingMaterial(vec4.fromValues(0.0, 1.0, 0.0, 1.0));
        } else {
            this.m_VertexNormalsMaterial = new DeferredShadingMaterial(vertexNormalsColor);
        }

        // VBOset pour dessiner les triangles, les arêtes et les normales, voir onDraw
        // la création est retardée pour ne créer que le strict nécessaire et à cause des objets chargés en différé (voir MeshModuleLoading.js#loadObjFile)
        this.m_VBOsetTriangles = null;
        this.m_VBOsetEdges = null;
        this.m_VBOsetFaceNormals = null;
        this.m_VBOsetVertexNormals = null;
    }


    /**
     * Cette méthode indique d'utiliser le mode suivant pour les triangles
     * - triangles dessinés normalement
     * - triangles comme s'ils étaient opaques mais pas dessinés en couleur
     * - triangles pas dessinés du tout
     */
    nextTrianglesMode()
    {
        // incrémenter le mode modulo 3 : 0, 1, 2, 0...
        this.m_DrawTriangles = (this.m_DrawTriangles + 1) % 3;
        // forcer le mode 1 s'il n'y a pas d'arêtes
        if (!this.m_DrawEdges) this.m_DrawTriangles = 1;
    }


    /**
     * Cette méthode fait utiliser le mode suivant pour les arêtes
     * - pas d'arêtes, dans ce cas, ça force l'affichage des triangles normalement
     * - arêtes dessinées devant les triangles.
     */
    nextEdgesMode()
    {
        // inverser l'affichage des arêtes
        this.m_DrawEdges = !this.m_DrawEdges;
        // forcer le mode triangles s'il n'y a pas d'arêtes
        if (!this.m_DrawEdges) this.m_DrawTriangles = 1;
    }


    /** Cette méthode demande de dessiner les normales
     * - aucune normale
     * - normales de triangles (en rouge)
     * - normales de sommets (en vert)
     * - les deux types de normales
     */
    nextNormalsMode()
    {
        // incrémenter le mode de dessin des normales modulo 4 : 0, 1, 2, 3, 0...
        this.m_DrawNormals = (this.m_DrawNormals + 1) % 4;
    }


    /**
     * dessin avec les modes configurés
     * @param mat4Projection : mat4 de projection des sommets
     * @param mat4ModelView : mat4 de positionnement de l'objet dans la scène
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // module de dessin
        let renderer = new MeshModuleDrawing(this.m_Mesh);

        // créer les VBO qui manquent par rapport aux modes de dessin demandés
        if (this.m_DrawTriangles > 0 && this.m_VBOsetTriangles == null) {
            this.m_VBOsetTriangles = renderer.createStripVBOset(this.m_MatTriangles, true);
            if (this.m_VBOsetTriangles == null) return;
        }
        if (this.m_DrawEdges && this.m_VBOsetEdges == null) {
            // créer le VBO de dessin des arêtes
            this.m_VBOsetEdges = renderer.createEdgesVBOset(this.m_EdgesMaterial);
            if (this.m_VBOsetEdges == null) return;
        }
        if (this.m_DrawNormals%2 == 1 && this.m_VBOsetFaceNormals == null) {
            // créer le VBO de dessin des normales de facettes
            this.m_VBOsetFaceNormals = renderer.createFacesNormalsVBOset(this.m_FaceNormalsMaterial, this.m_NormalsLength);
            if (this.m_VBOsetFaceNormals == null) return;
        }
        if (this.m_DrawNormals >= 2 && this.m_VBOsetVertexNormals == null) {
            // créer le VBO de dessin des normales de sommets
            this.m_VBOsetVertexNormals = renderer.createVertexNormalsVBOset(this.m_VertexNormalsMaterial, this.m_NormalsLength);
            if (this.m_VBOsetVertexNormals == null) return;
        }

        // préparer le dessin des arêtes
        if (this.m_DrawEdges) {
            // mettre en place un décalage de polygone
            gl.enable(gl.POLYGON_OFFSET_FILL);
            gl.polygonOffset(1.0, 1.0);
        }

        // selon le mode de dessin des triangles
        switch (this.m_DrawTriangles) {
            case 1:
                // dessiner les triangles
                this.m_VBOsetTriangles.onDraw(mat4Projection, mat4ModelView);
                break;
            case 2:
                // ne pas dessiner sur l'écran (seulement sur le Depth Buffer)
                gl.colorMask(false, false, false, false);
                // dessiner les triangles
                this.m_VBOsetTriangles.onDraw(mat4Projection, mat4ModelView);
                // dessiner à nouveau sur l'écran
                gl.colorMask(true, true, true, true);
                break;
        }

        if (this.m_DrawEdges) {
            // supprimer le décalage
            gl.polygonOffset(0, 0);
            gl.disable(gl.POLYGON_OFFSET_FILL);

            // dessiner les arêtes
            gl.lineWidth(1.2);
            this.m_VBOsetEdges.onDraw(mat4Projection, mat4ModelView);
        }

        if (this.m_DrawNormals%2 == 1 && this.m_VBOsetFaceNormals != null) {
            this.m_VBOsetFaceNormals.onDraw(mat4Projection, mat4ModelView);
        }

        if (this.m_DrawNormals >= 2 && this.m_VBOsetVertexNormals != null) {
            this.m_VBOsetVertexNormals.onDraw(mat4Projection, mat4ModelView);
        }
    }


    /** destructeur */
    destroy()
    {
        if (this.m_EdgesMaterial != null)
            this.m_EdgesMaterial.destroy();
        if (this.m_FaceNormalsMaterial != null)
            this.m_FaceNormalsMaterial.destroy();
        if (this.m_VertexNormalsMaterial != null)
            this.m_VertexNormalsMaterial.destroy();

        if (this.m_VBOsetTriangles != null)
            this.m_VBOsetTriangles.destroy();
        if (this.m_VBOsetEdges != null)
            this.m_VBOsetEdges.destroy();
        if (this.m_VBOsetFaceNormals != null)
            this.m_VBOsetFaceNormals.destroy();
        if (this.m_VBOsetVertexNormals != null)
            this.m_VBOsetVertexNormals.destroy();
    }
}
