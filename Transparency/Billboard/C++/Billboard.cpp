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
 */
Billboard::Billboard(Texture360* texture360)
{
    // matrices pour stocker les ModelView
    m_ModelViewTmp  = mat4::create();
    m_ModelViewFixed = mat4::create();

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
    m_Material->select(0.0);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessin du billboard sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Billboard::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
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
    mat4::rotateX(m_ModelViewTmp, mat4ModelView, Utils::radians(-90));
    // réduire la hauteur du billboard
    mat4::scale(m_ModelViewTmp, m_ModelViewTmp, vec3::fromValues(1.0, 0.7, 1.0));
    // dessiner le rectangle
    m_VBOset->onDraw(mat4Projection, m_ModelViewTmp);
    // réactiver le test du depth buffer
    glEnable(GL_DEPTH_TEST);

    /** dessiner le billboard */

    // copier la matrice ModelView avant de la saccager
    mat4::copy(m_ModelViewFixed, mat4ModelView);

    // obtenir l'angle de rotation avec modelview => texture à utiliser selon l'angle
    float angle = atan2(m_ModelViewFixed[2], m_ModelViewFixed[0]);
    // mise de l'angle dans la plage [0, 1[
    angle = angle/(2*M_PI) + 1.0;
    angle = angle - floor(angle);

        /** variante : le billboard est strictement face à la caméra **/

/*
    // modifier la matrice this.m_ModelViewFixed afin qu'il n'y ait plus aucune rotation
    // mais appliquer une mise à l'échelle
    m_ModelViewFixed[ 0] = 1.0;
    m_ModelViewFixed[ 1] = 0.0;
    m_ModelViewFixed[ 2] = 0.0;
    m_ModelViewFixed[ 4] = 0.0;
    m_ModelViewFixed[ 5] = 1.0;
    m_ModelViewFixed[ 6] = 0.0;
    m_ModelViewFixed[ 8] = 0.0;
    m_ModelViewFixed[ 9] = 0.0;
    m_ModelViewFixed[10] = 1.0;
*/

    /** variante : le billboard reste vertical, mais face à la caméra **/

    m_ModelViewFixed[ 0] = 1.0;
    m_ModelViewFixed[ 1] = 0.0;
    m_ModelViewFixed[ 2] = 0.0;
    m_ModelViewFixed[ 8] = 0.0;
    m_ModelViewFixed[ 9] = 0.0;
    m_ModelViewFixed[10] = 1.0;


    // rendre le matériau normal
    m_Material->setCoefficients(1.0, 1.0);
    // activer la bonne texture
    m_Material->select(angle);
    // dessiner le rectangle face à la caméra
    m_VBOset->onDraw(mat4Projection, m_ModelViewFixed);

    // arrêter le blending
    glDisable(GL_BLEND);
}


/**
 * dessin du billboard sur l'écran, variante plus simple
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
