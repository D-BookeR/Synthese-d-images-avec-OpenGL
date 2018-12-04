#ifndef UTILS_H
#define UTILS_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

#include <gl-matrix.h>

/**
 * Ces macros permet d'afficher les erreurs OpenGL accumulées jusque là.
 * Elles appellent la fonction _debugGL, voir utils.cpp
 * On met par exemple debguGL("glDrawArrays rectangle"); juste après l'appel OpenGL.
 * Comme ça, si cet appel plante, on aura le message avec le n° de ligne.
 * La macro debugGLFatal provoque un exit(EXIT_FAILURE); et arrête immédiatement le programme.
 */
#define debugGL(message)              _debugGL(__FILE__, __LINE__,message,false)
#define debugGLFatal(message)         _debugGL(__FILE__, __LINE__,message,true)

int _debugGL(const char *file, int line, const char* message, bool fatal=false);


namespace Utils
{
    /// Constantes diverses

    // nombre d'unités par vecteur OpenGL
    static const int FLOAT = 1;
    static const int VEC2 = 2;
    static const int VEC3 = 3;
    static const int VEC4 = 4;

    // nombre d'octets
    static const int SIZEOF_FLOAT = 4;
    static const int SIZEOF_VEC2 = 2 * SIZEOF_FLOAT;
    static const int SIZEOF_VEC3 = 3 * SIZEOF_FLOAT;
    static const int SIZEOF_VEC4 = 4 * SIZEOF_FLOAT;


    /**
     * convertit en radians l'angle fourni en degrés
     * @param deg : angle en degrés
     * @return angle converti en radians
     */
    GLfloat radians(const GLfloat deg);

    /**
     * convertit en degrés l'angle fourni en radians
     * @param rad : angle en radians
     * @return angle converti en degrés
     */
    GLfloat degrees(const GLfloat rad);

    extern double Time;
    extern bool TimeChange;
    extern double TimePause;
    void UpdateTime();
    void PauseTime();
    float getTime();


    /**
     * cette fonction compile les deux sources, vertex et fragment shader
     * et les relie en tant que programme de shaders complet
     * @param VSsource : source du vertex shader
     * @param FSsource : source du fragment shader
     * @param name : nom du shader pour les messages d'erreurs ou le log
     * @param debug : mettre true si on veut enregistrer les shaders dans des fichiers .vert et .frag
     * @return identifiant OpenGL du programme de shader complet
     */
    GLint makeShaderProgram(std::string VSsource, std::string FSsource, std::string name, bool debug=false) throw (std::invalid_argument);

    /**
     * supprime un shader dont on fournit l'identifiant
     * @param id : identifiant du shader
     */
    void deleteShaderProgram(GLint id);

    /**
     * cette fonction crée un VBO contenant des GLfloat
     * @param values : std::vector de GLfloat à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    GLuint makeFloatVBO(std::vector<GLfloat> values, int vbo_type, int usage);

    /**
     * cette fonction crée un VBO contenant des GLshort
     * @param values : std::vector de GLshort à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    GLuint makeShortVBO(std::vector<int> values, int vbo_type, int usage);

    /**
     * cette fonction crée un VBO contenant des GLshort
     * @param values : std::vector de GLshort à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    GLuint makeShortVBO(std::vector<GLshort> values, int vbo_type, int usage);

    /**
     * cette fonction crée un VBO contenant des GLushort
     * @param values : std::vector de GLushort à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    GLuint makeShortVBO(std::vector<GLushort> values, int vbo_type, int usage);

    /**
     * cette fonction crée un VBO contenant des GLint
     * @param values : std::vector de GLint à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    GLuint makeIntVBO(std::vector<GLint> values, int vbo_type, int usage);

    /**
     * cette fonction crée un VBO contenant des GLuint
     * @param values : std::vector de GLuint à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    GLuint makeIntVBO(std::vector<GLuint> values, int vbo_type, int usage);

    /**
     * supprime un buffer VBO dont on fournit l'identifiant
     * @param id : identifiant du VBO
     */
    void deleteVBO(GLuint id);

    /**
     * cette fonction convertit un vec3(teinte, saturation, valeur) en (rouge, vert, bleu)
     * @param hsv : vec3(h,s,v) contenant la teinte à convertir
     * @return vec3(r,v,b)
     * @note les composantes sont toutes comprises entre 0 et 1
     */
    vec3 hsv2rgb(const vec3 hsv);

    /**
     * retourne valeur si elle est comprise entre min et max
     * sinon retourne min ou max selon la borne qui est dépassée
     * @param value : valeur à comparer aux bornes
     * @param min borne inférieure
     * @param max borne supérieure
     * @return valeur ou min ou max, selon les comparaisons
     */
    float clamp(float value, float min, float max);
    int clamp(int value, int min, int max);

    /**
     * prend une photo de l'écran et l'enregistre dans le fichier PPM indiqué
     * NB : ce format de fichier n'est pas du tout compressé. Sa taille sera 3*largeur*hauteur octets.
     * @param filename : nom du fichier PPM à créer/écraser avec l'image
     * @param width : largeur de la fenêtre OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    void ScreenShotPPM(const char* filename, int width, int height);

    /**
     * prend une photo de l'écran et l'enregistre dans le fichier PAM indiqué
     * NB : ce format de fichier n'est pas du tout compressé. Sa taille sera 4*largeur*hauteur octets.
     * @param filename : nom du fichier PAM à créer/écraser avec l'image
     * @param width : largeur de la fenêtre OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    void ScreenShotPAM(const char* filename, int width, int height);
};



/**
 * retourne l'indice de l'élement dans le tableau ou -1 s'il est absent
 * @param vect : tableau dans lequel il faut chercher l'element
 * @param element : élément dont on cherche la position
 * @return indice de l'élément ou -1 s'il est absent
 */
template<typename T> static int indexOf(const std::vector<T>& vect, const T& element)
{
    int pos = 0;
    for (typename std::vector<T>::const_iterator it = vect.begin(); it < vect.end(); it++,pos++) {
        if ((*it) == element) return pos;
    }
    return -1;
}


/**
 * supprime l'élément indiqué du tableau
 * @param vect : tableau dans lequel il faut supprimer l'element
 * @param element : élément à supprimer
 */
template<typename T> static void list_remove(std::vector<T>& vect, const T& element)
{
    typename std::vector<T>::iterator it = std::find(vect.begin(), vect.end(), element);
    if (it == vect.end()) return;
    vect.erase(it);
}


/**
 * supprime l'élément indiqué du tableau
 * @param vect : tableau dans lequel il faut supprimer l'element
 * @param index : position 0..N-1 de l'élément
 */
template<typename T> static void list_remove_index(std::vector<T>& vect, int index)
{
    vect.erase(vect.begin()+index);
}


/**
 * setunion(set1, set2) retourne l'union des deux ensembles fournis
 * @return éléments présents dans set1 et/ou dans set2
 * @see set_union de la classe std::set, mais trop mal fichu et trop général
 */
template<typename T> inline std::set<T> setunion(std::set<T> &set1, std::set<T> &set2)
{
    std::set<T> result = set1;
    for (T it: set2) {
        result.insert(it);
    }
    return result;
}


#endif

