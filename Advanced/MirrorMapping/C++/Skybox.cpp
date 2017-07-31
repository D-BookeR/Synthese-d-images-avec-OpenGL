// Définition de la classe Skybox


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Skybox.h>
#include <SkyboxMaterial.h>



/**
 * Cette fonction définit la classe Skybox = maillage recouvert par une texture.
 * @param texture : instance de TextureCube
 */
Skybox::Skybox(TextureCube* texture)
{
    // créer le maillage du cube
    m_Mesh = new Mesh("Skybox");

    // rajouter les sommets
    int i = 0;
    for (int z=-1; z<=1; z+=2) {
        for (int y=-1; y<=1; y+=2) {
            for (int x=-1; x<=1; x+=2) {
                MeshVertex* v = m_Mesh->addVertex("");
                i++;
                v->setCoord(vec3::fromValues(x,y,z));
            }
        }
    }

    // rajouter les 6 faces vues de l'intérieur
    int faces[6][4] = { {3,2,0,1}, {6,7,5,4}, {4,5,1,0}, {7,6,2,3}, {5,7,3,1}, {0,2,6,4} };
    for (int i=0; i<6; i++) {
        int* face = faces[i];
        m_Mesh->addQuad(
            m_Mesh->getVertex(face[0]),
            m_Mesh->getVertex(face[1]),
            m_Mesh->getVertex(face[2]),
            m_Mesh->getVertex(face[3]));
    }

    // créer le matériau du cube
    m_Material = new SkyboxMaterial(texture);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Skybox::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // désactiver depth test et modification du depth buffer
    //glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // dessiner le cube avec son matériau
    m_VBOset->onDraw(mat4Projection, mat4ModelView);

    // réactiver le depth buffer
    //glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Skybox::~Skybox()
{
    delete m_Material;
    delete m_VBOset;
    delete m_Mesh;
}
