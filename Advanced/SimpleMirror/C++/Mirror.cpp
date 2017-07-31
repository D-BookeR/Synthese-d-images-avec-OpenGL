#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Mirror.h>


/**
 * Constructeur
 */
Mirror::Mirror()
{
    // créer le maillage du miroir
    m_Mesh = new Mesh("miroir");

    // sommets
    MeshVertex* P0m = m_Mesh->addVertex("P0");
    P0m->setCoord(vec3::fromValues(-2.0, 0, 0));
    MeshVertex* P1m = m_Mesh->addVertex("P1");
    P1m->setCoord(vec3::fromValues(+2.0, 0, 0));
    MeshVertex* P2m = m_Mesh->addVertex("P2");
    P2m->setCoord(vec3::fromValues(+2.0, +2.5, 0));
    MeshVertex* P3m = m_Mesh->addVertex("P3");
    P3m->setCoord(vec3::fromValues(-2.0, +2.5, 0));

    // quadrilatère
    m_Mesh->addQuad(P0m, P1m, P2m, P3m);

    // créer le matériau du rectangle
    m_Material = new ColorMaterial(vec4::fromValues(0.1, 0.1, 0.2, 1.0));

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine la surface du miroir
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Mirror::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Mirror::~Mirror()
{
    delete m_VBOset;
    delete m_Material;
    delete m_Mesh;
}
