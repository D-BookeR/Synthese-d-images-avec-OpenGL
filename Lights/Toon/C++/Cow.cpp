#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>

#include <Cow.h>


/**
 * Constructeur
 * @param material : matériau à appliquer
 */
Cow::Cow(ToonMaterial* material)
{
    // créer le matériau pour les arêtes
    vec4 black = vec4::fromValues(0.0, 0.0, 0.0, 1.0);
    m_EdgesMaterial = new ColorMaterial(black);

    // charger le maillage
    m_Mesh = new Mesh("Cow");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Cow/Cow.obj", "", 0.15);

    // calculer les normales
    m_Mesh->computeNormals();

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(material, true);

    // créer le VBOset des aretes
    m_VBOsetEdges = renderer.createEdgesVBOset(m_EdgesMaterial);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Cow::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner l'objet
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Fonction de dessin des arêtes de l'objet
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Cow::onDrawEdges(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner l'objet
    m_VBOsetEdges->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Cow::~Cow()
{
    delete m_Mesh;
    delete m_VBOset;
    delete m_VBOsetEdges;
    delete m_EdgesMaterial;
}
