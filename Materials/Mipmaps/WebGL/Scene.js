// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("Rectangle");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les deux textures, l'une avec mipmaps, l'autre sans
        // pour filtrage, essayer
        //      gl.NEAREST
        //      gl.LINEAR
        //      gl.NEAREST_MIPMAP_NEAREST
        //      gl.LINEAR_MIPMAP_NEAREST
        //      gl.NEAREST_MIPMAP_LINEAR
        //      gl.LINEAR_MIPMAP_LINEAR
        // voir https://www.opengl.org/wiki/GLAPI/glTexParameter à GL_TEXTURE_MIN_FILTER
        this.m_TextureAvec = new Texture2D("data/textures/sols/4016.jpg", gl.LINEAR_MIPMAP_LINEAR, gl.REPEAT);
        this.m_TextureWithout = new Texture2D("data/textures/sols/4016.jpg", gl.LINEAR, gl.REPEAT);

        this.m_MaterialWith = new TextureMaterial(this.m_TextureAvec);
        this.m_MaterialWithout = new TextureMaterial(this.m_TextureWithout);

        // créer deux rectangles, chacun son matériau
        this.m_ObjectWith = new Rectangle(this.m_MaterialWith);
        this.m_ObjectWithout = new Rectangle(this.m_MaterialWithout);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);      // laisser voir le dos du rectangle

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 0.0;
        this.m_CameraAngleElevation = 6.0;
        this.m_CameraDistance = 15;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 200.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner les rectangles côte à côte
        let mat4ModelViewTmp = mat4.create();

        mat4.translate(mat4ModelViewTmp, mat4ModelView, vec3.fromValues(-100.1,-2,0));
        this.m_ObjectWith.onDraw(mat4Projection, mat4ModelViewTmp);

        mat4.translate(mat4ModelViewTmp, mat4ModelView, vec3.fromValues(+100.1,-2,0));
        this.m_ObjectWithout.onDraw(mat4Projection, mat4ModelViewTmp);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_ObjectWithout.destroy();
        this.m_ObjectWith.destroy();
        this.m_MaterialWithout.destroy();
        this.m_MaterialWith.destroy();
        this.m_TextureWithout.destroy();
        this.m_TextureAvec.destroy();
        super.destroy();
    }
}
