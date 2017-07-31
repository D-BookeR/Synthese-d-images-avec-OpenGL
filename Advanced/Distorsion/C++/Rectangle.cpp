// Définition de la classe Rectangle


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Rectangle.h>
#include <TwistMaterial.h>



/**
 * Constructeur
 */
Rectangle::Rectangle()
{
    // créer le maillage
    m_Mesh = new Mesh("Rectangle");

    // sommets
    MeshVertex* P0 = m_Mesh->addVertex("P0");
    P0->setCoord(vec3::fromValues(-1.33, -1, 0));
    P0->setTexCoord(vec2::fromValues(0,0));
    MeshVertex* P1 = m_Mesh->addVertex("P1");
    P1->setCoord(vec3::fromValues(+1.33, -1, 0));
    P1->setTexCoord(vec2::fromValues(1,0));
    MeshVertex* P2 = m_Mesh->addVertex("P2");
    P2->setCoord(vec3::fromValues(+1.33, +1, 0));
    P2->setTexCoord(vec2::fromValues(1,1));
    MeshVertex* P3 = m_Mesh->addVertex("P3");
    P3->setCoord(vec3::fromValues(-1.33, +1, 0));
    P3->setTexCoord(vec2::fromValues(0,1));

    // quadrilatère
    m_Mesh->addQuad(P0, P1, P2, P3);

    // créer le matériau du rectangle
    m_Material = new TwistMaterial();
    m_Material->setMaxAngle(-90.0);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Rectangle::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_Material->setMaxAngle(cos(Utils::Time) * 180.0);

    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Rectangle::~Rectangle()
{
    delete m_Material;
    delete m_VBOset;
    delete m_Mesh;
}
