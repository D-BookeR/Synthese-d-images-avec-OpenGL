// Définition de la classe Sphere

#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>
#include <MeshModuleDrawing.h>

#include <Sphere.h>



/**
 * Cette fonction définit la classe Sphere.
 * @param nbLon : nombre de divisions en longitude
 * @param nbLat : nombre de divisions en latitude
 * @param texture à appliquer sur l'objet
 */
Sphere::Sphere(int nbLon, int nbLat, Texture2D* texture)
{
    // créer le matériau
    m_Material = new TransparentMaterial(texture, 0.2);

    // créer le maillage : une nappe rectangulaire non refermée sur elle-même
    m_Mesh = new Mesh("Sphere");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addRectangularSurface(nbLon+1, nbLat, "sphere %d-%d", false, false);

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
            vertex->setTexCoord(vec2::fromValues(a,b));
        }
    }

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : longitude variant entre 0 et 1 en tournant autour du centre
 * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
 * @return vec3 contenant les coordonnées
 */
vec3 Sphere::getVertex(float a, float b)
{
    // longitude
    float lon = (a * 2.0 - 1.0) * M_PI;
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


/**
 * affecte la texture DepthMap du shader
 * @param far : depth buffer qui limite les dessins arrière de ce matériau
 * @param near : depth buffer qui limite les dessins avant de ce matériau
 */
void Sphere::setDepthMaps(GLint far, GLint near)
{
    m_Material->setDepthMaps(far, near);
}


/**
 * spécifie la taille de la fenêtre
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Sphere::setWindowDimensions(int width, int height)
{
    m_Material->setWindowDimensions(width, height);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Sphere::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Sphere::~Sphere()
{
    delete m_Mesh;
    delete m_Material;
    delete m_VBOset;
}
