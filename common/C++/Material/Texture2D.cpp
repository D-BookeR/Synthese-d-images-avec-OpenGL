/**
 * cette classe lance le chargement d'une image et en fait une texture 2D
 * NB: le chargement est fait par une requête au serveur, donc peut prendre du temps
 * pendant lequel la texture n'est pas utilisable
 * @note il se peut qu'il y ait une erreur "It is forbidden to load a WebGL texture from a cross-domain element that has not been validated with CORS. See https://developer.mozilla.org/en/WebGL/Cross-Domain_Textures"
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <SDL_image.h>

SDL_Surface * flipSurface(SDL_Surface * surface);

#include <utils.h>
#include <Texture2D.h>



//***************************************************************************
// lecture d'une image avec SDL...

// consulter :
//  tutorial :  http://??
//  reference : http://??


// constructeur d'une image = lecteur de fichier


/**
 * le constructeur lance le chargement d'une image et en fait une texture 2D
 * @param filename : nom du fichier contenant l'image à charger
 * @param filtering : mettre GL_LINEAR ou gl.NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
 * @param repetition : mettre GL_CLAMP_TO_EDGE ou GL_REPEAT
 */
Texture2D::Texture2D(const char* filename, GLenum filtering, GLenum repetition)
{
    // valeurs par défaut
    m_TextureID = 0;
    m_Width = -1;
    m_Height = -1;

    loadTexture(filename, filtering, repetition);
}

/**
 * le constructeur lance le chargement d'une image et en fait une texture 2D
 * @param filename : nom du fichier contenant l'image à charger
 * @param filtering : mettre GL_LINEAR ou GL_NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
 * @param repetition : mettre GL_CLAMP_TO_EDGE ou GL_REPEAT
 */
Texture2D::Texture2D(std::string filename, GLenum filtering, GLenum repetition)
{
    // valeurs par défaut
    m_TextureID = 0;
    m_Width = -1;
    m_Height = -1;

    loadTexture(filename.c_str(), filtering, repetition);
}


/**
 * le constructeur lance le chargement d'une image et en fait une texture 2D
 * @param filename : nom du fichier contenant l'image à charger
 * @param filtering : mettre GL_LINEAR ou gl.NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
 * @param repetition : mettre GL_CLAMP_TO_EDGE ou GL_REPEAT
 */
void Texture2D::loadTexture(const char* filename, GLenum filtering, GLenum repetition)
{
    // au cas où la suite plante, on invalide d'abord cette texture
    m_TextureID = 0;

    // chargement de l'image
    SDL_Surface *surface = IMG_Load(filename);
    if (!surface) {
        std::cerr << "Texture2D : impossible d'ouvrir \"" << filename << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    // retourner la surface verticalement
    SDL_Surface *tmp = flipSurface(surface);
    SDL_FreeSurface(surface);
    surface = tmp;

    // infos sur cette texture
    m_Width = surface->w;
    m_Height = surface->h;
    //std::cout << filename << " " << m_Width << "x"<<m_Height << std::endl;

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
        std::cerr << "Texture2D: " << filename << " : format inconnu, "  << (int)surface->format->BytesPerPixel << " octets/pixel" << std::endl;
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

    // faire charger l'image dans l'unité 0 (pb si utilisée par ailleurs)
    glActiveTexture(GL_TEXTURE0);

    // création d'une texture OpenGL
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_Width, m_Height, 0, texture_format, components_type, surface->pixels);

    // libération de l'image SDL
    SDL_FreeSurface(surface);

    // filtrage avec mipmaps ?
    if (filtering == GL_NEAREST_MIPMAP_NEAREST || filtering == GL_LINEAR_MIPMAP_NEAREST ||
        filtering == GL_NEAREST_MIPMAP_LINEAR  || filtering == GL_LINEAR_MIPMAP_LINEAR) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // activer le filtering anisotropique
        if (filtering == GL_LINEAR_MIPMAP_LINEAR) {
            GLfloat maxAniso;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        }
    } else {
        // filtering antialiasing de la texture sans mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    }

    // mode de répétition de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repetition);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repetition);

    // réglage de OpenGL pour avoir un bon rendu des textures
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}


Texture2D::Texture2D(GLenum filtering, GLenum repetition)
{
    // valeurs par défaut
    m_TextureID = 0;
    m_Width = -1;
    m_Height = -1;

    // faire charger l'image dans l'unité 0 (pb si utilisée par ailleurs)
    glActiveTexture(GL_TEXTURE0);

    // création d'une texture OpenGL
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // filtering antialiasing de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);

    // mode de répétition de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repetition);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repetition);

    // réglage de OpenGL pour avoir un bon rendu des textures
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}


/**
 * supprime cette texture
 */
Texture2D::~Texture2D()
{
    glDeleteTextures(1,&m_TextureID);
}


/**
 * cette fonction associe cette texture à une unité de texture pour un shader
 * NB: le shader concerné doit être actif
 * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
 * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
 */
void Texture2D::setTextureUnit(GLenum unit, GLint locSampler)
{
    // si la texture n'est pas bien initialisée
    if (m_TextureID == 0) return;

    // activer l'unité de texture
    glActiveTexture(unit);

    // la lier ou délier à la texture
    if (locSampler < 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        // lier à la variable uniform Sampler2D
        glUniform1i(locSampler, unit-GL_TEXTURE0);
    }
}



SDL_Surface * flipSurface(SDL_Surface * surface)
{
    int current_line,pitch;
    SDL_Surface * fliped_surface =
        SDL_CreateRGBSurface(SDL_SWSURFACE,
               surface->w,surface->h,
               surface->format->BitsPerPixel,
               surface->format->Rmask,
               surface->format->Gmask,
               surface->format->Bmask,
               surface->format->Amask);

    SDL_LockSurface(surface);
    SDL_LockSurface(fliped_surface);

    pitch = surface->pitch;
    for (current_line = 0; current_line < surface->h; current_line ++) {
        memcpy(&((unsigned char* )fliped_surface->pixels)[current_line*pitch],
               &((unsigned char* )surface->pixels)[(surface->h - 1  - current_line)*pitch],
               pitch);
    }

    SDL_UnlockSurface(fliped_surface);
    SDL_UnlockSurface(surface);
    return fliped_surface;
}
