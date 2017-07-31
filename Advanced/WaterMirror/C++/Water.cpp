#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Water.h>


/**
 * Constructeur
 * @param material : matériau pour l'eau
 */
Water::Water(Material* material)
{
    // créer le maillage du miroir
    m_Mesh = new Mesh("eau");

    // sommets
    MeshVertex* P0m = m_Mesh->addVertex("P0");
    P0m->setCoord(vec3::fromValues(-999, 0, +999));
    P0m->setTexCoord(vec2::fromValues(-999, +999));
    P0m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));
    MeshVertex* P1m = m_Mesh->addVertex("P1");
    P1m->setCoord(vec3::fromValues(+999, 0, +999));
    P1m->setTexCoord(vec2::fromValues(+999, +999));
    P1m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));
    MeshVertex* P2m = m_Mesh->addVertex("P2");
    P2m->setCoord(vec3::fromValues(+999, 0, -999));
    P2m->setTexCoord(vec2::fromValues(+999, -999));
    P2m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));
    MeshVertex* P3m = m_Mesh->addVertex("P3");
    P3m->setCoord(vec3::fromValues(-999, 0, -999));
    P3m->setTexCoord(vec2::fromValues(-999, -999));
    P3m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));

    // quadrilatère
    m_Mesh->addQuad(P0m, P1m, P2m, P3m);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(material, true);
}


/**
 * dessine la surface de l'eau
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Water::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Water::~Water()
{
    delete m_VBOset;
    delete m_Mesh;
}
