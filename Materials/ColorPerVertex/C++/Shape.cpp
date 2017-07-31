// Définition de la classe Shape


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Shape.h>
#include <ColorPerVertexMaterial.h>



/**
 * Constructeur
 */
Shape::Shape()
{
    // créer le maillage
    m_Mesh = new Mesh("Shape");

    // sommet 0
    MeshVertex* s0 = m_Mesh->addVertex("s0");
    s0->setCoord(vec3::fromValues(-1, 0, +1));
    s0->setColor(vec3::fromValues(0.86,0.13,0.99));

    // sommet 1
    MeshVertex* s1 = m_Mesh->addVertex("s1");
    s1->setCoord(vec3::fromValues( 0, 0, +1));
    s1->setColor(vec3::fromValues(0.76,0.32,0.50));

    // sommet 2
    MeshVertex* s2 = m_Mesh->addVertex("s2");
    s2->setCoord(vec3::fromValues(+1, 0, +1));
    s2->setColor(vec3::fromValues(0.12,0.99,0.87));

    // sommet 3
    MeshVertex* s3 = m_Mesh->addVertex("s3");
    s3->setCoord(vec3::fromValues(-1, 0,  0));
    s3->setColor(vec3::fromValues(0.79,0.30,0.21));

    // sommet 4
    MeshVertex* s4 = m_Mesh->addVertex("s4");
    s4->setCoord(vec3::fromValues( 0, 0,  0));
    s4->setColor(vec3::fromValues(0.86,0.49,0.17));

    // sommet 5
    MeshVertex* s5 = m_Mesh->addVertex("s5");
    s5->setCoord(vec3::fromValues(+1, 0,  0));
    s5->setColor(vec3::fromValues(0.12,0.80,0.19));

    // sommet 6
    MeshVertex* s6 = m_Mesh->addVertex("s6");
    s6->setCoord(vec3::fromValues(-1, 0, -1));
    s6->setColor(vec3::fromValues(0.28,0.70,0.86));

    // sommet 7
    MeshVertex* s7 = m_Mesh->addVertex("s7");
    s7->setCoord(vec3::fromValues( 0, 0, -1));
    s7->setColor(vec3::fromValues(0.00,0.29,0.22));

    // sommet 8
    MeshVertex* s8 = m_Mesh->addVertex("s8");
    s8->setCoord(vec3::fromValues(+1, 0, -1));
    s8->setColor(vec3::fromValues(0.42,0.26,0.83));

    // ajouter les quads
    m_Mesh->addQuad(s0, s1, s4, s3);
    m_Mesh->addQuad(s1, s2, s5, s4);
    m_Mesh->addQuad(s3, s4, s7, s6);
    m_Mesh->addQuad(s4, s5, s8, s7);

    // créer le matériau du rectangle
    m_Material = new ColorPerVertexMaterial();

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Shape::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Shape::~Shape()
{
    delete m_Material;
    delete m_VBOset;
    delete m_Mesh;
}
