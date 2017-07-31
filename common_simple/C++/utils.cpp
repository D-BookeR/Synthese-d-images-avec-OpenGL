#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <SDL_image.h>

#include <utils.h>

static const char kPathSeparator =
#if defined(WIN32) || defined(_WIN32)
                            '\\';
#else
                            '/';
#endif


/**
 * Cette fonction permet d'afficher les erreurs OpenGL accumulées jusque là.
 * Elle est appelée par les macros debugGL(message) et debugGLFatal(message), voir utils.h
 * On met par exemple debguGL("glDrawArrays rectangle"); juste après l'appel OpenGL.
 * Comme ça, si cet appel plante, on aura le message avec le n° de ligne.
 * @param file nom du fichier source dans lequel on appelle cette fonction
 * @param line numéro de ligne dans le source
 * @param message à afficher s'il y a une erreur
 * @param fatal true si toute erreur est fatale, false si c'est seulement un avertissement
 * @return 0 si aucune erreur, 1 s'il y a eu une erreur
 */
int _debugGL(const char *file, int line, const char* message, bool fatal)
{
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        // rajouter un message dans le log
        std::cerr << "glError in " << file << ", line " << line << " : " << gluErrorString(glErr) << " in " << message << std::endl;
        retCode = 1;
        // code d'erreur suivant (OpenGL peut en générer plusieurs d'un coup)
        glErr = glGetError();
    }
    // si le code est non-nul, on arrête
    if (fatal && retCode > 0) exit(EXIT_FAILURE);

    return retCode;
}



namespace Utils {

// fonction pour obtenir le nombre de secondes absolu

#ifdef _WIN32
    // version Win32
    #include <time.h>
    double getAbsoluteTime()
    {
        return (double)clock()  / CLOCKS_PER_SEC;
    }
#else
    // version Linux
    #include <sys/time.h>
    double getAbsoluteTime()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec*1e-6;
    }
#endif


double InitialTime = getAbsoluteTime();
double Time = 0.0;
bool TimeChange = true;
double TimePause = 0.0;


void UpdateTime()
{
    if (TimeChange) {
        Time = (getAbsoluteTime() - InitialTime);
    }
}


void PauseTime()
{
    if (TimeChange) {
        TimePause = getAbsoluteTime();
    } else {
        InitialTime += (getAbsoluteTime() - TimePause);
    }
    TimeChange = !TimeChange;
}



/**
 * retourne le temps global, en nombre de secondes écoulées depuis le lancement de l'application
 */
float getTime()
{
    return Time;
}



/**
 * cette fonction numérote les lignes du texte passé en paramètre
 * @param text : texte à numéroter
 */
static void PrintNumberedLines(const char* text)
{
    // copier le texte pour pouvoir le modifier
    char* copie = strdup(text);
    // parcourir les lignes
    int line_number = 0;
    char* debut = copie;
    char* fin = copie;
    while (*fin != '\0') {
        // chercher le prochain \0 ou \n
        while (*fin != '\0' && *fin != '\n') fin++;
        // afficher le texte compris entre debut et fin exclus
        line_number++;
        std::cerr << line_number << ":\t";
        while (debut < fin) {
            std::cerr << *debut;
            debut++;
        }
        std::cerr << std::endl;
        // préparer le passage à la ligne suivante (si elle existe)
        if (*fin != '\0') {
            fin++;
            debut = fin;
        }
    }
    // libérer la copie
    free(copie);
}


/**
 * cette fonction compile le source en tant que shader du type indiqué
 * @param shaderType : fournir GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER
 * @param source : fournir un texte contenant le programme à compiler
 * @param name : nom du shader, pour afficher des messages d'erreur ou le log
 * @return l'identifiant OpenGL du shader, 0 si erreur
 */
static GLint compileShader(GLenum shaderType, const GLchar* source, std::string name) throw (std::invalid_argument)
{
    /*****DEBUG*****/
    if (shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER) {
        throw std::invalid_argument("Utils::compileShader: shader type is neither GL_VERTEX_SHADER or GL_FRAGMENT_SHADER");
    }
    /*****DEBUG*****/

    // compiler le source
    GLint shader = glCreateShader(shaderType);
    /*****DEBUG*****/
    if (shader == 0) {
        debugGLFatal("Utils::compileShader: glCreateShader failed");
    }
    /*****DEBUG*****/
    if (shader == 0) return 0;

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // vérifier l'état
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) return shader;

    // il y a eu une erreur, afficher le log
    int infologLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 1) {
        int charsWritten  = 0;
        GLchar *infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
        if (status == GL_FALSE) {
            std::cerr << "In " << name << ":" << std::endl;
            std::cerr << infoLog;
            PrintNumberedLines(source);
        }
        free(infoLog);
        // quitter si pb
        if (status == GL_FALSE) throw std::invalid_argument(name);
    }
    return shader;
}


