// Définition de la classe Rectangle3


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Rectangle3.h>



/**
 * Constructeur
 */
Rectangle3::Rectangle3()
{
    // créer le maillage
    m_Mesh = new Mesh("Rectangle3");

    // sommets
    MeshVertex* C = m_Mesh->addVertex("C");
    C->setCoord(vec3::fromValues(0.0, -1.0, 0.0));
    C->setTexCoord(vec2::fromValues(0.75, 0.0));

    MeshVertex* D = m_Mesh->addVertex("D");
    D->setCoord(vec3::fromValues(+1.0, -1.0, 0.0));
    D->setTexCoord(vec2::fromValues(1.0, 0.0));

    MeshVertex* E = m_Mesh->addVertex("E");
    E->setCoord(vec3::fromValues(+1.0, +1.0, 0.0));
    E->setTexCoord(vec2::fromValues(1.0, 1.0));

    MeshVertex* F = m_Mesh->addVertex("F");
    F->setCoord(vec3::fromValues(0.0, +1.0, 0.0));
    F->setTexCoord(vec2::fromValues(0.75, 1.0));

    // quadrilatère
    m_Mesh->addQuad(C, D, E, F);

    // créer le matériau du rectangle
    m_Texture = new Texture2D("data/textures/1024px-LandscapeArchPano.jpg", GL_LINEAR, GL_REPEAT);
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
void Rectangle3::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Rectangle3::~Rectangle3()
{
    delete m_Material;
    delete m_VBOset;
    delete m_Mesh;
}
