#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>

#include <Teapot.h>


/**
 * Cette fonction définit la classe Teapot = maillage recouvert par une texture.
 * @param texture : instance de TextureCube
 */
Teapot::Teapot(TextureCube* texture)
{
    // créer le matériau de la théière
    m_Material = new MirrorMaterial(texture);

    // charger le maillage
    m_Mesh = new Mesh("Teapot");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Teapot/teapot.obj", "", 0.005);
    //loader.loadObjFile("data/models/Sphere/sphere.obj", "", 0.3);
    //loader.loadObjFile("data/models/Cow/Cow.obj", "", 0.05);
    //loader.loadObjFile("data/models/Apple/apple.obj", "", 0.008);

    // calculer les normales
    m_Mesh->computeNormals();

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelViewScene : matrice de vue
 */
void Teapot::onDraw(mat4& mat4Projection, mat4& mat4ModelViewScene)
{

    // fournir la matrice mat4ModelViewScene au matériau
    m_Material->setModelViewScene(mat4ModelViewScene);

    // matrice ModelView pour afficher la théière
    mat4 mat4ModelViewTeapot = mat4::create();

    // appliquer une rotation en fonction du temps
    mat4::rotateX(mat4ModelViewTeapot, mat4ModelViewScene, Utils::radians(Utils::Time * 15.0));
    mat4::rotateY(mat4ModelViewTeapot, mat4ModelViewTeapot, Utils::radians(Utils::Time * 32.0));

    // décaler la théière vers le bas
    mat4::translate(mat4ModelViewTeapot, mat4ModelViewTeapot, vec3::fromValues(0.0,-0.2,0.0));

    // dessiner l'objet
    m_VBOset->onDraw(mat4Projection, mat4ModelViewTeapot);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Teapot::~Teapot()
{
    delete m_Mesh;
    delete m_Material;
    delete m_VBOset;
}
