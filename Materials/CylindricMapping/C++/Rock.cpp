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
 * @param cylindre englobant
 */
Rock::Rock(Texture2D* texture, Cylinder* cylindre)
{
    // créer le matériau
    m_Material = new TextureMaterial(texture);

    // charger le maillage
    m_Mesh = new Mesh("Rock");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Rock1/Rock1.obj", "", 0.5);

    // module de transformation sur le maillage
    MeshModuleProcessing transformation(m_Mesh);

    // décaler tous les sommets
    mat4 translation = mat4::create();
    mat4::translate(translation, translation, vec3::fromValues(0.0, -0.2, 0.0));
    transformation.transform(translation);

    // subdiviser le maillage pour avoir assez de sommets
    m_Mesh->computeNormals();
    transformation.subdivideAll(m_Mesh->getTriangleList(), 2, 0.0);

    // déterminer la hauteur minimale et maximale (idem MeshModuleUtils.creerAABB)
    vec3 vmin = vec3::clone(m_Mesh->getVertex(0)->getCoord());
    vec3 vmax = vec3::clone(m_Mesh->getVertex(0)->getCoord());
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        vec3::min(vmin, vmin, vertex->getCoord());
        vec3::max(vmax, vmax, vertex->getCoord());
    }
    float minY = vmin[1];
    float maxY = vmax[1];

    // recalculer les coordonnées de texture de chaque sommet
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        // coordonnées 3D du sommet
        vec3& coords = vertex->getCoord();

        // transformation en coordonnées cylindriques
        float azimut = atan2(coords[0], coords[2]);  // attention x joue le rôle de y et z celui de x
        float height = coords[1];

        // ramener les coordonnées cylindriques dans l'intervalle [0,1]
        float s = ((azimut/M_PI) + 1.0) / 2.0;
        float t = (height - minY)/(maxY - minY);

        // affectation des coordonnées de texture
        vec2 texcoord = vec2::fromValues(s, t);
        vertex->setTexCoord(texcoord);
    }

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);

    // modifier le cylindre (une fois que le rocher est chargé)
    cylindre->updateMinMaxY(minY, maxY);
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
