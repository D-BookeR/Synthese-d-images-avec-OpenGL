#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleProcessing.h>
#include <MeshModuleDrawing.h>

#include <Rock.h>


/**
 * Constructeur
 * @param texture Texture2D à appliquer sur l'objet
 */
Rock::Rock(Texture2D* texture)
{
    // créer le matériau
    m_Material = new TextureMaterial(texture);

    // charger le maillage
    m_Mesh = new Mesh("Rock");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Rock1/Rock1.obj", "", 0.5);

    // subdiviser le maillage pour avoir assez de sommets
    MeshModuleProcessing transformation(m_Mesh);
    m_Mesh->computeNormals();
    transformation.subdivideAll(m_Mesh->getTriangleList(), 2, 0.0);

    // recalculer les coordonnées de texture de chaque sommet
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        // coordonnées 3D du sommet
        vec3& coords = vertex->getCoord();

        // translation du maillage
        vec3::add(coords, coords, vec3::fromValues(0.0, -0.2, 0.0));

        // transformation en coordonnées sphériques
        float norm = vec3::length(coords);
        float azimut = atan2(coords[0], coords[2]);  // attention x joue le rôle de y et z celui de x
        float height = asin(coords[1] / norm);

        // ramener les coordonnées sphériques dans l'intervalle [0,1]
        float s = ((azimut/M_PI) + 1.0) / 2.0;
        float t = (height/M_PI) + 0.5;

        // affectation des coordonnées de texture
        vec2 texcoord = vec2::fromValues(s, t);
        vertex->setTexCoord(texcoord);
    }

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Rock::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Rock::~Rock()
{
    delete m_Mesh;
    delete m_Material;
    delete m_VBOset;
}
