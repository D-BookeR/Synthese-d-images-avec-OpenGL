/**
 * Définit une classe permettant de gérer une hiérarchie d'objets 3D
 */
class SceneElement
{
    /**
     * Constructeur d'un élément 3d
     * @param object : objet dessinable qui est géré par cet élément
     * @param parent de cet objet, null si aucun
     */
    constructor(object=null, parent=null)
    {
        // objet à dessiner
        this.m_Object = object;

        // hiérarchie
        this.m_Parent = null;
        this.m_Children = [];
        this.setParent(parent);

        // matrices de transformation directe et temporaire
        this.m_Transformation = mat4.create();
        this.m_ModelViewTmp = mat4.create();    // évite d'allouer une nouvelle matrice à chaque image
    }


    /** destructeur */
    destroy()
    {
    }


    /**
     * définit le parent de cet élément, ça doit être un autre élément ou null
     * @param parent de l'élément
     */
    setParent(parent)
    {
        // s'il y avait déjà un parent, se retirer de la liste de ses enfants
        if (this.m_Parent != null) {
            list_remove(parent.m_Children, this);
        }
        // enregistrer ce parent
        this.m_Parent = parent;
        // se rajouter dans la liste de ses enfants
        if (parent != null) {
            parent.m_Children.push(this);
        }
    }


    /**
     * remet la transformation de l'élément à l'identité
     */
    identity()
    {
        mat4.identity(this.m_Transformation);
    }


    /**
     * multiplie la transformation de l'élément par une translation
     * @param v vec3
     */
    translate(v)
    {
        mat4.translate(this.m_Transformation, this.m_Transformation, v);
    }


    /**
     * multiplie la transformation de l'élément par une rotation
     * @param angle en radians
     * @param axis : vec3 donnant l'axe de rotation
     */
    rotate(angle, axis)
    {
        mat4.rotate(this.m_Transformation, this.m_Transformation, angle, axis);
    }


    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe X
     * @param angle en radians
     */
    rotateX(angle)
    {
        mat4.rotateX(this.m_Transformation, this.m_Transformation, angle);
    }


    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe Y
     * @param angle en radians
     */
    rotateY(angle)
    {
        mat4.rotateY(this.m_Transformation, this.m_Transformation, angle);
    }


    /**
     * multiplie la transformation de l'élément par une rotation sur l'axe Z
     * @param angle en radians
     */
    rotateZ(angle)
    {
        mat4.rotateZ(this.m_Transformation, this.m_Transformation, angle);
    }


    /**
     * multiplie la transformation de l'élément par une homothétie
     * @param factor : vec3
     */
    scale(factor)
    {
        mat4.scale(this.m_Transformation, this.m_Transformation, factor);
    }


    /**
     * transforme l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été transformés
     * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
     * @param exclude : s'il est non null, alors ne pas le transformer
     */
    transform(mat4ModelView, exclude=null)
    {
        // s'il faut remonter, alors transformer le parent dans la transformation inverse
        if (this.m_Parent != null && this.m_Parent != exclude) {
            // transformation inverse de this pour transformer l'élément parent
            mat4.invert(this.m_ModelViewTmp, this.m_Transformation);
            mat4.multiply(this.m_ModelViewTmp, mat4ModelView, this.m_ModelViewTmp);
            // appel récursif pour transformer les SceneElement au dessus de this, mais pas this
            this.m_Parent.transform(this.m_ModelViewTmp, this);
        }

        // transformer l'objet géré par cet élément
        if (this.m_Object != null) {
            if (typeof this.m_Object.transform != "undefined")
                this.m_Object.transform(mat4ModelView);
        }

        // transformer les éléments enfants, sauf celui qui est désigné par exclude
        for (let child of this.m_Children) {
            if (child != exclude) {
                // calculer la transformation de cet enfant
                mat4.multiply(this.m_ModelViewTmp, mat4ModelView, child.m_Transformation);
                // transformer la hiérarchie de cet enfant sans remonter vers this (son parent)
                child.transform(this.m_ModelViewTmp, this);
            }
        }
    }


    /**
     * dessine l'élément ainsi que tous ses enfants et ancêtres, sauf ceux qui ont déjà été dessinés
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
     * @param exclude : s'il est non null, alors ne pas le dessiner
     */
    onDraw(mat4Projection, mat4ModelView, exclude=null)
    {
        // s'il faut remonter, alors dessiner le parent dans la transformation inverse
        if (this.m_Parent != null && this.m_Parent != exclude) {
            // transformation inverse de this pour dessiner l'élément parent
            mat4.invert(this.m_ModelViewTmp, this.m_Transformation);
            mat4.multiply(this.m_ModelViewTmp, mat4ModelView, this.m_ModelViewTmp);
            // appel récursif pour dessiner les SceneElement au dessus de this, mais pas this
            this.m_Parent.onDraw(mat4Projection, this.m_ModelViewTmp, this);
        }

        // dessiner l'objet géré par cet élément
        if (this.m_Object != null) {
            this.m_Object.onDraw(mat4Projection, mat4ModelView);
        }

        // dessiner les éléments enfants, sauf celui qui est désigné par exclude
        for (let child of this.m_Children) {
            if (child != exclude) {
                // calculer la transformation de cet enfant
                mat4.multiply(this.m_ModelViewTmp, mat4ModelView, child.m_Transformation);
                // dessiner la hiérarchie de cet enfant sans remonter vers this (son parent)
                child.onDraw(mat4Projection, this.m_ModelViewTmp, this);
            }
        }
    }
}
