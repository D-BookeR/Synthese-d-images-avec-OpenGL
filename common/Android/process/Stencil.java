//// ce script fournit des fonctions utilitaires pour les programmes
//// du livre Synthèse d'images à l'aide d'OpenGL

package com.dbooker.livreopengl.process;

import static android.opengl.GLES20.*;

import java.nio.FloatBuffer;

import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.utils.Utils;

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


@SuppressWarnings("unused")
public class Stencil
{
    // variables de gestion du stencil buffer
    protected static int m_VertexBufferId;  // VBO permettant de dessiner un rectangle pour remplir tout l'écran
    protected static int m_ShaderId;        // shader pour remplir tout l'écran en tenant compte du stencil
    protected static int m_VertexLoc;       // emplacement des coordonnées venant du VBO
    protected static int m_ColorLoc;        // emplacement de l'uniform contenant la couleur d'effacement


    /**
     * initialise le mécanisme des stencils, il ne faut appeler cette méthode qu'une seule fois
     * dans la vie de l'application
     */
    public static void init() throws Exception
    {
        // Toute la suite est nécessaire parce que glClear(GL_COLOR_BUFFER_BIT) efface l'écran sans tenir compte du stencil
        // Le but est d'effacer l'écran uniquement là où le masque l'autorise

        // construit un VBO pour remplir tout l'écran
        float[] vertices = {
                -1.0f, -1.0f,
                +1.0f, -1.0f,
                +1.0f, +1.0f,
                -1.0f, +1.0f
        };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // construit un shader très simple
        String srcVertexShader =
            "#version 310 es\n" +
            "in vec2 glVertex;\n" +
            "void main()\n" +
            "{\n" +
            "    gl_Position = vec4(glVertex, 0.0, 1.0);\n" +
            "}";

        String srcFragmentShader =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "uniform vec4 color;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "    glFragColor = color;\n" +
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "Stencil");

        // déterminer où sont les variables attribute et uniform
        m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
        m_ColorLoc  = glGetUniformLocation(m_ShaderId, "color");
    }


    /**
     * libère le mécanisme des stencils, il ne faut appeler cette méthode qu'une seule fois
     * dans la vie de l'application
     */
    public static void destroy()
    {
        Utils.deleteShaderProgram(m_ShaderId);
        Utils.deleteVBO(m_VertexBufferId);
    }


    /**
     * passe en mode création du masque du stencil. Le paramètre seuil
     * permet de gérer plusieurs niveaux de masquage : en mode utilisation, seuls les dessins
     * faits au dessus du seuil seront visibles
     * @param seuil pour créer ou compléter le masque
     */
    public static void create(int seuil)
    {
        // configuration du stencil
        glEnable(GL_STENCIL_TEST);                         // activation du Stencil Buffer
        glStencilFunc(GL_ALWAYS, seuil, 0xFF);             // accepter tous les fragments dans ce stencil
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);         // fonctions pour affecter le contenu du stencil
        glStencilMask(0xFF);                               // modifier le stencil

        // effacer le stencil
        glClearStencil(0);                                 // mettre des 0 partout quand on vide le stencil
        glClear(GL_STENCIL_BUFFER_BIT);                    // vider le stencil

        // bloquer les dessins sur l'écran
        glColorMask(false, false, false, false);           // ne pas dessiner de pixels
        glDepthMask(false);                                // ne pas écrire sur le depth buffer
    }

    /**
     * passe en mode création du masque du stencil avec un seuil de 1
     */
    public static void create()
    {
        create(1);
    }


    /**
     * continue en mode création du masque du stencil mais avec un seuil
     * différent, si on veut
     * @param seuil pour compléter le masque
     */
    public static void extend(int seuil)
    {
        glStencilFunc(GL_EQUAL, seuil, 0xFF);              // on ne dessine que si le stencil vaut déjà  au moins le seuil
        glStencilOp(GL_ZERO, GL_INCR, GL_INCR);            // on incrémente le stencil si on a eu le droit de dessiner
    }

    /**
     * continue en mode création du masque du stencil avec un seuil de 1
     */
    public static void extend()
    {
        extend(1);
    }


    /**
     * passe en mode utilisation du stencil, les dessins qui suivent ne seront
     * visibles que là où le stencil a un seuil au moins égal à celui passé en paramètre
     * @param seuil valeur que le stencil doit avoir pour que les dessins soient visibles
     */
    public static void use(int seuil)
    {
        // réactiver l'écriture sur le color buffer et le depth buffer
        glColorMask(true, true, true, true);               // réactiver le dessin sur l'écran
        glDepthMask(true);                                 // réactiver l'écriture sur le depth buffer

        // repasser en mode normal, avec le stencil actif pour dessiner
        glDepthFunc(GL_LESS);                              // test normal du depth buffer
        glDepthRangef(0.0f, 1.0f);                         // plage complète
        glStencilFunc(GL_LEQUAL, seuil, 0xFF);             // On testera si le stencil est au moins égal au seuil
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilMask(0x00);                               // ne pas modifier le stencil
    }


    /**
     * passe en mode utilisation du stencil, les dessins qui suivent ne seront
     * visibles que là où le stencil a un seuil au moins égal à 1
     */
    public static void use()
    {
        use(1);
    }


    /**
     * revient en mode "normal", sans stencil
     */
    public static void disable()
    {
        glDisable(GL_STENCIL_TEST);
        glStencilMask(0x00);                               // ne pas modifier le stencil
        glColorMask(true, true, true, true);               // réactiver le dessin sur l'écran
        glDepthMask(true);                                 // réactiver l'écriture sur le depth buffer
        glDepthFunc(GL_LESS);                              // test normal du depth buffer
    }


    /**
     * efface l'écran comme glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
     * mais en tenant compte du stencil
     * @param seuil pour effacer seulement ce niveau
     */
    public static void clearScreen(int seuil)
    {
        // configurer le stencil
        glDepthRangef(1.0f, 1.0f);                         // on va mettre des 1 partout dans le depth buffer
        glDepthFunc(GL_ALWAYS);                            // forcer la modification partout
        glStencilFunc(GL_EQUAL, seuil, 0xFF);              // accepter tous les fragments dans ce stencil
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);            // fonctions pour affecter le contenu du stencil
        glStencilMask(0xFF);                               // modifier le stencil

        // activer le shader
        glUseProgram(m_ShaderId);

        // fournir la couleur d'effacement de l'écran
        FloatBuffer tmp = FloatBuffer.allocate(4);
        glGetFloatv(GL_COLOR_CLEAR_VALUE, tmp);
        vec4 color = vec4.fromValues(tmp.get(0), tmp.get(1), tmp.get(2), tmp.get(3));
        vec4.glUniform(m_ColorLoc, color);

        // activer et lier le buffer contenant les coordonnées
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glEnableVertexAttribArray(m_VertexLoc);
        glVertexAttribPointer(m_VertexLoc, 2, GL_FLOAT, false, 0, 0);

        // dessiner un quad qui remplit tout l'écran
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver le buffer
        glDisableVertexAttribArray(m_VertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // désactiver le shader
        glUseProgram(0);
    }
}
