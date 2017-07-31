#ifndef MISC_MESHSTRUCTUREDRAW_H
#define MISC_MESHSTRUCTUREDRAW_H


#include <vector>
#include <map>
#include <list>
#include <string>

#include <gl-matrix.h>
#include <utils.h>

#include <Mesh.h>
#include <Material.h>
#include <VBOset.h>
#include <DeferredShadingMaterial.h>


/**
 * Définition de la classe MeshStructureDraw
 * Cette classe permet de dessiner la structure d'un maillage
 * - triangles : visibles, opaques ou absents
 * - arêtes : visibles ou non
 * - normales de triangles et de sommets : visibles ou non
 * Voir les méthodes nextTrianglesMode, nextEdgesMode et nextNormalsMode
 */
class MeshStructureDraw
{
public:

    /**
     * Constructeur
     * @param mesh maillage à gérer avec ce renderer
     * @param trianglesMaterial : matériau pour dessiner les triangles
     * @param edgesColor : vec3 donnant la couleur pour dessiner les arêtes, peut être nullptr
     * @param faceNormalsColor : peut être nullptr, vec3 donnant la couleur des normales de triangles
     * @param vertexNormalsColor : peut être nullptr, vec3 donnant la couleur des normales de sommets
     * @param length longueur des normales, dépend de la dimension du maillage, 1.0 par défaut
     */
    MeshStructureDraw(Mesh* mesh, Material* trianglesMaterial, vec4* edgesColor=nullptr, vec4* faceNormalsColor=nullptr, vec4* vertexNormalsColor=nullptr, float length=1.0);

    /**
     * Destructeur
     */
     ~MeshStructureDraw();

    /**
     * Cette méthode indique d'utiliser le mode suivant pour les triangles
     * - triangles dessinés normalement
     * - triangles comme s'ils étaient opaques mais pas dessinés en couleur
     * - triangles pas dessinés du tout
     */
    void nextTrianglesMode();

    /**
     * Cette méthode fait utiliser le mode suivant pour les arêtes
     * - pas d'arêtes, dans ce cas, ça force l'affichage des triangles normalement
     * - arêtes dessinées devant les triangles.
     */
    void nextEdgesMode();

    /** Cette méthode demande de dessiner les normales
     * - aucune normale
     * - normales de triangles (en rouge)
     * - normales de sommets (en vert)
     * - les deux types de normales
     */
    void nextNormalsMode();

    /**
     * dessin avec les modes configurés
     * @param mat4Projection : mat4 de projection des sommets
     * @param mat4ModelView : mat4 de positionnement de l'objet dans la scène
     */
    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);


protected:

    /** maillage géré par ce renderer (partage seulement, pas d'allocation) **/
    Mesh* m_Mesh;

    /** matériau pour dessiner les triangles (idem maillage) **/
    Material* m_MatTriangles;

    /** matériau pour dessiner les arêtes : créé ici **/
    Material* m_EdgesMaterial;

    /** matériau pour dessiner les normales de faces : créé ici **/
    Material* m_FaceNormalsMaterial;

    /** matériau pour dessiner les normales de sommets : créé ici **/
    Material* m_VertexNormalsMaterial;

    /** option de dessin des triangles, arêtes et normales **/
    int m_DrawTriangles;
    bool m_DrawEdges;
    int m_DrawNormals;

    /** VBOset pour dessiner les triangles **/
    VBOset* m_VBOsetTriangles;

    /** VBOset pour dessiner les arêtes **/
    VBOset* m_VBOsetEdges;

    /** VBOset pour dessiner les normales de faces **/
    VBOset* m_VBOsetFaceNormals;

    /** VBOset pour dessiner les normales de sommets **/
    VBOset* m_VBOsetVertexNormals;

    /** longueur des normales **/
    float m_NormalsLength;
};

#endif
