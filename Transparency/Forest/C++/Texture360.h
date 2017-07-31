#ifndef TEXTURE360_H360_H
#define TEXTURE360_H360_H

// Définition de la classe Texture360

#include <string>
#include <vector>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>


class Texture360
{
public:

    /**
     * définit un ensemble de textures
     * @param base : nom complet de la base servant à créer les noms des images. On y rajoute le numéro et .png
     * @param nombre : nombre de textures à charger. Les numéros vont de 1 à ce nombre.
     * @param filtrage : mettre gl.LINEAR par exemple, c'est le filtrage des textures
     */
    Texture360(std::string base, int nombre, GLenum filtering=GL_LINEAR_MIPMAP_LINEAR);

    /**
     * retourne la Texture2D correspondant à l'angle
     * @param angle : float entre 0 et 1
     * @return texture webgl correspondant
     */
    Texture2D* select(float angle);

    /** destructeur */
    ~Texture360();

protected:

    // nombre de textures gérées
    int m_TexturesNumber;

    // liste des textures
    std::vector<Texture2D*> m_Textures;

};

#endif
