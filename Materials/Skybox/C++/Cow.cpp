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
 */
Cow::Cow()
{
    // créer un matériau
    vec4 Kd = vec4::fromValues(0.2, 0.8, 0.2, 1.0);
    vec3 Ks = vec3::fromValues(0.9, 0.9, 0.9);
    float Ns = 128;
    m_Material = new DeferredShadingMaterial(Kd, Ks, Ns);

    // créer un maillage
    m_Mesh = new Mesh("Cow");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Cow/Cow.obj", "", 0.05);

    // calculer les normales
    m_Mesh->computeNormals();

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessin de l'objet
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Cow::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Cow::~Cow()
{
    delete m_Mesh;
    delete m_Material;
    delete m_VBOset;
}
