#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <SquaredPyramid.h>


/**
 * Constructeur
 * @param material : matériau à appliquer
 */
SquaredPyramid::SquaredPyramid(Material* material)
{
    // créer le maillage
    m_Mesh = new Mesh("SquaredPyramid");

    // créer les 5 sommets
    MeshVertex* s1 = m_Mesh->addVertex("s1");
    s1->setCoord(vec3::fromValues(-1,-1,-1));
    s1->setColor( vec3::fromValues(1.0, 0.0, 0.0));

    MeshVertex* s2 = m_Mesh->addVertex("s2");
    s2->setCoord(vec3::fromValues(+1,-1,-1));
    s2->setColor( vec3::fromValues(0.8, 1.0, 0.0));

    MeshVertex* s3 = m_Mesh->addVertex("s3");
    s3->setCoord(vec3::fromValues(+1,-1,+1));
    s3->setColor( vec3::fromValues(0.0, 1.0, 0.4));

    MeshVertex* s4 = m_Mesh->addVertex("s4");
    s4->setCoord(vec3::fromValues(-1,-1,+1));
    s4->setColor( vec3::fromValues(0.0, 0.4, 1.0));

    MeshVertex* s5 = m_Mesh->addVertex("s5");
    s5->setCoord(vec3::fromValues( 0,+1, 0));
    s5->setColor( vec3::fromValues(0.8, 0.0, 1.0));

    // créer le fond
    m_Mesh->addTriangle(s1,s2,s3);
    m_Mesh->addTriangle(s1,s3,s4);

    // créer la face devant
    m_Mesh->addTriangle(s4,s3,s5);

    // créer la face droite
    m_Mesh->addTriangle(s3,s2,s5);

    // créer la face arrière
    m_Mesh->addTriangle(s2,s1,s5);

    // créer la face gauche
    m_Mesh->addTriangle(s1,s4,s5);

    // créer le VBO
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createVBOset(material);
}


/**
 * dessine la pyramide
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void SquaredPyramid::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
SquaredPyramid::~SquaredPyramid()
{
    delete m_VBOset;
    delete m_Mesh;
}
