// Définition de la classe Pumpkin

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>
#include <MeshModuleAnimation.h>

#include <Pumpkin.h>


/**
 * Constructeur
 */
Pumpkin::Pumpkin()
{
    // définir les matériaux
    vec3 Ks = vec3::fromValues(0.165, 0.165, 0.165);
    m_Materials["Pumpkin01"] = new MorphMaterial("data/models/Pumpkin/Pumpkin01.jpg", Ks, 64.0);
    m_Materials["Pumpkin02"] = new MorphMaterial("data/models/Pumpkin/Pumpkin02.jpg", Ks, 64.0);
    m_Materials["Stalk01"] = new MorphMaterial("data/models/Pumpkin/Stalk01.jpg", Ks, 64.0);
    m_Materials["Stalk02"] = new MorphMaterial("data/models/Pumpkin/Stalk02.jpg", Ks, 64.0);

    // modules de chargement, dessin et morphing
    MeshModuleLoading loader;
    MeshModuleDrawing renderer;
    MeshModuleAnimation animation;

    // charger les maillages, un par matériau
    for (auto const& it: m_Materials) {
        // obtenir le matériau
        std::string matname = it.first;
        MorphMaterial* material = it.second;

        // charger le maillage correspondant à ce matériau
        Mesh mesh("Pumpkin");
        loader.setMesh(&mesh);
        loader.loadObjFile("data/models/Pumpkin/pumpkin.obj", matname, 2.0);

        // charger le maillage du morph n°1
        Mesh morph("Pumpkin_morph1");
        loader.setMesh(&morph);
        loader.loadObjFile("data/models/Pumpkin/pumpkin_morph1.obj", matname, 2.0);

        // construire le morph
        animation.setMesh(&mesh);
        animation.buildMorph(&morph);

        // construire le VBOset pour dessiner ce maillage
        renderer.setMesh(&mesh);
        m_VBOsets[matname] = renderer.createStripVBOset(material, true);
    }
}


/**
 * définit la valeur du morph pour changer la forme de l'objet
 * @param coef entre 0 et 1
 */
void Pumpkin::setMorphCoef(float coef)
{
    // fournir le coefficient à tous les matériaux
    for (auto const& it: m_Materials) {
        MorphMaterial* material = it.second;
        material->setMorphCoef(coef);
    }
}


/**
 * dessine l'objet
 * @param mat4Projection
 * @param mat4ModelView
 */
void Pumpkin::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner les maillages
    for (auto const& it: m_VBOsets) {
        VBOset* vboset = it.second;
        vboset->onDraw(mat4Projection, mat4ModelView);
    }
}


/**
 * Cette méthode supprime les ressources allouées
 */
Pumpkin::~Pumpkin()
{
    for (auto const& it: m_VBOsets) {
        delete it.second;
    }
    for (auto const& it: m_Materials) {
        delete it.second;
    }
}
