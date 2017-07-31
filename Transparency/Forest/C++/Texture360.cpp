#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <Texture360.h>


/**
 * définit un ensemble de textures pour un billboard
 * @param base : nom complet de la base servant à créer les noms des images. On y rajoute le numéro et .png
 * @param number : nombre de textures à charger. Les numéros vont de 1 à ce nombre.
 * @param filtering : mettre gl.LINEAR par exemple, c'est le filtrage des textures
 */
Texture360::Texture360(std::string base, int number, GLenum filtering)
{
    m_TexturesNumber = number;
    if (number == 1) {
        m_Textures.push_back(new Texture2D(base, filtering, GL_CLAMP_TO_EDGE));
    } else {
        for (int i=0; i<number; i++) {
            // construire le nom du fichier .png à partir de la base et du numéro
            std::stringstream image_filename;
            image_filename << base << "_" << (i+1) << ".png";
            m_Textures.push_back(new Texture2D(image_filename.str(), filtering, GL_CLAMP_TO_EDGE));
        }
    }
}


/**
 * retourne la Texture2D correspondant à l'angle
 * @param angle : float entre 0 et 1
 * @return texture webgl correspondant
 */
Texture2D* Texture360::select(float angle)
{
    if (m_TexturesNumber <= 1) {
        return m_Textures[0];
    } else {
        int n = floor(angle * m_TexturesNumber);
        return m_Textures[n];
    }
}


/**
 * Cette méthode supprime les ressources allouées
 */
Texture360::~Texture360()
{
    for (Texture2D* texture: m_Textures) {
        delete texture;
    }
}
