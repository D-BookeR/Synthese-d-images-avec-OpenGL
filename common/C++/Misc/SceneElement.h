#ifndef MISC_ELEMENT3D_H
#define MISC_ELEMENT3D_H

// Définition de la classe SceneElement

#include <map>
#include <list>

#include <gl-matrix.h>
#include <utils.h>
#include <Mesh.h>
#include <Material.h>

/**
 * C'est une classe abstraite indiquant qu'un objet doit avoir une
 * méthode de transformation par une matrice et une méthode de dessin
 * Voir par exemple les lampes et les sous classes de MeshObject
 */
class Drawable
{
public:
    /** application d'une matrice à l'objet */
    virtual void transform(mat4& matrix);

    /**
     * dessin de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    virtual void onDraw(mat4& mat4Projection, mat4& mat4View);

    /** destructeur */
    virtual ~Drawable() {};
};


class SceneElement
{
protected:
    // objet à dessiner
    Drawable* m_Object;

    // hiérarchie
    SceneElement* m_Parent;
    std::list<SceneElement*> m_Children;

    // matrices de transformation directe et temporaire
    mat4 m_Transformation;
    mat4 m_ModelViewTmp;



public:

    /** constructeur */
    SceneElement(Drawable* objet=nullptr, SceneElement* parent=nullptr);

    /** destructeur, libère les ressources */
    virtual ~SceneElement();

    /**
     * définit le parent de cet élément, ça doit être un autre élément ou nullptr
     * @param parent de l'élément
     */
    void setParent(SceneElement* parent);

    /**
     * remet la transformation de l'élément à l'identité
     */
    void identity();

    /**
     * multiplie la transformation de l'élément par une translation
     * @param v vec3
     */
    void translate(const vec3& v);

    /**
     * multiplie la transformation de l'élément par une rotation
     * @param angle en radians
     * @param axis : vec3 donnant l'axe de rotation
     */
    void rotate(float angle, const vec3& axis);

    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe X
     * @param angle en radians
     */
    void rotateX(float angle);

    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe Y
     * @param angle en radians
     */
    void rotateY(float angle);

    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe Z
     * @param angle en radians
     */
    void rotateZ(float angle);

    /**
     * multiplie la transformation de l'élément par une homothétie
     * @param v : vec3
     */
    void scale(const vec3& v);

    /**
     * transforme l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été transformés
     * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
     * @param exclude : s'il est non null, alors ne pas le transformer
     */
    void transform(mat4& mat4ModelView, SceneElement* exclude=nullptr);

    /**
     * dessine l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été dessinés
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
     * @param exclude : s'il est non null, alors ne pas le dessiner
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView, SceneElement* exclude=nullptr);
};

#endif
