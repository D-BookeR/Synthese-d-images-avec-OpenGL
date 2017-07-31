#ifndef WATERMATERIAL_H
#define WATERMATERIAL_H

// Définition de la classe WaterMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <FrameBufferObject.h>
#include <Material.h>
#include <VBOset.h>


/**
 * Cette classe réalise la fusion du reflet et de la vue sous-surface.
 * Elle contient plusieurs constantes qui correspondent aux textures et objets employés
 */
class WaterMaterial: public Material
{
public:

    WaterMaterial();

    virtual ~WaterMaterial();

    /**
     * fournit les deux g-buffer utilisés par ce matériau
     * @param FBOreflection : FBO contenant deux color buffers : couleur diffuse et position du reflet
     * @param FBObackground : FBO contenant deux color buffers : couleur diffuse et position du fond
     * @param width : largeur du viewport
     * @param height : hauteur du viewport
     */
    void setGBuffers(FrameBufferObject* FBOreflection, FrameBufferObject* FBObackground, int width, int height);

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

    /**
     * Cette méthode active le matériau
     * @param mat4Projection : fournir la matrice de projection
     * @param mat4ModelView : fournir la matrice de vue
     */
    void enable(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * Cette méthode désactive le matériau
     */
    void disable();


protected:

    virtual std::string getVertexShader();
    virtual std::string getFragmentShader();
    virtual void compileShader();

protected:

    Texture2D* m_MapPhase;

    FrameBufferObject* m_FBOreflection;
    FrameBufferObject* m_FBObackground;

    mat3 m_Mat3Normal;
    vec2 m_ViewPort;

    GLenum m_Unit;

    GLint m_MapIslandPositionLoc;
    GLint m_MapReflectionLoc;
    GLint m_MapRefractionLoc;
    GLint m_MapPhaseLoc;
    GLint m_ViewPortLoc;
    GLint m_TimeLoc;
};

#endif
