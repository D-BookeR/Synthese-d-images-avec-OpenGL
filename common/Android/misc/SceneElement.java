package com.dbooker.livreopengl.misc;

import java.util.LinkedList;
import java.util.List;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;


/**
 * Définit une classe permettant de gérer une hiérarchie d'objets 3D
 */
@SuppressWarnings("unused")
public class SceneElement
{
    /**
     * une chose (lampe, objet...) qui veut être un SceneElement doit implémenter les méthodes suivantes
     */
    public interface Drawable
    {
        void transform(mat4 mat4View);
        void onDraw(mat4 mat4Projection, mat4 mat4View);
    }


    // objet à dessiner
    protected Drawable m_Object;

    // hiérarchie
    protected SceneElement m_Parent;
    protected List<SceneElement> m_Children;

    // matrices de transformation directe et temporaire
    protected mat4 m_Transformation;
    protected mat4 m_ModelViewTmp;


    /**
     * Constructeur d'un élément 3d
     * @param objet dessinable qui est géré par cet élément
     * @param parent de cet objet, null si aucun
     */
    public SceneElement(Drawable objet, SceneElement parent)
    {
        // objet à dessiner
        m_Object = objet;

        // hiérarchie
        m_Parent = null;
        m_Children = new LinkedList<>();
        setParent(parent);

        // matrices de transformation directe et temporaire
        m_Transformation = mat4.create();
        m_ModelViewTmp = mat4.create();    // évite d'allouer une nouvelle matrice à chaque image
    }


    /** destructeur */
    public void destroy()
    {
    }


    /**
     * définit le parent de cet élément, ça doit être un autre élément ou null
     * @param parent de l'élément
     */
    public void setParent(SceneElement parent)
    {
        // s'il y avait déjà un parent, se retirer de la liste de ses enfants
        if (m_Parent != null) {
            parent.m_Children.remove(this);
        }
        // enregistrer ce parent
        m_Parent = parent;
        // se rajouter dans la liste de ses enfants
        if (parent != null) {
            parent.m_Children.add(this);
        }
    }


    /**
     * remet la transformation de l'élément à l'identité
     */
    public void identity()
    {
        mat4.identity(m_Transformation);
    }


    /**
     * multiplie la transformation de l'élément par une translation
     * @param v vec3
     */
    public void translate(vec3 v)
    {
        mat4.translate(m_Transformation, m_Transformation, v);
    }


    /**
     * multiplie la transformation de l'élément par une rotation
     * @param angle en radians
     * @param axis : vec3 donnant l'axe de rotation
     */
    public void rotate(float angle, vec3 axis)
    {
        mat4.rotate(m_Transformation, m_Transformation,  angle, axis);
    }


    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe X
     * @param angle en radians
     */
    public void rotateX(float angle)
    {
        mat4.rotateX(m_Transformation, m_Transformation,  angle);
    }


    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe Y
     * @param angle en radians
     */
    public void rotateY(float angle)
    {
        mat4.rotateY(m_Transformation, m_Transformation,  angle);
    }


    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe Z
     * @param angle en radians
     */
    public void rotateZ(float angle)
    {
        mat4.rotateZ(m_Transformation, m_Transformation,  angle);
    }


    /**
     * multiplie la transformation de l'élément par une homothétie
     * @param factor : vec3
     */
    public void scale(vec3 factor)
    {
        mat4.scale(m_Transformation, m_Transformation, factor);
    }


    /**
     * transforme l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été transformés
     * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
     * @param exclude : s'il est non null, alors ne pas le transformer
     */
    public void transform(mat4 mat4ModelView, SceneElement exclude)
    {
        // s'il faut remonter, alors transformer le parent dans la transformation inverse
        if (m_Parent != null && m_Parent != exclude) {
            // transformation inverse de this pour transformer l'élément parent
            mat4.invert(m_ModelViewTmp, m_Transformation);
            mat4.multiply(m_ModelViewTmp, mat4ModelView, m_ModelViewTmp);
            // appel récursif pour transformer les SceneElement au dessus de this, mais pas this
            m_Parent.transform(m_ModelViewTmp, this);
        }

        // transformer l'objet géré par cet élément
        if (m_Object != null) {
            m_Object.transform(mat4ModelView);
        }

        // transformer les éléments enfants, sauf celui qui est désigné par exclude
        for (SceneElement child: m_Children) {
            if (child != exclude) {
                // calculer la transformation de cet enfant
                mat4.multiply(m_ModelViewTmp, mat4ModelView, child.m_Transformation);
                // transformer la hiérarchie de cet enfant sans remonter vers this (son parent)
                child.transform(m_ModelViewTmp, this);
            }
        }
    }
    public void transform(mat4 mat4ModelView)
    {
        transform(mat4ModelView, null);
    }


    /**
     * dessine l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été dessinés
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
     * @param exclude : s'il est non null, alors ne pas le dessiner
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView, SceneElement exclude)
    {
        // s'il faut remonter, alors dessiner le parent dans la transformation inverse
        if (m_Parent != null && m_Parent != exclude) {
            // transformation inverse de this pour dessiner l'élément parent
            mat4.invert(m_ModelViewTmp, m_Transformation);
            mat4.multiply(m_ModelViewTmp, mat4ModelView, m_ModelViewTmp);
            // appel récursif pour dessiner les SceneElement au dessus de this, mais pas this
            m_Parent.onDraw(mat4Projection, m_ModelViewTmp, this);
        }

        // dessiner l'objet géré par cet élément
        if (m_Object != null) {
            m_Object.onDraw(mat4Projection, mat4ModelView);
        }

        // dessiner les éléments enfants, sauf celui qui est désigné par exclude
        for (SceneElement child: m_Children) {
            if (child != exclude) {
                // calculer la transformation de cet enfant
                mat4.multiply(m_ModelViewTmp, mat4ModelView, child.m_Transformation);
                // dessiner la hiérarchie de cet enfant sans remonter vers this (son parent)
                child.onDraw(mat4Projection, m_ModelViewTmp, this);
            }
        }
    }
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        onDraw(mat4Projection, mat4ModelView, null);
    }
}
