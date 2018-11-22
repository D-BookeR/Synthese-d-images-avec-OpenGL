#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Ground.h>


/**
 * Constructeur
 * @param depth : profondeur à affecter à la coordonnée y des sommets
 */
Ground::Ground(float depth)
{
    // créer le maillage du miroir
    m_Mesh = new Mesh("fond");

    // sommets
    MeshVertex* P0m = m_Mesh->addVertex("P0");
    P0m->setCoord(vec3::fromValues(-999, depth, +999));
    P0m->setTexCoord(vec2::fromValues(-999, +999));
    P0m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));
    MeshVertex* P1m = m_Mesh->addVertex("P1");
    P1m->setCoord(vec3::fromValues(+999, depth, +999));
    P1m->setTexCoord(vec2::fromValues(+999, +999));
    P1m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));
    MeshVertex* P2m = m_Mesh->addVertex("P2");
    P2m->setCoord(vec3::fromValues(+999, depth, -999));
    P2m->setTexCoord(vec2::fromValues(+999, -999));
    P2m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));
    MeshVertex* P3m = m_Mesh->addVertex("P3");
    P3m->setCoord(vec3::fromValues(-999, depth, -999));
    P3m->setTexCoord(vec2::fromValues(-999, -999));
    P3m->setNormal(vec3::fromValues(0.0, 1.0, 0.0));

    // quadrilatère
    m_Mesh->addQuad(P0m, P1m, P2m, P3m);

    // créer le matériau du rectangle
    m_Texture = new Texture2D("data/textures/sols/79798.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    m_Material = new TextureMaterial(m_Texture);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine le fond
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Ground::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Ground::~Ground()
{
    delete m_VBOset;
    delete m_Material;
    delete m_Texture;
    delete m_Mesh;
}
