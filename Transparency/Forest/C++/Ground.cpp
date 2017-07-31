// Définition de la classe Ground


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Ground.h>
#include <TextureMaterial.h>


/**
 * Constructeur
 * @param taille du rectangle
 */
Ground::Ground(float taille)
{
    // créer le maillage
    m_Mesh = new Mesh("Ground");

    // sommets
    MeshVertex* P0 = m_Mesh->addVertex("P0");
    P0->setCoord(vec3::fromValues(-taille, 0, -taille));
    P0->setTexCoord(vec2::fromValues(0,taille));
    MeshVertex* P1 = m_Mesh->addVertex("P1");
    P1->setCoord(vec3::fromValues(-taille, 0, +taille));
    P1->setTexCoord(vec2::fromValues(0,0));
    MeshVertex* P2 = m_Mesh->addVertex("P2");
    P2->setCoord(vec3::fromValues(+taille, 0, +taille));
    P2->setTexCoord(vec2::fromValues(taille,0));
    MeshVertex* P3 = m_Mesh->addVertex("P3");
    P3->setCoord(vec3::fromValues(+taille, 0, -taille));
    P3->setTexCoord(vec2::fromValues(taille,taille));

    // quadrilatère
    m_Mesh->addQuad(P0, P1, P2, P3);

    // créer le matériau du rectangle
    m_Texture = new Texture2D("data/textures/sols/ground_texture898.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    m_Material = new TextureMaterial(m_Texture);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
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
    delete m_Material;
    delete m_Texture;
    delete m_VBOset;
    delete m_Mesh;
}
