#ifndef MATERIAL_MATERIAL_H
#define MATERIAL_MATERIAL_H

// Définition de la classe abstraite Material


#include <gl-matrix.h>
#include <utils.h>

class Material;
class VBOset;

#include <Mesh.h>


class Material
{
protected:

    /**
     * constructeur
     * NB: votre materiau doit ensuite appeler compileShader() pour créer le shader
     * @param nom : nom du matériau
     */
    Material(std::string nom);


public:

    /** destructeur */
    virtual ~Material();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau, voir MeshModuleDrawing
     */
    virtual VBOset* createVBOset();

    /**
     * Cette méthode active le matériau
     * @param mat4Projection : fournir la matrice de projection
     * @param mat4ModelView : fournir la matrice de vue
     */
    virtual void enable(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * Cette méthode désactive le matériau
     */
    virtual void disable();

    /**
     * retourne le nom du matériau
     */
    std::string getName();

    /**
     * retourne l'identifiant du shader de ce matériau
     * @return identifiant du shader
     */
    GLint getShaderId();

    /**
     * Cette méthode de classe retourne une collection de Material.
     * @param folder : contient le fichier mtl et les textures
     * @param mtlfilename : nom seul du fichier au format MTL (ne pas mettre de chemin)
     * @return map(name, Material*) des matériaux définis dans le fichier
     */
    static std::map<std::string, Material*> loadMTL(std::string folder, std::string mtlfilename);

    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param actif : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    void setClipPlane(bool actif, vec4 plane);

    /**
     * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
     * @param actif : true s'il faut garder le code gérant le plan de coupe dans le shader
     */
    void setClipPlane(bool actif);

    /**
     * désactive le plan de coupe
     */
    void resetClipPlane();


protected:


    /**
     * recompile le shader du matériau
     */
    virtual void compileShader();

    virtual std::string getVertexShader();
    virtual std::string getFragmentShader();

protected:

    /** nom du matériau **/
    std::string m_Name;

    /** identifiants liés au shader */
    GLint m_ShaderId;
    GLint m_MatPloc;
    GLint m_MatMVloc;
    GLint m_MatNloc;
    GLint m_ClipPlaneLoc;
    // NB: les attributs glVertex et autres sont dans le VBOset

    /** matrice normale */
    mat3 m_Mat3Normal;

    /** plan de coupe */
    bool m_ClipPlaneOn;
    vec4 m_ClipPlane;

};

#endif
