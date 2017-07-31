#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <stdlib.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>

#include <NoiseValue2D.h>


/**
 * retourne la valeur p(x) interpolée par une cubique entre les deux points fournis
 * @param p0 valeur de la fonction en x=0
 * @param p1 valeur de la fonction en x=1
 * @param x variant entre 0 et 1
 * @return valeur interpolée de la fonction en x
 */
float CubicInterpolation(float p0, float p1, float x)
{
//    float k = x * x * (3 - 2 * x);  // version cubique
    float k = ((6*x - 15) * x + 10) * x * x * x;  // version quintique
    return (1.0-k)*p0 + k*p1;
}

/**
 * retourne la valeur p(x,y) interpolée par une bi-cubique
 * @param p tableau de 2*2 valeurs de la fonction
 * @param x variant entre 0 et 1
 * @param y variant entre 0 et 1
 * @return valeur de la fonction en (x,y)
 */
float BicubicInterpolation(float p[2][2], float x, float y)
{
    // interpoler les valeurs des lignes
    float p0 = CubicInterpolation(p[0][0], p[0][1], x);
    float p1 = CubicInterpolation(p[1][0], p[1][1], x);
    // interpoler en colonne
    return CubicInterpolation(p0, p1, y);
}

GLubyte ubmin(GLubyte a, GLubyte b)
{
    if (a < b) return a;
    return b;
}


/** constructeur
 * @param width largeur du tableau
 * @param height hauteur du tableau
 */
RawNoise2D::RawNoise2D(int width, int height)
{
    // dimensions du bruit
    m_Width = width;
    m_Height = height;

    // tableau de réels [-1, +1] aléatoires
    m_Data = new float[width * height];
    for (int y=0; y<m_Height; y++) {
        for (int x=0; x<m_Width; x++) {
            m_Data[x + y*m_Width] = rand()/(float)RAND_MAX * 2.0 - 1.0;
        }
    }
}


/**
 * retourne la valeur qui est dans le tableau en (x,y)
 * @param x on considère la partie entière de x
 * @param y on considère la partie entière de y
 * @return m_Data en (x,y)
 */
float RawNoise2D::getRawNoise(int x, int y)
{
    // arrondir x à l'entier inférieur valide
    int x0 = floor(x);
    if (x0 < 0) x0 = 0;
    if (x0 >= m_Width) x0 = m_Width-1;

    // arrondir y à l'entier inférieur valide
    int y0 = floor(y);
    if (y0 < 0) y0 = 0;
    if (y0 >= m_Height) y0 = m_Height-1;

    // retourner la valeur en ce point
    return m_Data[x0 + y0*m_Width];
}


/**
 * retourne la valeur interpolée en (x,y)
 * @param x
 * @param y
 * @return l'interpolation bicubique
 */
float RawNoise2D::getSmoothNoise(float x, float y)
{
    // les 4 points autour de (x,y)
    int x0 = floor(x);
    int y0 = floor(y);
    // remplir le tableau des 2x2 valeurs entourant (x,y)
    for (int dy=0; dy<2; dy++) {
        for (int dx=0; dx<2; dx++) {
            m_Tab2x2[dy][dx] = getRawNoise(x0+dx, y0+dy);
        }
    }
    // interpolation bicubique entre ces valeurs
    return BicubicInterpolation(m_Tab2x2, x-x0, y-y0);
}


/**
 * Cette méthode supprime les ressources allouées
 */
RawNoise2D::~RawNoise2D()
{
    delete[] m_Data;
}



/** constructeur
 * @param width largeur de la texture
 * @param height hauteur de la texture
 * @param octaves : nombre de superpositions de bruits bicubiques
 */
NoiseValue2D::NoiseValue2D(int width, int height, int octaves): Texture2D()
{
    // test sur le paramètre
    if (octaves < 1) throw std::invalid_argument("NoiseValue2D: incorrect octave number (less than 1)");

    // générer le pixmap de la texture
    GLubyte* pixmap = new GLubyte[width * height];
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            pixmap[y*width + x] = 0;
        }
    }

    // déterminer la fréquence spatiale et l'amplitude du premier octave
    int frequence = 1 << (octaves-1);
    if (width/frequence < 2) throw std::invalid_argument("NoiseValue2D: too many octaves for width");
    if (height/frequence < 2) throw std::invalid_argument("NoiseValue2D: too many octaves for height");
    float amplitude = 128.0f / (2.0f-1.0f/frequence);

    // ajouter les bruits
    for (int octave=0; octave<octaves; octave++) {

        // créer un bruit de cette fréquence
        RawNoise2D noise(width/frequence, height/frequence);

        // ajouter les valeurs lissées
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                float value = amplitude * (noise.getSmoothNoise((float)x/frequence, (float)y/frequence) + 1.0f);
                pixmap[y*width + x] = Utils::clamp((GLubyte)(pixmap[y*width + x] + value), 0, 255);
            }
        }

        // diminuer l'amplitude et la fréquence spatiale
        amplitude = amplitude * 0.5;
        frequence = frequence / 2;
    }

    // fournir le pixmap à la texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixmap);

    // libérer l'unité de texture
    glBindTexture(GL_TEXTURE_2D, 0);
}


/**
 * Cette méthode supprime les ressources allouées
 */
NoiseValue2D::~NoiseValue2D()
{
}
