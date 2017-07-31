#ifndef PROCESS_STENCIL_H
#define PROCESS_STENCIL_H

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

namespace Stencil {

    /**
     * initialise le mécanisme des stencils, il ne faut appeler cette méthode qu'une seule fois
     * dans la vie de l'application
     */
    void init();

    /**
     * libère le mécanisme des stencils, il ne faut appeler cette méthode qu'une seule fois
     * dans la vie de l'application
     */
    void destroy();

    /**
     * passe en mode création du masque du stencil. Le paramètre seuil
     * permet de gérer plusieurs niveaux de masquage : en mode utilisation, seuls les dessins
     * faits au dessus du seuil seront visibles
     * @param seuil pour créer ou compléter le masque
     */
    void create(int seuil=1);

    /**
     * continue en mode création du masque du stencil mais avec un seuil
     * différent, si on veut
     * @param seuil pour compléter le masque
     */
    void extend(int seuil=1);

    /**
     * passe en mode utilisation du stencil, les dessins qui suivent ne seront
     * visibles que là où le stencil a un seuil au moins égal à celui passé en paramètre
     * @param seuil valeur que le stencil doit avoir pour que les dessins soient visibles
     */
    void use(int seuil=1);

    /**
     * revient en mode "normal", sans stencil
     */
    void disable();


    /**
     * efface l'écran comme glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
     * mais en tenant compte du stencil
     * @param seuil pour effacer seulement ce niveau
     */
    void clearScreen(int seuil=1);


    /* variables de gestion du Stencil Buffer */
    extern GLint m_VertexBufferId;    // VBO permettant de dessiner un rectangle pour remplir tout l'écran
    extern GLint m_ShaderId;          // shader pour remplir tout l'écran en tenant compte du stencil
    extern GLint m_VertexLoc;         // emplacement des coordonnées venant du VBO
    extern GLint m_ColorLoc;          // emplacement de l'uniform contenant la couleur d'effacement

};


#endif
