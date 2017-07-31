/**
 * Définit une classe permettant de gérer une hiérarchie d'objets 3D
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <ios>

#include <utils.h>
#include <SceneElement.h>


void Drawable::transform(mat4& mat4View)
{
    // ne fait rien
}

void Drawable::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // ne fait rien
}


/**
 * Constructeur d'un élément 3d
 * @param object : objet dessinable qui est géré par cet élément
 * @param parent de cet objet, nullptr si aucun
 */
SceneElement::SceneElement(Drawable* object, SceneElement* parent)
{
    // objet à dessiner
    m_Object = object;

    // hiérarchie
    m_Parent = nullptr;
    setParent(parent);

    // matrices de transformation directe et temporaire
    m_Transformation = mat4::create();
    m_ModelViewTmp = mat4::create();    // évite d'allouer une nouvelle matrice à chaque image
}


/** destructeur */
SceneElement::~SceneElement()
{
}


/**
 * définit le parent de cet élément, ça doit être un autre élément ou nullptr
 * @param parent de l'élément
 */
void SceneElement::setParent(SceneElement* parent)
{
    // s'il y avait déjà un parent, se retirer de la liste de ses enfants
    if (m_Parent != nullptr) {
        parent->m_Children.remove(this);
    }
    // enregistrer ce parent
    m_Parent = parent;
    // se rajouter dans la liste de ses enfants
    if (parent != nullptr) {
        parent->m_Children.push_front(this);
    }
}


/**
 * remet la transformation de l'élément à l'identité
 */
void SceneElement::identity()
{
    mat4::identity(m_Transformation);
}


/**
 * multiplie la transformation de l'élément par une translation
 * @param v vec3
 */
void SceneElement::translate(const vec3& v)
{
    mat4::translate(m_Transformation, m_Transformation, v);
}


/**
 * multiplie la transformation de l'élément par une rotation
 * @param angle en radians
 * @param axis : vec3 donnant l'axe de rotation
 */
void SceneElement::rotate(float angle, const vec3& axis)
{
    mat4::rotate(m_Transformation, m_Transformation, angle, axis);
}


/**
 * multiplie la transformation de l'élément par une rotation sur l'axe X
 * @param angle en radians
 */
void SceneElement::rotateX(float angle)
{
    mat4::rotateX(m_Transformation, m_Transformation, angle);
}


/**
 * multiplie la transformation de l'élément par une rotation sur l'axe Y
 * @param angle en radians
 */
void SceneElement::rotateY(float angle)
{
    mat4::rotateY(m_Transformation, m_Transformation, angle);
}


/**
 * multiplie la transformation de l'élément par une rotation sur l'axe Z
 * @param angle en radians
 */
void SceneElement::rotateZ(float angle)
{
    mat4::rotateZ(m_Transformation, m_Transformation, angle);
}


/**
 * multiplie la transformation de l'élément par une homothétie
 * @param factor : vec3
 */
void SceneElement::scale(const vec3& factor)
{
    mat4::scale(m_Transformation, m_Transformation, factor);
}


/**
 * transforme l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été transformés
 * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
 * @param exclude : s'il est non null, alors ne pas le transformer
 */
void SceneElement::transform(mat4& mat4ModelView, SceneElement* exclude)
{
    // s'il faut remonter, alors transformer le parent dans la transformation inverse
    if (m_Parent != nullptr && m_Parent != exclude) {
        // transformation inverse de this pour transformer l'élément parent
        mat4::invert(m_ModelViewTmp, m_Transformation);
        mat4::multiply(m_ModelViewTmp, mat4ModelView, m_ModelViewTmp);
        // appel récursif pour transformer les SceneElement au dessus de this, mais pas this
        m_Parent->transform(m_ModelViewTmp, this);
    }

    // transformer l'objet géré par cet élément
    if (m_Object != nullptr) {
        m_Object->transform(mat4ModelView);
    }

    // transformer les éléments enfants, sauf celui qui est désigné par exclude
    for (SceneElement* child: m_Children) {
        if (child != exclude) {
            // calculer la transformation de cet enfant
            mat4::multiply(m_ModelViewTmp, mat4ModelView, child->m_Transformation);
            // transformer la hiérarchie de cet enfant sans remonter vers this (son parent)
            child->transform(m_ModelViewTmp, this);
        }
    }
}


/**
 * dessine l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été dessinés
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
 * @param exclude : s'il est non null, alors ne pas le dessiner
 */
void SceneElement::onDraw(mat4& mat4Projection, mat4& mat4ModelView, SceneElement* exclude)
{
    // s'il faut remonter, alors dessiner le parent dans la transformation inverse
    if (m_Parent != nullptr && m_Parent != exclude) {
        // transformation inverse de this pour dessiner l'élément parent
        mat4::invert(m_ModelViewTmp, m_Transformation);
        mat4::multiply(m_ModelViewTmp, mat4ModelView, m_ModelViewTmp);
        // appel récursif pour dessiner les SceneElement au dessus de this, mais pas this
        m_Parent->onDraw(mat4Projection, m_ModelViewTmp, this);
    }

    // dessiner l'objet géré par cet élément
    if (m_Object != nullptr) {
        m_Object->onDraw(mat4Projection, mat4ModelView);
    }

    // dessiner les éléments enfants, sauf celui qui est désigné par exclude
    for (SceneElement* child: m_Children) {
        if (child != exclude) {
            // calculer la transformation de cet enfant
            mat4::multiply(m_ModelViewTmp, mat4ModelView, child->m_Transformation);
            // dessiner la hiérarchie de cet enfant sans remonter vers this (son parent)
            child->onDraw(mat4Projection, m_ModelViewTmp, this);
        }
    }
}
