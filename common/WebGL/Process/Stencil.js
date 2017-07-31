/**
 * Cette classe statique fournir des méthodes pour travailler avec les stencils.
 * Le principe d'un stencil est de pouvoir délimiter une zone de l'image dans
 * lauquelle on peut dessiner. Pour travailler avec un stencil, on commence par
 * l'initialiser pour réserver les structures de données sous-jacentes, void init(),
 * puis on passe en mode création, voir create(), on dessine la forme qui va servir
 * de masque, ensuite on passe en mode utilisation, voir use(), pour dessiner
 * à travers le masque, enfin on revient en mode sans stencil, voir unuse()
 * NB; OpenGL ne gère qu'un seul Stencil mais il y a une notion de niveau de masque,
 * on peut imbriquer plusieurs masques ayant des seuils différents.
 */


class Stencil {

    /**
     * initialise le mécanisme des stencils, il ne faut appeler cette méthode qu'une seule fois
     * dans la vie de l'application
     */
    static init()
    {
        // vérifier s'il y a un stencil associé au contexte OpenGL
        let contextAttributes = gl.getContextAttributes();
        let haveStencilBuffer = contextAttributes.stencil;
        if (!haveStencilBuffer) {
            console.error("Rajoutez l'option {stencil:true} dans l'appel à initGL dans main.html");
            console.error("Exemple: initGL(canvas, {stencil: true});");
            throw "Il n'y a pas de stencil buffer, regardez la console";
        }

        // Toute la suite est nécessaire parce que gl.clear(gl.COLOR_BUFFER_BIT) efface l'écran sans tenir compte du stencil
        // Le but est d'effacer l'écran uniquement là où le masque l'autorise

        // construit un VBO pour remplir tout l'écran
        let vertices = [
            -1, -1,
            +1, -1,
            +1, +1,
            -1, +1
        ];
        this.m_VertexBufferId = Utils.makeFloatVBO(vertices, gl.ARRAY_BUFFER, gl.STATIC_DRAW);

        // construit un shader très simple
        let srcVertexShader = dedent
            `#version 300 es
            in vec2 glVertex;
            void main()
            {
                gl_Position = vec4(glVertex, 0.0, 1.0);
            }`;

        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            uniform vec4 color;
            out vec4 glFragColor;
            void main()
            {
                glFragColor = color;
            }`;

        // compiler le shader de dessin
        this.m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "Stencil");

        // déterminer où sont les variables attribute et uniform
        this.m_VertexLoc = gl.getAttribLocation(this.m_ShaderId, "glVertex");
        this.m_ColorLoc  = gl.getUniformLocation(this.m_ShaderId, "color");
    }


    /**
     * libère le mécanisme des stencils, il ne faut appeler cette méthode qu'une seule fois
     * dans la vie de l'application
     */
    static destroy()
    {
        Utils.deleteShaderProgram(this.m_ShaderId);
        Utils.deleteVBO(this.m_VertexBufferId);
    }


    /**
     * passe en mode création du masque du stencil. Le paramètre seuil
     * permet de gérer plusieurs niveaux de masquage : en mode utilisation, seuls les dessins
     * faits au dessus du seuil seront visibles
     * @param seuil pour créer ou compléter le masque
     */
    static create(seuil=1)
    {
        // configuration du stencil
        gl.enable(gl.STENCIL_TEST);                             // activation du Stencil Buffer
        gl.stencilFunc(gl.ALWAYS, seuil, 0xFF);                 // accepter tous les fragments dans ce stencil
        gl.stencilOp(gl.KEEP, gl.KEEP, gl.REPLACE);             // fonctions pour affecter le contenu du stencil
        gl.stencilMask(0xFF);                                   // modifier le stencil

        // effacer le stencil
        gl.clearStencil(0);                                     // mettre des 0 partout quand on vide le stencil
        gl.clear(gl.STENCIL_BUFFER_BIT);                        // vider le stencil

        // bloquer les dessins sur l'écran
        gl.colorMask(false, false, false, false);               // ne pas dessiner de pixels
        gl.depthMask(false);                                    // ne pas écrire sur le depth buffer
    }


    /**
     * continue en mode création du masque du stencil mais avec un seuil
     * différent, si on veut
     * @param seuil pour compléter le masque
     */
    static extend(seuil=1)
    {
        gl.stencilFunc(gl.EQUAL, seuil, 0xFF);                  // on ne dessine que si le stencil vaut déjà  au moins le seuil
        gl.stencilOp(gl.ZERO, gl.INCR, gl.INCR);                // on incrémente le stencil si on a eu le droit de dessiner
    }


    /**
     * passe en mode utilisation du stencil, les dessins qui suivent ne seront
     * visibles que là où le stencil a un seuil au moins égal à celui passé en paramètre
     * @param seuil valeur que le stencil doit avoir pour que les dessins soient visibles
     */
    static use(seuil=1)
    {
        // réactiver l'écriture sur le color buffer et le depth buffer
        gl.colorMask(true, true, true, true);                   // réactiver le dessin sur l'écran
        gl.depthMask(true);                                     // réactiver l'écriture sur le depth buffer

        // repasser en mode normal, avec le stencil actif pour dessiner
        gl.depthFunc(gl.LESS);                                  // test normal du depth buffer
        gl.depthRange(0.0, 1.0);                                // plage complète
        gl.stencilFunc(gl.LEQUAL, seuil, 0xFF);                 // On testera si le stencil est au moins égal au seuil
        gl.stencilOp(gl.KEEP, gl.KEEP, gl.KEEP);
        gl.stencilMask(0x00);                                   // ne pas modifier le stencil
    }


    /**
     * revient en mode "normal", sans stencil
     */
    static disable()
    {
        gl.disable(gl.STENCIL_TEST);
        gl.stencilMask(0x00);                                   // ne pas modifier le stencil
        gl.colorMask(true, true, true, true);                   // réactiver le dessin sur l'écran
        gl.depthMask(true);                                     // réactiver l'écriture sur le depth buffer
        gl.depthFunc(gl.LESS);                                  // test normal du depth buffer
    }


    /**
     * efface l'écran comme glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
     * mais en tenant compte du stencil
     * @param seuil pour effacer seulement ce niveau
     */
    clearScreen(seuil=1)
    {
        // configurer le stencil
        gl.depthRange(1.0, 1.0);                                // on va mettre des 1 partout dans le depth buffer
        gl.depthFunc(gl.ALWAYS);                                // forcer la modification partout
        gl.stencilFunc(gl.EQUAL, seuil, 0xFF);                  // accepter tous les fragments dans ce stencil
        gl.stencilOp(gl.KEEP, gl.KEEP, gl.KEEP);                // fonctions pour affecter le contenu du stencil
        gl.stencilMask(0xFF);                                   // modifier le stencil

        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // fournir la couleur d'effacement de l'écran
        let color = gl.getParameter(gl.COLOR_CLEAR_VALUE);
        vec4.glUniform(this.m_ColorLoc, color);

        // activer et lier le buffer contenant les coordonnées
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_VertexBufferId);
        gl.enableVertexAttribArray(this.m_VertexLoc);
        gl.vertexAttribPointer(this.m_VertexLoc, 2, gl.FLOAT, gl.FALSE, 0, 0);

        // dessiner un quad qui remplit tout l'écran
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver le buffer
        gl.disableVertexAttribArray(this.m_VertexLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        // désactiver le shader
        gl.useProgram(null);
    }
}
