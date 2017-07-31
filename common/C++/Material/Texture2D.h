#ifndef MATERIAL_TEXTURE2D_H
#define MATERIAL_TEXTURE2D_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <string>

class Texture2D {
public:
    // constructeurs...
    Texture2D(GLenum filtering=GL_LINEAR, GLenum repetition=GL_CLAMP_TO_EDGE);

    /**
     * le constructeur lance le chargement d'une image et en fait une texture 2D
     * @param filename : nom du fichier contenant l'image à charger
     * @param filtering : mettre GL_LINEAR ou gl.NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
     * @param repetition : mettre GL_CLAMP_TO_EDGE ou GL_REPEAT
     */
    Texture2D(const char* filename, GLenum filtering=GL_LINEAR, GLenum repetition=GL_CLAMP_TO_EDGE);
    Texture2D(std::string filename, GLenum filtering=GL_LINEAR, GLenum repetition=GL_CLAMP_TO_EDGE);

    // destructeur
    virtual ~Texture2D();

    /**
     * cette fonction associe la texture à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple gl.TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou <0 pour désactiver la texture
     */
    void setTextureUnit(GLenum unit, GLint locSampler=-1);

    // informations sur la texture
    GLuint m_TextureID;              // numéro d'identification de OpenGL
    GLuint m_Width, m_Height;    // dimensions

private:

    /**
     * le constructeur lance le chargement d'une image et en fait une texture 2D
     * @param filename : nom du fichier contenant l'image à charger
     * @param filtering : mettre GL_LINEAR ou gl.NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
     * @param repetition : mettre GL_CLAMP_TO_EDGE ou GL_REPEAT
     */
    void loadTexture(const char* filename, GLenum filtering, GLenum repetition);
};


#endif
