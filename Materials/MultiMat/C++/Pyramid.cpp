// Définition de la classe Pyramid


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <vector>

#include <utils.h>

#include <Pyramid.h>
#include <Mesh.h>
#include <MeshModuleDrawing.h>
#include <RedMaterial.h>
#include <GreenMaterial.h>
#include <ProceduralMaterial.h>
#include <ColorPerVertexMaterial.h>



/**
 * Constructeur
 */
Pyramid::Pyramid()
{
    // matériaux utilisés
    m_ColorMaterial = new ColorPerVertexMaterial();
    m_RedMaterial = new RedMaterial();
    m_GreenMaterial = new GreenMaterial();
    m_ProceduralMaterial = new ProceduralMaterial();

    // module de dessin
    MeshModuleDrawing renderer;

    // coordonnées des sommets
    vec3 coordsA = vec3::fromValues( 0.0, 1.5, 0.0);
    vec3 coordsB = vec3::fromValues( 1.0,-1.0, 1.5);
    vec3 coordsC = vec3::fromValues( 1.0,-1.0,-1.5);
    vec3 coordsD = vec3::fromValues(-1.0,-1.0, 0.0);

    // couleurs des sommets (pour le matériau ColorPerVertexMaterial)
    vec3 colorA = vec3::fromValues(0.0, 1.0, 1.0);
    vec3 colorB = vec3::fromValues(0.0, 0.0, 1.0);
    vec3 colorC = vec3::fromValues(1.0, 0.0, 1.0);
    vec3 colorD = vec3::fromValues(1.0, 0.0, 0.0);

    // coordonnées de texture (pour le matériau ProceduralMaterial)
    vec2 texcoords00 = vec2::fromValues(0.0, 0.0);
    vec2 texcoords10 = vec2::fromValues(1.0, 0.0);
    vec2 texcoords11 = vec2::fromValues(0.5, 1.0);

    MeshVertex* A;
    MeshVertex *B;
    MeshVertex *C;
    MeshVertex *D;

    Mesh* colorMatMesh = new Mesh("CouleurVertex");
    A = colorMatMesh->addVertex("a")->setCoord(coordsA)->setColor(colorA);
    B = colorMatMesh->addVertex("b")->setCoord(coordsB)->setColor(colorB);
    C = colorMatMesh->addVertex("c")->setCoord(coordsC)->setColor(colorC);
    colorMatMesh->addTriangle(A, B, C);
    renderer.setMesh(colorMatMesh);
    m_VBOsets.push_back(renderer.createStripVBOset(m_ColorMaterial, true));

    Mesh* redMatMesh = new Mesh("Rouge");
    D = redMatMesh->addVertex("d")->setCoord(coordsD);
    C = redMatMesh->addVertex("c")->setCoord(coordsC);
    B = redMatMesh->addVertex("b")->setCoord(coordsB);
    redMatMesh->addTriangle(D, C, B);
    renderer.setMesh(redMatMesh);
    m_VBOsets.push_back(renderer.createStripVBOset(m_RedMaterial, true));

    Mesh* greenMatMesh = new Mesh("Vert");
    D = greenMatMesh->addVertex("d")->setCoord(coordsD);
    B = greenMatMesh->addVertex("b")->setCoord(coordsB);
    A = greenMatMesh->addVertex("a")->setCoord(coordsA);
    greenMatMesh->addTriangle(D, B, A);
    renderer.setMesh(greenMatMesh);
    m_VBOsets.push_back(renderer.createStripVBOset(m_GreenMaterial, true));

    Mesh* proceduralMatMesh = new Mesh("Procedural");
    D = proceduralMatMesh->addVertex("d")->setCoord(coordsD)->setTexCoord(texcoords00);
    A = proceduralMatMesh->addVertex("a")->setCoord(coordsA)->setTexCoord(texcoords10);
    C = proceduralMatMesh->addVertex("c")->setCoord(coordsC)->setTexCoord(texcoords11);
    proceduralMatMesh->addTriangle(D, A, C);
    renderer.setMesh(proceduralMatMesh);
    m_VBOsets.push_back(renderer.createStripVBOset(m_ProceduralMaterial, true));

    // suppression des objets temporaires dynamiquement alloués
    delete colorMatMesh;
    delete redMatMesh;
    delete greenMatMesh;
    delete proceduralMatMesh;
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Pyramid::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    for (VBOset* vboset: m_VBOsets) {
        vboset->onDraw(mat4Projection, mat4ModelView);
    }
}


/**
 * Cette méthode supprime les ressources allouées
 */
Pyramid::~Pyramid()
{
    for (VBOset* vboset: m_VBOsets) {
        delete vboset;
    }
    delete m_ProceduralMaterial;
    delete m_GreenMaterial;
    delete m_RedMaterial;
    delete m_ColorMaterial;
}