/**
 * cette fonction compile les deux sources, vertex et fragment shader
 * et les relie en tant que programme de shaders complet
 * @param VSsource : source du vertex shader
 * @param FSsource : source du fragment shader
 * @param name : nom du shader pour les messages d'erreurs ou le log
 * @param debug : mettre true si on veut enregistrer les shaders dans des fichiers .vert et .frag
 * @return identifiant OpenGL du programme de shader complet
 */
GLint makeShaderProgram(std::string VSsource, std::string FSsource, std::string name, bool debug) throw (std::invalid_argument)
{
    if (debug) {
        // enregistrement des shaders pour GLSLangValidator
        std::ofstream vsfile;
        vsfile.open(name+".vert");
        vsfile << VSsource;
        vsfile.close();
        std::ofstream fsfile;
        fsfile.open(name+".frag");
        fsfile << FSsource;
        fsfile.close();
    }

    // compiler les shaders séparément
    GLint vertexShader   = compileShader(GL_VERTEX_SHADER,   VSsource.c_str(), "Vertex Shader of "+name);
    GLint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FSsource.c_str(), "Fragment Shader of "+name);

    // créer un programme et ajouter les deux shaders
    GLint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // forcer "glVertex" à être l'attribut 0 (pb si c'est pas comme ça qu'elle s'appelle)
    // voir http://www.opengl.org/wiki/Vertex_Attribute, pour éviter le bug de l'attribut 0 pas lié
    glBindAttribLocation(program, 0, "glVertex");

    // lier le programme
    glLinkProgram(program);

    // vérifier l'état
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {

        // il y a eu une erreur, afficher le log
        int infologLength = 0;
        int charsWritten  = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH,&infologLength);
        if (infologLength > 1) {
            GLchar *infoLog = (char *)malloc(infologLength);
            glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
            std::cerr << "In " << name << ":" << std::endl;
            std::cerr << infoLog << std::endl;
            free(infoLog);
        }

        // quitter par une exception
        throw std::invalid_argument(name);
    }

    return program;
}


/**
 * supprime un shader dont on fournit l'identifiant
 * @param id : identifiant du shader
 */
void deleteShaderProgram(GLint id)
{
    // détacher et supprimer tous ses shaders

    // supprimer le programme
    glDeleteProgram(id);
}


/**
 * cette fonction crée un VBO contenant des GLfloat
 * @param values : tableau de float à mettre dans le VBO
 * @param vbo_type : type OpenGL du VBO, par exemple GL_ARRAY_BUFFER
 * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
 * @return identifiant OpenGL du VBO
 */
GLuint makeFloatVBO(std::vector<GLfloat> values, int vbo_type, int usage)
{
    /*****DEBUG*****/
    if (values.size() < 1) {
        throw std::invalid_argument("Utils::makeFloatVBO: values vector is empty");
    }
    if (vbo_type != GL_ARRAY_BUFFER) {
        throw std::invalid_argument("Utils::makeFloatVBO: second parameter, vbo_type is not GL_ARRAY_BUFFER");
    }
    if (usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW) {
        throw std::invalid_argument("Utils::makeFloatVBO: third parameter, usage is neither GL_STATIC_DRAW or GL_DYNAMIC_DRAW");
    }
    /*****DEBUG*****/
    // créer un VBO et le remplir avec les données
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(vbo_type, id);
    glBufferData(vbo_type, values.size()*sizeof(GLfloat), values.data(), usage);
    glBindBuffer(vbo_type, 0);

    return id;
}

/**
 * cette fonction crée un VBO contenant des GLshort
 * @param values : tableau de GLshort à mettre dans le VBO
 * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
 * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
 * @return identifiant OpenGL du VBO
 */
