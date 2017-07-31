// Définition de la classe SceneBase


// classes et modules nécessaires
Requires("libs/Process/FrameBufferObject.js");


/**
 * Définit une base pour dessiner une scène
 */
class SceneBase
{
    /**
     * Définit une base pour dessiner une scène
     * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
     */
    constructor(deferredShading)
    {
        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // lampes
        this.m_Lights = [];

        // FBO pour dessiner en mode Multiple Render Targets
        this.m_DeferredShading = deferredShading;
        this.m_GBuffer = null;     // sera initialisé dans onSurfaceChanged

        // matrice de projection et de transformation
        this.m_Mat4Projection = mat4.create();
        this.m_Mat4ModelView = mat4.create();
    }


    /** destructeur */
    destroy()
    {
        if (this.m_GBuffer != null) this.m_GBuffer.destroy();
    }


    /** affiche les informations sur la caméra **/
    updateInfos()
    {
        throw "method SceneBase.updateInfos should be overloaded in Scene subclass";
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     * @param scale : facteur d'agrandissement du FBO pour simuler un antialiasing global
     */
    onSurfaceChanged(width, height, scale=2)
    {
        // met en place le viewport
        gl.viewport(0, 0, width, height);

        // créer un FBO pour dessiner hors écran avec plusieurs buffers pour stocker toutes les informations nécessaires
        if (this.m_GBuffer != null) this.m_GBuffer.destroy();
        if (this.m_DeferredShading) {
            this.m_GBuffer = new FrameBufferObject(width*scale, height*scale, gl.TEXTURE_2D, gl.TEXTURE_2D, 3);
        } else {
            this.m_GBuffer = null;
        }
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dd : nombre de pixels parcourus
     */
    onZoom(dd)
    {
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    onPan(dx, dy)
    {
    }


    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width : largeur de la vue OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    onRotate(oldX, oldY, x, y, width, height)
    {
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        switch (code) {
        case 'P'.charCodeAt(0):
            // mettre en pause
            PauseTime();
            break;
        }
    }


    /**
     * retourne la matrice ModelView pour dessiner la scène
     * @return matrice ModelView
     */
    getModelView()
    {
        return this.m_Mat4ModelView;
    }


    /**
     * ajoute une lampe pour éclairer le matériau du renderer
     * @param light : lampe à ajouter
     */
    addLight(light)
    {
        this.m_Lights.push(light);
    }


    /**
     * transforme toutes les lampes
     * c'est à dire calcule la position relative à la caméra m_PositionCamera
     * à partir de la position relative à la scène m_Position
     * @param mat4View matrice de positionnement de la scène par rapport à la caméra
     */
    transformLights(mat4View)
    {
        for (let light of this.m_Lights) {
            light.transform(mat4View);
        }
    }


    /**
     * dessine les shadow maps des lampes
     * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
     */
    makeShadowMaps(mat4View)
    {
        for (let light of this.m_Lights) {
            // calculer sa shadow map (si la lampe en gère une)
            light.makeShadowMap(this, mat4View);
        }
    }


    /**
     * transforme les lampes puis calcule leur shadow map s'il y en a
     * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
     */
    prepareLights(mat4View)
    {
        this.transformLights(mat4View);
        this.makeShadowMaps(mat4View);
    }


    /**
     * appelée pour dessiner en mode MRT
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    drawDeferredShading(mat4Projection, mat4ModelView)
    {
        // rediriger les dessins vers le FBO
        if (this.m_DeferredShading && this.m_GBuffer != null) this.m_GBuffer.enable();

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner les objets de la scène
        this.onDraw(mat4Projection, mat4ModelView);

        // revenir au dessin sur l'écran
        if (this.m_DeferredShading && this.m_GBuffer != null) this.m_GBuffer.disable();

        // (debug) dessiner l'un des buffers sur l'écran
        //this.m_GBuffer.onDraw(gl.COLOR_ATTACHMENT0);
        //this.m_GBuffer.onDraw(gl.DEPTH_ATTACHMENT);
    }


    /**
     * rajoute l'éclairement d'une lampe
     * @param light : lampe à ajouter
     * @param is_first : mettre true si c'est la première qu'on ajoute ainsi, false sinon
     */
    addLighting(light, is_first=false)
    {
        if (!this.m_DeferredShading || this.m_GBuffer == null) return;

        if (is_first) {

            // dessiner l'éclairement de la première lampe
            light.process(this.m_GBuffer);

        } else {

            // passer en mode blending, en tenant compte du canal alpha des éclairements ajoutés
            gl.enable(gl.BLEND);
            gl.blendFunc(gl.SRC_ALPHA, gl.SRC_ALPHA);

            // rajouter l'éclairement de la lampe
            light.process(this.m_GBuffer);

            // revenir en mode normal
            gl.disable(gl.BLEND);
        }
    }


    /**
     * rajoute les éclairements de toutes les lampes
     */
    addLightings()
    {
        if (this.m_DeferredShading && this.m_GBuffer != null) {
            let is_first = true;
            for (let light of this.m_Lights) {
                this.addLighting(light, is_first);
                is_first = false;
            }
        }
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        throw "method SceneBase.onDraw should be overloaded in Scene subclass";
    }


    /**
     * dessin de la scène sur l'écran
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice MV
        let mat4ModelView = this.getModelView();

        // positionner les lampes et dessiner leur shadow map s'il y en a
        this.prepareLights(mat4ModelView);

        // dessiner les objets dans le FBO mrt (ou sur l'écran)
        this.drawDeferredShading(this.m_Mat4Projection, mat4ModelView);

        // ajouter les éclairements des lampes
        this.addLightings();
    }
}
