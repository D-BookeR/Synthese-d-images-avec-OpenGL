// Définition de la classe Island


#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleTopology.h>
#include <MeshModuleDrawing.h>

#include <Island.h>



/**
 * Cette fonction définit la classe Island.
 * @param points_count : nombre de points à créer en X et en Z
 * @param scale : facteur de taille du maillage
 * @param height : position verticale du maillage
 * @param depth : position verticale du maillage
 */
Island::Island(int points_count, float scale, float height, float depth)
{
    // test sur la taille du terrain
    // WebGL pose une grosse limite sur les paramètres de gl.drawElements : des ushort uniquement, pas de uint
    if (points_count > 256) {
        std::cout << "Island : nombre de points trop grand, pas plus de 256" << std::endl;
        points_count = 256;
    }

    // paramètres pour créer les sommets
    m_Scale = scale;
    m_Depth = depth;

    // créer le maillage : une grille hexagonale
    m_Mesh = new Mesh("Island");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addHexagonalSurface(points_count, points_count, "terrain %d-%d", false, false);

    // parcourir les sommets pour définir leurs coordonnées
    for (int ix=0; ix<points_count; ix++) {
        for (int iz=0; iz<points_count; iz++) {
            // numéro du vertex
            int num = ix + iz*points_count + num0;
            // récupérer le sommet concerné
            MeshVertex* vertex = m_Mesh->getVertex(num);
            // définir les coordonnées 3D du point
            vertex->setCoord(this->getCoord(points_count, ix, iz));
            vertex->setTexCoord(this->getTexCoord(points_count, ix, iz));
        }
    }

    // créer le matériau de l'objet
    m_Material = new IslandMaterial("data/models/TerrainHM/terrain_hm.png", height, 0.005);

    // créer le VBOset pour dessiner cet objet (attention, peut être très lent)
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
 * @param points_count : nombre de points en X et en Z
 * @param ix : numéro du point en X
 * @param iz : numéro du point en Z
 * @return vec3 contenant les coordonnées
 */
vec3 Island::getCoord(int points_count, int ix, int iz)
{
    // le terrain est légèrement rectangulaire à cause de la maille
    float K = sqrt(3.0)/2.0;

    // décalage en X selon la parité de iz
    float offset_x = (iz%2) * -0.5;

    // coordonnées du sommet
    float x = ((ix+offset_x) / points_count - 0.5) * m_Scale;
    float y = m_Depth;
    float z = ((iz+0.0) / points_count - 0.5) * K * m_Scale;
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode retourne les coordonnées de texture d'un point identifié par (ix, iz)
 * @param points_count : nombre de points en X et en Z
 * @param ix : numéro du point en X
 * @param iz : numéro du point en Z
 * @return vec3 contenant les coordonnées
 */
vec2 Island::getTexCoord(int points_count, int ix, int iz)
{
    // décalage en X selon la parité de iz
    float offset_x = (iz%2) * -0.5;

    float u = (ix+offset_x)/points_count;
    float v = 1.0 - (iz+0.0)/points_count;
    return vec2::fromValues(u, v);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Island::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    // dessiner le maillage
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
 * @param active : true s'il faut compiler un shader gérant le plan de coupe
 * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
 */
void Island::setClipPlane(bool active, vec4 plane)
{
    // fournir le plan de coupe au matériau
    m_Material->setClipPlane(active, plane);
}


/**
 * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
 * @param active : true s'il faut garder le code gérant le plan de coupe dans le shader
 */
void Island::setClipPlane(bool active)
{
    // désactiver le plan de coupe dans le matériau
    m_Material->setClipPlane(active);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Island::~Island()
{
    delete m_VBOset;
    delete m_Mesh;
}
