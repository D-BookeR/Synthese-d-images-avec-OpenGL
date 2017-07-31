// Définition de la classe Earth

#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>
#include <MeshModuleDrawing.h>
#include <DeferredShadingMaterial.h>

#include <Earth.h>



/**
 * Classe Earth = sphere avec une texture
 * @param nbLon : nombre de divisions en longitude
 * @param nbLat : nombre de divisions en latitude
 */
Earth::Earth(int nbLon, int nbLat)
{
    // créer le maillage : une nappe rectangulaire
    m_Mesh = new Mesh("Earth");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addRectangularSurface(nbLon+1, nbLat, "sphere %d-%d", true, false);

    // aller du pôle sud au pôle nord, pour définir les sommets
    for (int ilat=0; ilat<nbLat; ilat++) {
        // faire le tour du globe
        for (int ilon=0; ilon<=nbLon; ilon++) {
            // récupérer le sommet
            MeshVertex* vertex = m_Mesh->getVertex(ilon + ilat*(nbLon+1) + num0);
            // calculer ses coordonnées
            float a = (float)ilon / nbLon;
            float b = (float)ilat / (nbLat-1);
            vertex->setCoord(this->getVertex(a, b));
            vertex->setNormal(this->getVertex(a, b));
            vertex->setTexCoord(vec2::fromValues(1-a,b));
        }
    }

    // définir le matériau de l'objet
    std::string diffuse = "data/textures/earth/earth_map.jpg";
    vec3 Ks = vec3::fromValues(0.9, 0.9, 0.9);
    float Ns = 128;
    m_Material = new DeferredShadingMaterial(diffuse, Ks, Ns);

    // créer le VBOset de dessin
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : longitude variant entre 0 et 1 en tournant autour du centre
 * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
 * @return vec3 contenant les coordonnées
 */
vec3 Earth::getVertex(float a, float b)
{
    // longitude : 0.5 - a pour décaler 0° au niveau du méridien de greenwich et tourner dans le bon sens
    float lon = (0.5 - a) * 2.0 * M_PI;
    float coslon = cos(lon);
    float sinlon = sin(lon);

    // latitude
    float lat = (b - 0.5) * M_PI;
    float coslat = cos(lat);
    float sinlat = sin(lat);

    // conversion sphériques -> cartésiennes
    float x = sinlon * coslat;
    float y = sinlat;
    float z = coslon * coslat;
    return vec3::fromValues(x,y,z);
}


void Earth::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Earth::~Earth()
{
    delete m_Material;
    delete m_Mesh;
    delete m_VBOset;
}