GLuint makeShortVBO(std::vector<GLshort> values, int vbo_type, int usage)
{
    /*****DEBUG*****/
    if (values.size() < 1) {
        throw std::invalid_argument("Utils::makeShortVBO: values vector is empty");
    }
    if (vbo_type != GL_ARRAY_BUFFER && vbo_type != GL_ELEMENT_ARRAY_BUFFER) {
        throw std::invalid_argument("Utils::makeShortVBO: second parameter, vbo_type is neither GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER");
    }
    if (usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW) {
        throw std::invalid_argument("Utils::makeShortVBO: third parameter, usage is neither GL_STATIC_DRAW or GL_DYNAMIC_DRAW");
    }
    /*****DEBUG*****/
    // créer un VBO et le remplir avec les données
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(vbo_type, id);
    glBufferData(vbo_type, values.size()*sizeof(GLshort), values.data(), usage);
    glBindBuffer(vbo_type, 0);

    return id;
}


/**
 * cette fonction crée un VBO contenant des ushort
 * @param values : tableau de ushort à mettre dans le VBO
 * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
 * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
 * @return identifiant OpenGL du VBO
 */
GLuint makeShortVBO(std::vector<GLushort> values, int vbo_type, int usage)
{
    /*****DEBUG*****/
    if (values.size() < 1) {
        throw std::invalid_argument("Utils::makeUShortVBO: values vector is empty");
    }
    if (vbo_type != GL_ARRAY_BUFFER && vbo_type != GL_ELEMENT_ARRAY_BUFFER) {
        throw std::invalid_argument("Utils::makeUShortVBO: second parameter, vbo_type is neither GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER");
    }
    if (usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW) {
        throw std::invalid_argument("Utils::makeUShortVBO: third parameter, usage is neither GL_STATIC_DRAW or GL_DYNAMIC_DRAW");
    }
    /*****DEBUG*****/
    // créer un VBO et le remplir avec les données
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(vbo_type, id);
    glBufferData(vbo_type, values.size()*sizeof(GLushort), values.data(), usage);
    glBindBuffer(vbo_type, 0);

    return id;
}

/**
 * cette fonction crée un VBO contenant des int
 * @param values : tableau de int à mettre dans le VBO
 * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
 * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
 * @return identifiant OpenGL du VBO
 */
GLuint makeIntVBO(std::vector<GLint> values, int vbo_type, int usage)
{
    /*****DEBUG*****/
    if (values.size() < 1) {
        throw std::invalid_argument("Utils::makeIntVBO: values vector is empty");
    }
    if (vbo_type != GL_ARRAY_BUFFER && vbo_type != GL_ELEMENT_ARRAY_BUFFER) {
        throw std::invalid_argument("Utils::makeIntVBO: second parameter, vbo_type is neither GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER");
    }
    if (usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW) {
        throw std::invalid_argument("Utils::makeIntVBO: third parameter, usage is neither GL_STATIC_DRAW or GL_DYNAMIC_DRAW");
    }
    /*****DEBUG*****/
    // créer un VBO et le remplir avec les données
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(vbo_type, id);
    glBufferData(vbo_type, values.size()*sizeof(GLint), values.data(), usage);
    glBindBuffer(vbo_type, 0);

    return id;
}


/**
 * cette fonction crée un VBO contenant des GLuint
 * @param values : tableau de GLuint à mettre dans le VBO
 * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
 * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
 * @return identifiant OpenGL du VBO
 */
GLuint makeIntVBO(std::vector<GLuint> values, int vbo_type, int usage)
{
    /*****DEBUG*****/
    if (values.size() < 1) {
        throw std::invalid_argument("Utils::makeUIntVBO: values vector is empty");
    }
    if (vbo_type != GL_ARRAY_BUFFER && vbo_type != GL_ELEMENT_ARRAY_BUFFER) {
        throw std::invalid_argument("Utils::makeUIntVBO: second parameter, vbo_type is neither GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER");
    }
    if (usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW) {
        throw std::invalid_argument("Utils::makeUIntVBO: third parameter, usage is neither GL_STATIC_DRAW or GL_DYNAMIC_DRAW");
    }
    /*****DEBUG*****/
    // créer un VBO et le remplir avec les données
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(vbo_type, id);
    glBufferData(vbo_type, values.size()*sizeof(GLuint), values.data(), usage);
    glBindBuffer(vbo_type, 0);

    return id;
}


/**
 * supprime un buffer VBO dont on fournit l'identifiant
 * @param id : identifiant du VBO
 */
void deleteVBO(GLuint id)
{
    glDeleteBuffers(1, &id);
}


/**
 * convertit en radians l'angle fourni en degrés
 * @param deg : angle en degrés
 * @return angle converti en radians
 */
GLfloat radians(const GLfloat deg)
{
    return deg * M_PI / 180.0;
}


/**
 * convertit en degrés l'angle fourni en radians
 * @param rad : angle en radians
 * @return angle converti en degrés
 */
