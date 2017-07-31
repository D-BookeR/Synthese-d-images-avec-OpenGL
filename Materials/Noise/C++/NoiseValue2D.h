#ifndef NOISEVALUE2D_H2D_H
#define NOISEVALUE2D_H2D_H

// Définition de la classe NoiseValue2D

#include <Texture2D.h>


/**
 * retourne la valeur p(x) interpolée par une cubique entre les deux points fournis
 * @param p0 : valeur de la fonction en x=0
 * @param p1 : valeur de la fonction en x=1
 * @param x : variant entre 0 et 1
 * @return valeur interpolée de la fonction en x
 */
float CubicInterpolation(float p0, float p1, float x);

/**
 * retourne la valeur p(x,y) interpolée par une bi-cubique
 * @param p : tableau de 2*2 valeurs de la fonction
 * @param x : variant entre 0 et 1
 * @param y : variant entre 0 et 1
 * @return valeur de la fonction en (x,y)
 */
float BicubicInterpolation(float p[2][2], float x, float y);



/**
 * cette classe gère un tableau de réels [-1, +1[ aléatoires
 */
class RawNoise2D
{
public:

    /** constructeur
     * @param largeur du tableau
     * @param hauteur du tableau
     */
    RawNoise2D(int width, int height);

    /** destructeur */
    ~RawNoise2D();

    /**
     * retourne la valeur qui est dans le tableau en (x,y)
     * @param x on considère la partie entière de x
     * @param y on considère la partie entière de y
     * @return m_Data en (x,y)
     */
    float getRawNoise(int x, int y);

    /**
     * retourne la valeur interpolée en (x,y)
     * @param x
     * @param y
     * @return l'interpolation bicubique
     */
    float getSmoothNoise(float x, float y);

protected:

    int m_Width;
    int m_Height;
    float* m_Data;
    float m_Tab2x2[2][2];
};



/**
 * Cette classe dérive de Texture2D
 * Elle génère une texture aléatoire de type « Value Noise »
 * voir https://en.wikipedia.org/wiki/Value_noise
 * Ce sont des fonctions aléatoires superposées et interpolées
 * de manière bicubique.
 */
class NoiseValue2D: public Texture2D
{
public:
    /** constructeur
     * @param largeur de la texture
     * @param hauteur de la texture
     * @param octaves : nombre de superpositions de bruits bicubiques
     */
    NoiseValue2D(int width, int height, int octaves);

    /** destructeur */
    ~NoiseValue2D();
};


#endif
