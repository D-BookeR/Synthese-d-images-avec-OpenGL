// Définition de la classe Rectangle


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleDrawing.h>
#include <ColorMaterial.h>

#include <Rectangle.h>
#include <TextureMaterial.h>



/**
 * Constructeur
 */
Rectangle::Rectangle()
{
    // créer le maillage
    m_Mesh = new Mesh("Rectangle");

    // tableau des coordonnées de texture
    // NB: la première ligne donne les coordonnées des sommets du bas du rectangle
    float texcoords[] = {
        0.0, 0.0,       0.5, 0.0,       1.0, 0.0,
        0.0, 0.5,       0.5, 0.5,       1.0, 0.5,
        0.0, 1.0,       0.5, 1.0,       1.0, 1.0
    };

    // créer les sommets
    int itc = 0;
    for (int z=+1; z>=-1; z--) {
        for (int x=-1; x<=+1; x++) {
            MeshVertex* vertex = m_Mesh->addVertex("");
            vertex->setCoord(vec3::fromValues(x,0,z));
            // affecter les coordonnées de texture de ce sommet
            vec2 texcoord = vec2::fromValues(texcoords[itc+0],texcoords[itc+1]);
            itc += 2;
            vertex->setTexCoord(texcoord);
        }
    }

    // créer les quads (automatiquement transformés en triangles)
    for (int z=0; z<2; z++) {
        for (int x=0; x<2; x++) {
            // récupérer les 4 sommets
            MeshVertex* s00 = m_Mesh->getVertex((x+0)+(z+0)*3);
            MeshVertex* s10 = m_Mesh->getVertex((x+1)+(z+0)*3);
            MeshVertex* s01 = m_Mesh->getVertex((x+0)+(z+1)*3);
            MeshVertex* s11 = m_Mesh->getVertex((x+1)+(z+1)*3);
            m_Mesh->addQuad(s00, s10, s11, s01);
        }
    }

    // créer la texture du rectangle
    m_Texture = new Texture2D("data/textures/earth/earth_map.jpg");

    // créer le matériau du rectangle
    m_Material = new TextureMaterial(m_Texture);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);

    // créer le matériau jaune pour dessiner les arêtes
    m_EdgesMaterial = new ColorMaterial(vec4::fromValues(1,1,0,1));

    // créer le VBOset
    m_VBOsetEdges = renderer.createEdgesVBOset(m_EdgesMaterial);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Rectangle::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // mettre en place un décalage de polygone
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    // dessiner les triangles
    m_VBOset->onDraw(mat4Projection, mat4ModelView);

    // supprimer le décalage
    glPolygonOffset(0, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // dessiner les arêtes
    glLineWidth(1.5);
    m_VBOsetEdges->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Rectangle::~Rectangle()
{
    delete m_EdgesMaterial;
    delete m_Material;
    delete m_Texture;
    delete m_VBOset;
    delete m_Mesh;
}
