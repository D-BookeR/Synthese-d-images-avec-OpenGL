/**
 * cette classe effectue le chargement d'un groupe de 6 images et en fait une texture cube
 * NB: le chargement est fait par une requête au serveur, donc peut prendre du temps
 * pendant lequel la texture n'est pas utilisable. La texture n'est utilisable que lorsque
 * les 6 images ont toutes été chargées.
 * @note il se peut qu'il y ait une erreur "It is forbidden to load a WebGL texture from a cross-domain element that has not been validated with CORS. See https://developer.mozilla.org/en/WebGL/Cross-Domain_Textures"
 * @param chemin : chemin terminé par un / indiquant un répertoire dans lequel on trouve 6 images : {pos|neg}[xyz].jpg
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <SDL_image.h>

#include <TextureCube.h>



static const char kPathSeparator =
#if defined(WIN32) || defined(_WIN32)
                            '\\';
#else
                            '/';
#endif



//***************************************************************************
// lecture d'une image avec SDL...

// consulter :
//  tutorial :  http://??
//  reference : http://??


// constructeur d'une image = lecteur de fichier

TextureCube::TextureCube(const char* dirname, GLenum filtering)
{
    loadTexture(dirname, filtering);
}

/**
 * le constructeur lance le chargement de 6 images et en fait une texture Cube
 * @param dirname : chemin terminé par un / indiquant un répertoire dans lequel on trouve 6 images : {pos|neg}[xyz].jpg
 * @param filtering : mettre GL_LINEAR ou GL_NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
 */
TextureCube::TextureCube(std::string dirname, GLenum filtering)
{
    loadTexture(dirname.c_str(), filtering);
}


void TextureCube::loadTexture(std::string dirname, GLenum filtering)
{
    // faire charger les images dans l'unité 0 (pb si elle est utilisée par ailleurs)
    glActiveTexture(GL_TEXTURE0);

    // création et sélection d'une texture OpenGL
    glGenTextures (1, &m_TextureID);
    glBindTexture (GL_TEXTURE_CUBE_MAP, m_TextureID);

    // filtrage antialiasing de la texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filtering);

    // mode de répétition de la texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // réglage de OpenGL pour avoir un bon rendu des textures
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // charger les 6 images, chacune sur une face
    loadImageFace(dirname+"/posx.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    loadImageFace(dirname+"/negx.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    loadImageFace(dirname+"/posy.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    loadImageFace(dirname+"/negy.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    loadImageFace(dirname+"/posz.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    loadImageFace(dirname+"/negz.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    // libérer l'unité de texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


/**
 * Cette méthode charge l'image nomface associée à la face identifiée par idface
 * @param face_name : nom complet du fichier contenant l'image
 * @param idface : identifiant OpenGL de la face, ex : GL_TEXTURE_CUBE_MAP_POSITIVE_X
 */
void TextureCube::loadImageFace(std::string face_name, GLenum idface)
{
    // chargement de l'image
    SDL_Surface *surface = IMG_Load(face_name.c_str());
    if (!surface) {
        std::cerr << "TextureCube : unable to open " << face_name << std::endl;
        exit(EXIT_FAILURE);
    }

    // infos sur cette texture
    m_Width = surface->w;
    m_Height = surface->h;

    // détermination du format exact
    GLenum texture_format, internal_format, components_type;
    switch (surface->format->BytesPerPixel) {
    case 4:
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGBA;
            components_type = GL_UNSIGNED_INT_8_8_8_8_REV;
            //std::cout << filename << " RGBA 8888 RGBA8" << std::endl;
        } else {
            texture_format = GL_BGRA;
            components_type = GL_UNSIGNED_INT_8_8_8_8;
            //std::cout << filename << " BGRA 8888 RGBA8" << std::endl;
        }
        internal_format = GL_RGBA8;
        break;
    case 3:
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGB;
            components_type = GL_UNSIGNED_BYTE;
            //std::cout << filename << " RGB octets RGB8" << std::endl;
        } else {
            texture_format = GL_BGR;
            components_type = GL_UNSIGNED_BYTE;
            //std::cout << filename << " BGR octets RGB8" << std::endl;
        }
        internal_format = GL_RGB8;
        break;
    case 1:
        texture_format = GL_LUMINANCE;
        components_type = GL_UNSIGNED_BYTE;
        internal_format = GL_LUMINANCE;
        //std::cout << filename << " luminance" << std::endl;
        break;
    default:
        std::cerr << "TextureCube: " << face_name << " : format inconnu, "  << (int)surface->format->BytesPerPixel << " octets/pixel" << std::endl;
    }

    // alignement des pixels
    int alignment = 8;
    while (surface->pitch%alignment) alignment>>=1; // x%1==0 for any x
    glPixelStorei(GL_UNPACK_ALIGNMENT,alignment);
    int expected_pitch = (m_Width*surface->format->BytesPerPixel+alignment-1)/alignment*alignment;
    if (surface->pitch-expected_pitch>=alignment) {
        // Alignment alone wont't solve it now
        glPixelStorei(GL_UNPACK_ROW_LENGTH,surface->pitch/surface->format->BytesPerPixel);
    } else {
        glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
    }

    // la fournir à OpenGL
    glTexImage2D(idface, 0, internal_format, m_Width, m_Height, 0, texture_format, components_type, surface->pixels);

    // libération de l'image SDL
    SDL_FreeSurface(surface);
}


/**
 * cette fonction associe cette texture à une unité de texture pour un shader
 * NB: le shader concerné doit être actif
 * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
 * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
 */
void TextureCube::setTextureUnit(GLenum unit, GLint locSampler)
{
    // si la texture n'est pas bien initialisée
    if (m_TextureID == 0) return;

    // activer l'unité de texture
    glActiveTexture(unit);

    // la lier ou délier à la texture
    if (locSampler < 0) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    } else {
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
        // lier l'unité à la variable uniform Sampler2D
        glUniform1i(locSampler, unit-GL_TEXTURE0);
    }
}


/**
 * supprime cette texture
 */
TextureCube::~TextureCube()
{
    glDeleteTextures(1, &m_TextureID);
}
