// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Texture360");
Requires("Billboard");
Requires("Ground");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // crée les objets à dessiner
        this.m_Ground = new Ground();

        // créer un ensemble de billboards
        this.m_TxTree = new Texture360("data/textures/Billboards/bb_arbre2", 8);
        this.m_Forest = [];
        let N = 13;
        for (let l=0; l<N; l++) {
            for (let c=0; c<N; c++) {
                // générer une taille variant de 20% autour de 1.0
                let sx = 0.8 + Math.random()*0.4;
                let sy = 0.8 + Math.random()*0.4;

                // la position de l'arbre varie un peu autour de (x=c, y=0, z=l)
                let x = c*(9.0/(N-1)) - 4.5  + (Math.random()-0.5)*0.003;
                let y = 0.0;
                let z = l*(9.0/(N-1)) - 4.5 +  (Math.random()-0.5)*0.003;

                // créer et mémoriser un billboard à cet emplacement
                let tree = new Billboard(this.m_TxTree, vec3.fromValues(x,y,z), sx, sy);

                // ajouter ce nouvel arbre dans la forêt
                this.m_Forest.push(tree);
            }
        }

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);      // laisser voir le dos des billboards

        // formule de blending
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

        // couleur du fond
        gl.clearColor(0.6, 0.7, 1.0, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 40.0;
        this.m_CameraAngleElevation = 11.0;
        this.m_CameraDistance = 9;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(50.0), width / height, 0.1, 80.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // dessiner le terrain
        this.m_Ground.onDraw(mat4Projection, mat4View);

        // positionner chaque arbre
        for (let tree of this.m_Forest) {
            // positionner l'arbre => récupérer sa matrice ModelView pour avoir sa distance
            tree.setModelView(mat4View);
        }

        // classer par distance décroissante à l'oeil
        this.m_Forest.sort(Billboard.prototype.DistanceCompare);

        // dessiner l'ensemble des arbres
        for (let tree of this.m_Forest) {
            tree.onDraw(mat4Projection);
        }
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Ground.destroy();
        for (let tree of this.m_Forest) {
            tree.destroy();
        }
        this.m_TxTree.destroy();
        super.destroy();
    }
}
