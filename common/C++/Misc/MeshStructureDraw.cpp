#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>

#include <utils.h>
#include <Mesh.h>
#include <MeshModuleDrawing.h>
#include <DeferredShadingMaterial.h>

#include <MeshStructureDraw.h>


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
MeshStructureDraw::MeshStructureDraw(Mesh* mesh, Material* trianglesMaterial, vec4* edgesColor, vec4* faceNormalsColor, vec4* vertexNormalsColor, float length)
{
    // maillage et matériau gérés
    m_Mesh = mesh;
    m_MatTriangles = trianglesMaterial;
    m_NormalsLength = length;

    // options initiales
    m_DrawTriangles = 1;
    m_DrawEdges = false;
    m_DrawNormals = 0;

    // créer le matériau pour les arêtes
    if (edgesColor == nullptr) {
        m_EdgesMaterial = new DeferredShadingMaterial(vec4::fromValues(1.0, 1.0, 1.0, 1.0));
    } else {
        m_EdgesMaterial = new DeferredShadingMaterial(*edgesColor);
    }

    // créer le matériau pour les normales de triangles
    if (faceNormalsColor == nullptr) {
        m_FaceNormalsMaterial = new DeferredShadingMaterial(vec4::fromValues(1.0, 0.0, 0.0, 1.0));
    } else {
        m_FaceNormalsMaterial = new DeferredShadingMaterial(*faceNormalsColor);
    }

    // créer le matériau pour les normales de sommets
    if (vertexNormalsColor == nullptr) {
        m_VertexNormalsMaterial = new DeferredShadingMaterial(vec4::fromValues(0.0, 1.0, 0.0, 1.0));
    } else {
        m_VertexNormalsMaterial = new DeferredShadingMaterial(*vertexNormalsColor);
    }

    // VBOset pour dessiner les triangles, les arêtes et les normales, voir onDraw
    // la création est retardée pour ne créer que le strict nécessaire
    m_VBOsetTriangles = nullptr;
    m_VBOsetEdges = nullptr;
    m_VBOsetFaceNormals = nullptr;
    m_VBOsetVertexNormals = nullptr;
}


/**
 * Cette méthode indique d'utiliser le mode suivant pour les triangles
 * - triangles dessinés normalement
 * - triangles comme s'ils étaient opaques mais pas dessinés en couleur
 * - triangles pas dessinés du tout
 */
void MeshStructureDraw::nextTrianglesMode()
{
    // incrémenter le mode modulo 3 : 0, 1, 2, 0...
    m_DrawTriangles = (m_DrawTriangles + 1) % 3;
    // forcer le mode 1 s'il n'y a pas d'arêtes
    if (!m_DrawEdges) m_DrawTriangles = 1;
}


/**
 * Cette méthode fait utiliser le mode suivant pour les arêtes
 * - pas d'arêtes, dans ce cas, ça force l'affichage des triangles normalement
 * - arêtes dessinées devant les triangles.
 */
void MeshStructureDraw::nextEdgesMode()
{
    // inverser l'affichage des arêtes
    m_DrawEdges = !m_DrawEdges;
    // forcer le mode triangles s'il n'y a pas d'arêtes
    if (!m_DrawEdges) m_DrawTriangles = 1;
}


/** Cette méthode demande de dessiner les normales
 * - aucune normale
 * - normales de triangles (en rouge)
 * - normales de sommets (en vert)
 * - les deux types de normales
 */
void MeshStructureDraw::nextNormalsMode()
{
    // incrémenter le mode de dessin des normales modulo 4 : 0, 1, 2, 3, 0...
    m_DrawNormals = (m_DrawNormals + 1) % 4;
}


/**
 * dessin avec les modes configurés
 * @param mat4Projection : mat4 de projection des sommets
 * @param mat4ModelView : mat4 de positionnement de l'objet dans la scène
 */
void MeshStructureDraw::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    // module de dessin
    MeshModuleDrawing renderer(m_Mesh);

    // créer les VBO qui manquent par rapport aux modes de dessin demandés
    if (m_DrawTriangles > 0 && m_VBOsetTriangles == nullptr) {
        m_VBOsetTriangles = renderer.createStripVBOset(m_MatTriangles, true);
        if (m_VBOsetTriangles == nullptr) return;
    }
    if (m_DrawEdges && m_VBOsetEdges == nullptr) {
        // créer le VBO de dessin des arêtes
        m_VBOsetEdges = renderer.createEdgesVBOset(m_EdgesMaterial);
        if (m_VBOsetEdges == nullptr) return;
    }
    if (m_DrawNormals%2 == 1 && m_VBOsetFaceNormals == nullptr) {
        // créer le VBO de dessin des normales de facettes
        m_VBOsetFaceNormals = renderer.createFacesNormalsVBOset(m_FaceNormalsMaterial, m_NormalsLength);
        if (m_VBOsetFaceNormals == nullptr) return;
    }
    if (m_DrawNormals >= 2 && m_VBOsetVertexNormals == nullptr) {
        // créer le VBO de dessin des normales de sommets
        m_VBOsetVertexNormals = renderer.createVertexNormalsVBOset(m_VertexNormalsMaterial, m_NormalsLength);
        if (m_VBOsetVertexNormals == nullptr) return;
    }

    // préparer le dessin des arêtes
    if (m_DrawEdges) {
        // mettre en place un décalage de polygone
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
    }

    // selon le mode de dessin des triangles
    switch (m_DrawTriangles) {
        case 1:
            // dessiner les triangles
            m_VBOsetTriangles->onDraw(mat4Projection, mat4ModelView);
            break;
        case 2:
            // ne pas dessiner sur l'écran (seulement sur le Depth Buffer)
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            // dessiner les triangles
            m_VBOsetTriangles->onDraw(mat4Projection, mat4ModelView);
            // dessiner à nouveau sur l'écran
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
    }

    if (m_DrawEdges) {
        // supprimer le décalage
        glPolygonOffset(0,0);
        glDisable(GL_POLYGON_OFFSET_FILL);

        // dessiner les arêtes
        glLineWidth(1.2);
        m_VBOsetEdges->onDraw(mat4Projection, mat4ModelView);
    }

    if (m_DrawNormals%2 == 1 && m_VBOsetFaceNormals != nullptr) {
        m_VBOsetFaceNormals->onDraw(mat4Projection, mat4ModelView);
    }

    if (m_DrawNormals >= 2 && m_VBOsetVertexNormals != nullptr) {
        m_VBOsetVertexNormals->onDraw(mat4Projection, mat4ModelView);
    }
}


/** destructeur */
MeshStructureDraw::~MeshStructureDraw()
{
    if (m_EdgesMaterial != nullptr)
        delete m_EdgesMaterial;
    if (m_FaceNormalsMaterial != nullptr)
        delete m_FaceNormalsMaterial;
    if (m_VertexNormalsMaterial != nullptr)
        delete m_VertexNormalsMaterial;

    if (m_VBOsetTriangles != nullptr)
        delete m_VBOsetTriangles;
    if (m_VBOsetEdges != nullptr)
        delete m_VBOsetEdges;
    if (m_VBOsetFaceNormals != nullptr)
        delete m_VBOsetFaceNormals;
    if (m_VBOsetVertexNormals != nullptr)
        delete m_VBOsetVertexNormals;
}

