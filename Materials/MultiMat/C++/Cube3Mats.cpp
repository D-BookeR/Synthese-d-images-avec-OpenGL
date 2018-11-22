// Définition de la classe Cube3Mats


#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <vector>

#include <utils.h>

#include <Cube3Mats.h>
#include <Mesh.h>
#include <MeshModuleDrawing.h>
#include <RedMaterial.h>
#include <GreenMaterial.h>
#include <ProceduralMaterial.h>


std::vector<float> permutation(std::vector<float>& v, int n);

// la fonction sign manque dans la bibliothèque mathématique
template <typename T> int sign(T val)
{
    return (T(0) < val) - (val < T(0));
}


/**
 * Constructeur
 */
Cube3Mats::Cube3Mats()
{
    // matériaux utilisés
    m_RedMaterial = new RedMaterial();
    m_GreenMaterial = new GreenMaterial();
    m_ProceduralMaterial = new ProceduralMaterial();

    // module de dessin
    MeshModuleDrawing renderer;

    // maillage et VBOset pour la gauche et droite
    Mesh* greenMatMesh = new Mesh("Vert");
    addFace(greenMatMesh,  1);
    addFace(greenMatMesh, -1);
    renderer.setMesh(greenMatMesh);
    m_VBOsetVert = renderer.createStripVBOset(m_GreenMaterial, true);

    // maillage et VBOset pour le dessus et le dessous
    Mesh* proceduralMatMesh = new Mesh("Procedural");
    addFace(proceduralMatMesh,  2);
    addFace(proceduralMatMesh, -2);
    renderer.setMesh(proceduralMatMesh);
    m_VBOsetProcedural = renderer.createStripVBOset(m_ProceduralMaterial, true);

    // maillage et VBOset pour l'avant et l'arrière
    Mesh* redMatMesh = new Mesh("Rouge");
    addFace(redMatMesh,  3);
    addFace(redMatMesh, -3);
    renderer.setMesh(redMatMesh);
    m_VBOsetRed = renderer.createStripVBOset(m_RedMaterial, true);

    // suppression des objets temporaire dynamiquement alloués
    delete greenMatMesh;
    delete proceduralMatMesh;
    delete redMatMesh;
}


/**
 * Cette fonction ajoute une face perpendiculaire à l'axe au maillage fourni
 * @param mesh : maillage auquel on rajoute la face
 * @param axis : n° de la coordonnée + 1 qui donne la normale, ex: 2 pour Y, son signe donne le sens
 */
void Cube3Mats::addFace(Mesh* mesh, int axis)
{
    int sign_axis = sign(axis);
    axis = abs(axis) - 1;

    // sommet A
    MeshVertex* A = mesh->addVertex("a");
    std::vector<float> coordsA = {(float)sign_axis, -1, -1};
    coordsA = permutation(coordsA, axis);
    A->setCoord(vec3::fromValues(coordsA[0], coordsA[1], coordsA[2]));
    A->setTexCoord(vec2::fromValues(0,0));

    // sommet B
    MeshVertex* B = mesh->addVertex("b");
    std::vector<float> coordsB = {(float)sign_axis, +1.0f, -1.0f};
    coordsB = permutation(coordsB, axis);
    B->setCoord(vec3::fromValues(coordsB[0], coordsB[1], coordsB[2]));
    B->setTexCoord(vec2::fromValues(1,0));

    // sommet C
    MeshVertex* C = mesh->addVertex("c");
    std::vector<float> coordsC = {(float)sign_axis, +1.0f, +1.0f};
    coordsC = permutation(coordsC, axis);
    C->setCoord(vec3::fromValues(coordsC[0], coordsC[1], coordsC[2]));
    C->setTexCoord(vec2::fromValues(1,1));

    // sommet D
    MeshVertex* D = mesh->addVertex("d");
    std::vector<float> coordsD = {(float)sign_axis, -1.0f, +1.0f};
    coordsD = permutation(coordsD, axis);
    D->setCoord(vec3::fromValues(coordsD[0], coordsD[1], coordsD[2]));
    D->setTexCoord(vec2::fromValues(0,1));

    // ajout d'un quad sur ces 4 sommets
    if (sign_axis > 0) {
        mesh->addQuad(A, B, C, D);
    } else {
        mesh->addQuad(D, C, B, A);
    }
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Cube3Mats::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOsetRed->onDraw(mat4Projection, mat4ModelView);
    m_VBOsetVert->onDraw(mat4Projection, mat4ModelView);
    m_VBOsetProcedural->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Cube3Mats::~Cube3Mats()
{
    delete m_VBOsetRed;
    delete m_VBOsetVert;
    delete m_VBOsetProcedural;

    delete m_RedMaterial;
    delete m_GreenMaterial;
    delete m_ProceduralMaterial;
}


/**
 * Cette fonction fait passer n fois le dernier élément de v tout au début
 * @param v liste à permuter, ex: [ a, b, c, d ]
 * @param n nombre de fois qu'il faut répéter la permutation
 * @return [ d, a, b, c ] si n = 1
 */
std::vector<float> permutation(std::vector<float>& v, int n)
{
    for (int i=0; i<n; i++) {
        float last = v.back();
        v.pop_back();
        v.insert(v.begin(), last);
    }
    return v;
}
