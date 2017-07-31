#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Billboard.h>


/**
 * crée un billboard : un rectangle portant une texture360
 * @param texture360
 * @param position : vec3 donnant les coordonnées du bas du panneau
 * @param tx : float donnant la taille horizontale
 * @param ty : float donnant la taille verticale
 */
Billboard::Billboard(Texture360* texture360, vec3 position, float tx, float ty)
{
    // paramètres
    m_Position = position;
    m_SizeX = tx;
    m_SizeY = ty;

    // matrices pour stocker les ModelView
    m_ModelViewOrig = mat4::create();
    m_ModelViewFixed = mat4::create();

    // angle sous lequel on voit le billboard
    m_Angle = 0.0;

    // créer le maillage
    m_Mesh = new Mesh("Billboard");

    // sommets
    MeshVertex* P0 = m_Mesh->addVertex("P0");
    P0->setCoord(vec3::fromValues(-0.5, 0.0, 0.0));
    P0->setTexCoord(vec2::fromValues(0.0, 0.0));

    MeshVertex* P1 = m_Mesh->addVertex("P1");
    P1->setCoord(vec3::fromValues(+0.5, 0.0, 0.0));
    P1->setTexCoord(vec2::fromValues(1.0, 0.0));

    MeshVertex* P2 = m_Mesh->addVertex("P2");
    P2->setCoord(vec3::fromValues(+0.5, 1.0, 0.0));
    P2->setTexCoord(vec2::fromValues(1.0, 1.0));

    MeshVertex* P3 = m_Mesh->addVertex("P3");
    P3->setCoord(vec3::fromValues(-0.5, 1.0, 0.0));
    P3->setTexCoord(vec2::fromValues(0.0, 1.0));

    // quadrilatère
    m_Mesh->addQuad(P0, P1, P2, P3);

    // créer le matériau du rectangle
    m_Material = new Texture360Material(texture360);
    m_Material->select(m_Angle);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * recopie ModelView dans les matrices locales
 * @param mat4ModelView
 */
void Billboard::setModelView(mat4& mat4ModelView)
{
    // d'abord placer le billboard à sa place
    mat4::translate(m_ModelViewOrig, mat4ModelView, m_Position);
    mat4::copy(m_ModelViewFixed, m_ModelViewOrig);

    // la distance est dans modelview.translation.z
    m_Distance = m_ModelViewFixed[14];

    // obtenir l'angle de rotation avec modelview => texture à utiliser selon l'angle
    float angle = atan2(m_ModelViewFixed[2], m_ModelViewFixed[0]);
    // mise de l'angle dans la plage [0, 1[
    angle = angle/(2*M_PI) + 1.0;
    m_Angle = angle - floor(angle);

        /** variante : le billboard est strictement face à la caméra **/

/*
    // modifier la matrice this.m_ModelViewFixed afin qu'il n'y ait plus aucune rotation
    // mais appliquer une mise à l'échelle
    m_ModelViewFixed[ 0] = m_SizeX;
    m_ModelViewFixed[ 1] = 0.0;
    m_ModelViewFixed[ 2] = 0.0;
    m_ModelViewFixed[ 4] = 0.0;
    m_ModelViewFixed[ 5] = m_SizeY;
    m_ModelViewFixed[ 6] = 0.0;
    m_ModelViewFixed[ 8] = 0.0;
    m_ModelViewFixed[ 9] = 0.0;
    m_ModelViewFixed[10] = 1.0;
*/

    /** variante : le billboard reste vertical, mais face à la caméra **/

    m_ModelViewFixed[ 0] = m_SizeX;
    m_ModelViewFixed[ 1] = 0.0;
    m_ModelViewFixed[ 2] = 0.0;
    m_ModelViewFixed[ 5] *= m_SizeY;
    m_ModelViewFixed[ 8] = 0.0;
    m_ModelViewFixed[ 9] = 0.0;
    m_ModelViewFixed[10] = 1.0;
}


/**
 * retourne la distance du billboard à l'œil
 * @return distance
 */
float Billboard::getDistance() const
{
    return m_Distance;
}


/**
 * compare la distance entre deux billboard afin de pouvoir classer une liste
 * du plus loin au plus proche
 * @param a : billboard à comparer
 * @param b : billboard à comparer
 * @return la valeur qu'il faut pour le tri
 */
bool Billboard::DistanceCompare(const Billboard* a, const Billboard* b)
{
    return a->m_Distance < b->m_Distance;
}


/**
 * dessin du billboard sur l'écran
 * @param mat4Projection : matrice de projection
 */
void Billboard::onDraw(mat4& mat4Projection)
{
    // mettre en place le blending
    glEnable(GL_BLEND);

    /** dessiner l'ombre */

    // rendre le matériau noir et un peu transparent
    m_Material->setCoefficients(0.0, 0.7);
    // activer la texture d'angle 0 pour l'ombre (soleil tjrs même position)
    m_Material->select(0.0);
    // désactiver le test du depth buffer
    glDisable(GL_DEPTH_TEST);
    // faire pivoter le billboard à plat par terre
    mat4 mat4ModelView = mat4::create();
    mat4::rotateX(mat4ModelView, m_ModelViewOrig, Utils::radians(-90));
    // réduire la hauteur du billboard
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(m_SizeX, 0.7*m_SizeY, 1.0));
    // dessiner le rectangle
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
    // réactiver le test du depth buffer
    glEnable(GL_DEPTH_TEST);

    /** dessiner le billboard */

    // rendre le matériau normal
    m_Material->setCoefficients(1.0, 1.0);
    // activer la bonne texture
    m_Material->select(m_Angle);
    // dessiner le rectangle face à la caméra
    m_VBOset->onDraw(mat4Projection, m_ModelViewFixed);

    // arrêter le blending
    glDisable(GL_BLEND);
}


/**
 * dessin du billboard sur l'écran, variante plus simple, sans positionnement préalable
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Billboard::onDraw2(mat4& mat4Projection, mat4& mat4ModelView)
{
    mat4 mat4ModelViewFixed = mat4::clone(mat4ModelView);
    mat4ModelViewFixed[ 0] = 1.0;
    mat4ModelViewFixed[ 1] = 0.0;
    mat4ModelViewFixed[ 2] = 0.0;
    mat4ModelViewFixed[ 4] = 0.0;
    mat4ModelViewFixed[ 5] = 1.0;
    mat4ModelViewFixed[ 6] = 0.0;
    mat4ModelViewFixed[ 8] = 0.0;
    mat4ModelViewFixed[ 9] = 0.0;
    mat4ModelViewFixed[10] = 1.0;

    // mettre en place le blending
    glEnable(GL_BLEND);

    // dessiner le rectangle face à la caméra
    m_VBOset->onDraw(mat4Projection, mat4ModelViewFixed);

    // arrêter le blending
    glDisable(GL_BLEND);
 }

/**
 * Cette méthode supprime les ressources allouées
 */
Billboard::~Billboard()
{
    delete m_Material;
    delete m_VBOset;
    delete m_Mesh;
}
