#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleProcessing.h>
#include <MeshModuleDrawing.h>

#include <CowEdges.h>
#include <DeferredShadingMaterial.h>
#include <ColorMaterial.h>


CowEdges::CowEdges()
{
    // créer le matériau pour les triangles
    vec4 Kd = vec4::fromValues(0.9, 0.9, 0.9, 1.0);
    vec3 Ks = vec3::fromValues(0.1, 0.1, 0.1);
    float Ns = 64;
    m_TrianglesMaterial = new DeferredShadingMaterial(Kd, Ks, Ns);
    // remarque : il n'y a pas de lampe dans la scène, alors le matériau semblera plat

    // créer le matériau pour les arêtes
    vec4 black = vec4::fromValues(0.0, 0.0, 0.0, 1.0);
    m_EdgesMaterial = new ColorMaterial(black);

    // créer le maillage de la vache
    m_Mesh = new Mesh("CowEdges");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Cow/Cow.obj", "", 0.15);

    // en option : subdiviser le maillage en lissant ou non
    MeshModuleProcessing transformation(m_Mesh);
    m_Mesh->computeNormals();
    transformation.subdivideAll(m_Mesh->getTriangleList(), 1, 1.25);

    // module de dessin
    MeshModuleDrawing renderer(m_Mesh);

    // finaliser l'objet : construire ses VBOs
    m_Mesh->computeNormals();
    m_VBOsetTriangles = renderer.createStripVBOset(m_TrianglesMaterial, true);

    // créer la liste d'arêtes et les VBOs pour les afficher
    m_VBOsetEdges = renderer.createEdgesVBOset(m_EdgesMaterial);
}



void CowEdges::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    bool triangles = true;            // remplit les triangles => opaques
    bool background_color = false;    // si true : les triangles sont de la couleur du fond

    // mettre en place un décalage de polygone
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    // dessiner les triangles uniquement dans le depth buffer
    if (triangles) {

        // en option : bloquer les dessins visibles, mais garder l'écriture du depth buffer
        // ça permet d'avoir une vache toujours de la même couleur que le fond
        if (background_color) glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        // dessiner les triangles mais ils seront invisibles
        m_VBOsetTriangles->onDraw(mat4Projection, mat4ModelView);

        // ré-auroriser les dessins sur l'écran
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    // supprimer le décalage
    glPolygonOffset(0.0, 0.0);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // dessiner les arêtes
    m_VBOsetEdges->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
CowEdges::~CowEdges()
{
    delete m_VBOsetTriangles;
    delete m_VBOsetEdges;
    delete m_Mesh;
    delete m_TrianglesMaterial;
    delete m_EdgesMaterial;
}
