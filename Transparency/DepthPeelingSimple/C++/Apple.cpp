#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>

#include <Apple.h>


/**
 * Cette fonction définit la classe Apple
 * @param material : matériau à utiliser pour dessiner cette pomme
 */
Apple::Apple(Material* material)
{
    // charger le maillage
    m_Mesh = new Mesh("Apple");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Apple/apple.obj", "", 0.02);

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Apple::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Apple::~Apple()
{
    delete m_Mesh;
    delete m_VBOset;
}
