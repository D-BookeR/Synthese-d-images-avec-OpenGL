// Définition de la classe Cylinder


#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>
#include <MeshModuleDrawing.h>

#include <Cylinder.h>


/**
 * constructeur
 * @param spokes_count : nombre de secteurs
 * @param texture à appliquer sur l'objet
 */
Cylinder::Cylinder(int spokes_count, Texture2D* texture)
{
    // créer le matériau transparent
    m_Material = new TransparentMaterial(texture, 0.2);

    // créer le maillage : une nappe rectangulaire non refermée sur elle-même
    m_Mesh = new Mesh("Cylinder");
    MeshModuleTopology topology(m_Mesh);
    m_Num0 = topology.addRectangularSurface(spokes_count+1, 2, "cylindre %d-%d", false, false);
    m_SpokesCount = spokes_count;

    // définir les vertices du tube
    for (int is=0; is<=spokes_count; is++) {
        float a = (float)is / spokes_count;

        // vertex sud
        MeshVertex* vS = m_Mesh->getVertex(is + 1*(spokes_count+1) + m_Num0);
        vS->setCoord(this->getVertex(a, -1));
        vS->setNormal(this->getNormal(a));
        vS->setTexCoord(vec2::fromValues(a, 0));

        // vertex nord
        MeshVertex* vN = m_Mesh->getVertex(is + 0*(spokes_count+1) + m_Num0);
        vN->setCoord(this->getVertex(a, +1));
        vN->setNormal(this->getNormal(a));
        vN->setTexCoord(vec2::fromValues(a, 1));
    }

    // créer le VBOset
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
 * @param b : hauteur sur l'axe y
 * @return vec3 contenant les coordonnées
 */
vec3 Cylinder::getVertex(float a, float b)
{
    // angle de ce point
    float angle = (a * 2.0 - 1.0) * M_PI;

    // conversion cylindriques -> cartésiennes
    float x = sin(angle);
    float y = b;
    float z = cos(angle);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode retourne la normale 3D au point identifié par (a,*)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre, b n'est pas nécessaire
 * @return vec3 contenant les coordonnées du vecteur normal
 */
vec3 Cylinder::getNormal(float a)
{
    // angle de ce point
    float angle = (a * 2.0 - 1.0) * M_PI;

    float nx = sin(angle);
    float ny = 0.0;
    float nz = cos(angle);
    return vec3::fromValues(nx,ny,nz);
}


/**
 * affecte la texture DepthMap du shader
 * @param far : depth buffer qui limite les dessins arrière de ce matériau
 * @param near : depth buffer qui limite les dessins avant de ce matériau
 */
void Cylinder::setDepthMaps(GLint far, GLint near)
{
    m_Material->setDepthMaps(far, near);
}


/**
 * spécifie la taille de la fenêtre
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Cylinder::setWindowDimensions(int width, int height)
{
    m_Material->setWindowDimensions(width, height);
}


/**
 * Modifie l'extension verticale du cylindre
 * @param minY hauteur de la base
 * @param maxY hauteur du sommet
 */
void Cylinder::updateMinMaxY(float minY, float maxY)
{
    for (int is=0; is<=m_SpokesCount; is++) {

        // vertex sud
        MeshVertex* vS = m_Mesh->getVertex(is + 1*(m_SpokesCount+1) + m_Num0);
        vS->getCoord()[1] = minY;

        // vertex nord
        MeshVertex* vN = m_Mesh->getVertex(is + 0*(m_SpokesCount+1) + m_Num0);
        vN->getCoord()[1] = maxY;
    }

    // recréer le VBOset
    delete m_VBOset;
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Cylinder::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Cylinder::~Cylinder()
{
    delete m_Mesh;
    delete m_Material;
    delete m_VBOset;
}