GLfloat degrees(const GLfloat rad)
{
    return rad * 180.0 / M_PI;
}


/**
 * cette fonction convertit un vec3(teinte, saturation, valeur) en (rouge, vert, bleu)
 * NB : les composantes sont toutes comprises entre 0 et 1
 * @param hsv : vec3(h,s,v) contenant la teinte à convertir
 * @return vec3(r,v,b)
 */
vec3 hsv2rgb(vec3 hsv)
{
    float h = (hsv[0] - floor(hsv[0])) * 6.0;
    float s = hsv[1];
    float v = hsv[2];

    if (s <= 0.0) return vec3::fromValues(v,v,v);

    int i = floor(h);
    float ff = h - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - (s * ff));
    float t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        return vec3::fromValues(v,t,p);
    case 1:
        return vec3::fromValues(q,v,p);
    case 2:
        return vec3::fromValues(p,v,t);
    case 3:
        return vec3::fromValues(p,q,v);
    case 4:
        return vec3::fromValues(t,p,v);
    case 5:
    default:
        return vec3::fromValues(v,p,q);
    }
}


/**
 * retourne valeur si elle est comprise entre min et max
 * sinon retourne min ou max selon la borne qui est dépassée
 * @param value : valeur à comparer aux bornes
 * @param min borne inférieure
 * @param max borne supérieure
 * @return valeur ou min ou max, selon les comparaisons
 */
float clamp(float value, float min, float max)
{
    if (value <= min) return min;
    if (value >= max) return max;
    return value;
}
int clamp(int value, int min, int max)
{
    if (value <= min) return min;
    if (value >= max) return max;
    return value;
}


/**
 * prend une photo de l'écran et l'enregistre dans le fichier PPM indiqué
 * NB : ce format de fichier n'est pas du tout compressé. Sa taille sera 3*largeur*hauteur octets.
 * @param filename : nom du fichier PPM à créer/écraser avec l'image
 * @param width : largeur de la fenêtre OpenGL
 * @param height : hauteur de la vue OpenGL
 */
void ScreenShotPPM(const char* filename, int width, int height)
{
    // allouer un tableau de pixels RGB
    const int taille = 3 * width * height;
    const char* pixels = new char[taille];

    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

    // ouverture du fichier en écriture
    std::ofstream fichier;
    fichier.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (! fichier.is_open()) {
        perror(filename);
        return;
    }

    // écriture de l'entête PPM
    fichier << "P6 " << width << " " << height << " 255\n";

    // écriture des pixels en binaire
    for (int y=height-1; y>=0; y--) {
        for (int x=0; x<width; x++) {
            fichier << pixels[(y*width + x)*3 + 0];
            fichier << pixels[(y*width + x)*3 + 1];
            fichier << pixels[(y*width + x)*3 + 2];
        }
    }

    // fermer le fichier
    fichier.close();

    // libérer la mémoire
    delete[] pixels;
}


/**
 * prend une photo de l'écran et l'enregistre dans le fichier PAM indiqué
 * NB : ce format de fichier n'est pas du tout compressé. Sa taille sera 4*largeur*hauteur octets.
 * @param filename : nom du fichier PAM à créer/écraser avec l'image
 * @param width : largeur de la fenêtre OpenGL
 * @param height : hauteur de la vue OpenGL
 */
void ScreenShotPAM(const char* filename, int width, int height)
{
    // allouer un tableau de pixels RGBA
    const int taille = 4 * width * height;
    const char* pixels = new char[taille];

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

    // ouverture du fichier en écriture
    std::ofstream fichier;
    fichier.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (! fichier.is_open()) {
        perror(filename);
        return;
    }

    // écriture de l'entête PAM
    fichier << "P7\n";
    fichier << "WIDTH "<<width<<"\n";
    fichier << "HEIGHT "<<height<<"\n";
    fichier << "DEPTH 4\n";
    fichier << "MAXVAL 255\n";
    fichier << "TUPLTYPE RGB_ALPHA\n";
    fichier << "ENDHDR\n";

    // écriture des pixels en binaire
    for (int y=height-1; y>=0; y--) {
        for (int x=0; x<width; x++) {
            fichier << pixels[(y*width + x)*4 + 0];
            fichier << pixels[(y*width + x)*4 + 1];
            fichier << pixels[(y*width + x)*4 + 2];
            fichier << pixels[(y*width + x)*4 + 3];
        }
    }

    // fermer le fichier
    fichier.close();

    // libérer la mémoire
    delete[] pixels;
}

};

