#ifndef MATERIAL_TEXTURECUBE_H
#define MATERIAL_TEXTURECUBE_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>


/**
 * cette classe effectue le chargement d'un groupe de 6 images et en fait une texture cube
 * @param chemin : chemin terminé par un / indiquant un répertoire dans lequel on trouve 6 images : {pos|neg}[xyz].jpg
 */
class TextureCube {
public:
    // constructeurs...
    TextureCube(const char* dirname, GLenum filtering=GL_LINEAR);
    TextureCube(std::string dirname, GLenum filtering=GL_LINEAR);

    // destructeur
    ~TextureCube();

    /**
     * cette fonction associe une texture à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple gl.TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou <0 pour désactiver la texture
     */
    void setTextureUnit(GLenum unit, GLint locSampler=-1);

    // informations sur la texture
    GLuint m_TextureID;              // numéro d'identification de OpenGL
    GLuint m_Width, m_Height;    // dimensions

private:

    void loadTexture(std::string filename, GLenum filtering);
    void loadImageFace(std::string face_name, GLenum idface);

};


#endif
