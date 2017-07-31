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
    // module pour créer les VBOsets
    MeshModuleDrawing renderer;

    /// Partie miroir

    // créer le maillage du miroir
    m_MirrorMesh = new Mesh("miroir");

    // sommets
    MeshVertex* P0m = m_MirrorMesh->addVertex("P0");
    P0m->setCoord(vec3::fromValues(-1.0, 0, 0));
    MeshVertex* P1m = m_MirrorMesh->addVertex("P1");
    P1m->setCoord(vec3::fromValues(+1.0, 0, 0));
    MeshVertex* P2m = m_MirrorMesh->addVertex("P2");
    P2m->setCoord(vec3::fromValues(+1.0, +2.5, 0));
    MeshVertex* P3m = m_MirrorMesh->addVertex("P3");
    P3m->setCoord(vec3::fromValues(-1.0, +2.5, 0));

    // quadrilatère
    m_MirrorMesh->addQuad(P0m, P1m, P2m, P3m);

    // créer le matériau du rectangle
    m_MirrorMaterial = new ColorMaterial(vec4::fromValues(0.1, 0.1, 0.2, 1.0));

    // créer les VBO pour afficher ce maillage
    renderer.setMesh(m_MirrorMesh);
    m_VBOsetMirror = renderer.createStripVBOset(m_MirrorMaterial, true);

    /// Partie Cadre

    // créer le maillage du cadre
    m_FrameMesh = new Mesh("cadre");

    // coordonnées des sommets
    vec3 P0c = vec3::fromValues(-1.0, 0, -0.01);
    vec3 P1c = vec3::fromValues(+1.0, 0, -0.01);
    vec3 P2c = vec3::fromValues(+1.0, +2.5, -0.01);
    vec3 P3c = vec3::fromValues(-1.0, +2.5, -0.01);
    vec3 P4c = vec3::fromValues(-1.0, 0, -0.2);
    vec3 P5c = vec3::fromValues(+1.0, 0, -0.2);
    vec3 P6c = vec3::fromValues(+1.0, +2.5, -0.2);
    vec3 P7c = vec3::fromValues(-1.0, +2.5, -0.2);

    // sorte de cube
    addFrameQuad("droit",  vec3::fromValues(+1,0,0), P1c, P5c, P6c, P2c);
    addFrameQuad("haut",   vec3::fromValues(0,+1,0), P2c, P6c, P7c, P3c);
    addFrameQuad("gauche", vec3::fromValues(-1,0,0), P3c, P7c, P4c, P0c);
    addFrameQuad("bas",    vec3::fromValues(0,-1,0), P0c, P4c, P5c, P1c);
    addFrameQuad("avant",  vec3::fromValues(0,0,-1), P5c, P4c, P7c, P6c);
    addFrameQuad("dos",    vec3::fromValues(0,0,+1), P0c, P1c, P2c, P3c);

    // créer le matériau du cadre
    m_FrameMaterial = new ColorMaterial(vec4::fromValues(0.6, 0.4, 0.2, 1.0));

    // créer les VBO pour afficher ce maillage
    renderer.setMesh(m_FrameMesh);
    m_VBOsetFrame = renderer.createStripVBOset(m_FrameMaterial, true);
}


/**
 * ajout d'une face (a,b,c,d) au maillage du cadre
 * @param name : nom de la face à créer
 * @param normal : vec3 à affecter aux 4 sommets
 * @param a : coordonnées
 * @param b : coordonnées
 * @param c : coordonnées
 * @param d : coordonnées
 */
void Mirror::addFrameQuad(std::string name, vec3 normal, vec3& a, vec3& b, vec3& c, vec3& d)
{
    MeshVertex* A = m_FrameMesh->addVertex(name+"A");
    A->setCoord(a);
    A->setNormal(normal);

    MeshVertex* B = m_FrameMesh->addVertex(name+"B");
    B->setCoord(b);
    B->setNormal(normal);

    MeshVertex* C = m_FrameMesh->addVertex(name+"C");
    C->setCoord(c);
    C->setNormal(normal);

    MeshVertex* D = m_FrameMesh->addVertex(name+"D");
    D->setCoord(d);
    D->setNormal(normal);

    m_FrameMesh->addQuad(A, B, C, D);
}


/**
 * dessine le cadre du miroir
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Mirror::onDrawFrame(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOsetFrame->onDraw(mat4Projection, mat4ModelView);
}


/**
 * dessine la surface du miroir
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Mirror::onDrawSurface(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOsetMirror->onDraw(mat4Projection, mat4ModelView);
}


/**
 * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
 * @param active : true s'il faut compiler un shader gérant le plan de coupe
 * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
 */
void Mirror::setClipPlane(bool active, vec4 plane)
{
    // fournir le plan de coupe aux matériaux
    m_MirrorMaterial->setClipPlane(active, plane);
    m_FrameMaterial->setClipPlane(active, plane);
}


/**
 * supprime un plan de coupe pour les fragments.
 * @param active : true s'il faut garder le shader gérant le plan de coupe
 */
void Mirror::setClipPlane(bool active)
{
    // fournir le plan de coupe aux matériaux
    m_MirrorMaterial->setClipPlane(active);
    m_FrameMaterial->setClipPlane(active);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Mirror::~Mirror()
{
    delete m_VBOsetFrame;
    delete m_FrameMaterial;
    delete m_FrameMesh;
    delete m_VBOsetMirror;
    delete m_MirrorMaterial;
    delete m_MirrorMesh;
}
