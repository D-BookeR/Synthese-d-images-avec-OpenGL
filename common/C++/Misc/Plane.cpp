/**
 * Cette classe permet de dessiner un mesh en forme de plan
 */


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Plane.h>


/**
 * constructeur
 * @param material : matériau à appliquer sur le plan
 */
Plane::Plane(Material* material)
{
    Mesh mesh("Plane");

    MeshVertex* s1 = mesh.addVertex("s1a");
    MeshVertex* s2 = mesh.addVertex("s2a");
    MeshVertex* s3 = mesh.addVertex("s3a");
    MeshVertex* s4 = mesh.addVertex("s4a");
    mesh.addQuad(s1, s2, s3, s4);
    s1->setCoord(vec3::fromValues(-1,0,+1)); s1->setTexCoord(vec2::fromValues(0.0, 1.0)); s1->setNormal(vec3::fromValues(0,+1,0));
    s2->setCoord(vec3::fromValues(+1,0,+1)); s2->setTexCoord(vec2::fromValues(1.0, 1.0)); s2->setNormal(vec3::fromValues(0,+1,0));
    s3->setCoord(vec3::fromValues(+1,0,-1)); s3->setTexCoord(vec2::fromValues(1.0, 0.0)); s3->setNormal(vec3::fromValues(0,+1,0));
    s4->setCoord(vec3::fromValues(-1,0,-1)); s4->setTexCoord(vec2::fromValues(0.0, 0.0)); s4->setNormal(vec3::fromValues(0,+1,0));

    // calculer les normales
    mesh.computeNormals();

    // préparer le VBOset pour le dessiner
    MeshModuleDrawing renderer(&mesh);
    m_VBOset = renderer.createStripVBOset(material, true);
}


/**
 * dessine l'objet
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
 */
void Plane::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/** destructeur */
Plane::~Plane()
{
    delete m_VBOset;
}
