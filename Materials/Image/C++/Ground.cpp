#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>

#include <Ground.h>


/**
 * Constructeur
 */
Ground::Ground()
{
    // créer le matériau du terrain
    m_Texture = new Texture2D("data/models/Terrain/TerrainAt.png");
    m_Material = new TextureMaterial(m_Texture);

    // charger le maillage
    m_Mesh = new Mesh("Ground");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Terrain/Terrain.obj", "", 1.0);

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Ground::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Ground::~Ground()
{
    delete m_Mesh;
    delete m_Material;
    delete m_Texture;
    delete m_VBOset;
}
