// Définition de la classe Ground


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleTopology.h>
#include <MeshModuleDrawing.h>

#include <Ground.h>



/**
 * Cette fonction définit la classe Ground.
 * @param points_count : nombre de points à créer en X et en Z
 * @param materiau : matériau déformant à employer
 */
Ground::Ground(int points_count, GroundMaterial* material)
{
    // test sur la taille du terrain
    // WebGL pose une grosse limite sur les paramètres de gl.drawElements : des ushort uniquement, pas de uint
    if (points_count > 256) {
        std::cout << "Ground : nombre de points trop grand, pas plus de 256" << std::endl;
        points_count = 256;
    }

    // créer le maillage : une grille hexagonale
    m_Mesh = new Mesh("Ground");
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

    // créer le VBOset pour dessiner cet objet (attention, peut être très lent)
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(material, true);
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
 * @param points_count : nombre de points en X et en Z
 * @param ix : numéro du point en X
 * @param iz : numéro du point en Z
 * @return vec3 contenant les coordonnées
 */
vec3 Ground::getCoord(int points_count, int ix, int iz)
{
    // le terrain est légèrement rectangulaire à cause de la maille
    float K = sqrt(3.0)/2.0;

    // décalage en X selon la parité de iz
    float offset_x = (iz%2) * -0.5;

    // coordonnées du sommet
    float x = (ix+offset_x) / points_count - 0.5;
    float y = 0;
    float z = ((iz+0.0) / points_count - 0.5) * K;
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode retourne les coordonnées de texture d'un point identifié par (ix, iz)
 * @param points_count : nombre de points en X et en Z
 * @param ix : numéro du point en X
 * @param iz : numéro du point en Z
 * @return vec3 contenant les coordonnées
 */
vec2 Ground::getTexCoord(int points_count, int ix, int iz)
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
void Ground::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    // dessiner le maillage
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Ground::~Ground()
{
    delete m_VBOset;
    delete m_Mesh;
}
