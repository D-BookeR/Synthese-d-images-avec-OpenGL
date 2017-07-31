// Définition de la classe Billboard

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Billboard.h>


/**
 * crée un billboard : un rectangle portant une texture transparente
 * @param material : matériau à appliquer
 * @param width : largeur du panneau
 * @param height : hauteur du panneau
 */
Billboard::Billboard(Material* material, float width, float height)
{
    // matrices pour stocker les ModelView
    m_ModelViewFixed = mat4::create();

    // créer le maillage
    m_Mesh = new Mesh("Billboard");

    // sommets
    MeshVertex* P0 = m_Mesh->addVertex("P0");
    P0->setCoord(vec3::fromValues(-width*0.5, 0.0, 0.0));
    P0->setTexCoord(vec2::fromValues(0.01, 0.01));

    MeshVertex* P1 = m_Mesh->addVertex("P1");
    P1->setCoord(vec3::fromValues(+width*0.5, 0.0, 0.0));
    P1->setTexCoord(vec2::fromValues(0.99, 0.01));

    MeshVertex* P2 = m_Mesh->addVertex("P2");
    P2->setCoord(vec3::fromValues(+width*0.5, height, 0.0));
    P2->setTexCoord(vec2::fromValues(0.99, 0.99));

    MeshVertex* P3 = m_Mesh->addVertex("P3");
    P3->setCoord(vec3::fromValues(-width*0.5, height, 0.0));
    P3->setTexCoord(vec2::fromValues(0.01, 0.99));

    // quadrilatère
    m_Mesh->addQuad(P0, P1, P2, P3);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(material, true);
}


/**
 * dessin du billboard sur l'écran, variante simple
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Billboard::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // modifier la matrice ModelView
    mat4::copy(m_ModelViewFixed, mat4ModelView);
    m_ModelViewFixed[ 0] = 1.0;
    m_ModelViewFixed[ 1] = 0.0;
    m_ModelViewFixed[ 2] = 0.0;
    m_ModelViewFixed[ 4] = 0.0;
    m_ModelViewFixed[ 5] = 1.0;
    m_ModelViewFixed[ 6] = 0.0;
    m_ModelViewFixed[ 8] = 0.0;
    m_ModelViewFixed[ 9] = 0.0;
    m_ModelViewFixed[10] = 1.0;

    // mettre en place le blending
    glEnable(GL_BLEND);

    // dessiner le rectangle face à la caméra
    m_VBOset->onDraw(mat4Projection, m_ModelViewFixed);

    // arrêter le blending
    glDisable(GL_BLEND);
 }


/**
 * Cette méthode supprime les ressources allouées
 */
Billboard::~Billboard()
{
    delete m_VBOset;
    delete m_Mesh;
}
