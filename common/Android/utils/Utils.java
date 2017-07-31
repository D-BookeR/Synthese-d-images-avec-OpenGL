package com.dbooker.livreopengl.utils;

import static android.opengl.GLES20.*;
import static android.opengl.GLU.*;

import java.lang.StringBuilder;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.Collection;

import com.dbooker.livreopengl.glmatrix.vec3;

import android.util.Log;


@SuppressWarnings("unused")
public class Utils
{
    private static final String TAG = "OpenGL";

    /// Constantes diverses

    // nombre d'unités par vecteur OpenGL
    public static final int FLOAT = 1;
    public static final int VEC2 = 2;
    public static final int VEC3 = 3;
    public static final int VEC4 = 4;

    // nombre d'octets
    public static final int SIZEOF_FLOAT = 4;
    public static final int SIZEOF_VEC2 = 2 * SIZEOF_FLOAT;
    public static final int SIZEOF_VEC3 = 3 * SIZEOF_FLOAT;
    public static final int SIZEOF_VEC4 = 4 * SIZEOF_FLOAT;

    // temps écoulé depuis le début
    public static float Time;


    public static void PauseTime()
    {
    }


    public static int debugGL(String text)
    {
        int glErr;
        int retCode = 0;

        glErr = glGetError();
        while (glErr != GL_NO_ERROR) {
            // rajouter un message dans le log
            Log.e(TAG, "glError : "+gluErrorString(glErr)+" in "+text);
            retCode = 1;
            // code d'erreur suivant (OpenGL peut en générer plusieurs d'un coup)
            glErr = glGetError();
        }
        return retCode;
    }


    /**
     * cette fonction numérote les lignes du texte passé en paramètre
     * @param text : texte à numéroter
     */
    private static void PrintNumberedLines(String text)
    {
        // séparer selon les retours à la ligne
        String[] lines = text.split("\n");
        // parcourir les lignes
        for (int line_number=0; line_number<lines.length; line_number++) {
            Log.i(TAG, (line_number+1) + ":\t" + lines[line_number]);
        }
    }


    /**
     * cette fonction compile le source en tant que shader du type indiqué
     * @param shaderType : fournir GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER
     * @param source : fournir un texte contenant le programme à compiler
     * @param name : nom du shader, pour afficher des messages d'erreur ou le log
     * @return l'identifiant OpenGL du shader, 0 si erreur
     */
    private static int compileShader(int shaderType, String source, String name) throws Exception
    {
        int shader = glCreateShader(shaderType);
        if (shader != 0) {
            glShaderSource(shader, source);
            glCompileShader(shader);
            int[] compiled = new int[1];
            glGetShaderiv(shader, GL_COMPILE_STATUS, compiled, 0);
            if (compiled[0] == 0) {
                Log.e(TAG, "Compilation error in " + name + " :");
                Log.e(TAG, glGetShaderInfoLog(shader));
                glDeleteShader(shader);
                PrintNumberedLines(source);
                throw new Exception("Compilation error in "+name);
            }
        }
        return shader;
    }


    /**
     * cette fonction compile les deux sources, vertex et fragment shader
     * et les relie en tant que programme de shaders complet
     * @param VSsource : source du vertex shader
     * @param FSsource : source du fragment shader
     * @param name : nom du shader pour les messages d'erreurs ou le log
     * @return identifiant OpenGL du programme de shader complet
     */
    public static int makeShaderProgram(String VSsource, String FSsource, String name) throws Exception
    {
        int vertexShader   = compileShader(GL_VERTEX_SHADER, VSsource,   "Vertex Shader of "+name);
        int fragmentShader = compileShader(GL_FRAGMENT_SHADER, FSsource, "Fragment Shader of "+name);

        // créer un programme et ajouter les deux shaders
        int program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        // forcer "glVertex" à être l'attribut 0 (pb si c'est pas comme ça qu'elle s'appelle)
        // voir http://www.opengl.org/wiki/Vertex_Attribute, pour éviter le bug de l'attribut 0 pas lié
        glBindAttribLocation(program, 0, "glVertex");

        // lier le programme
        glLinkProgram(program);

        // check for errors
        int[] linkStatus = new int[1];
        glGetProgramiv(program, GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GL_TRUE) {
            Log.e(TAG, "Unable to link Vertex Shader and Fragment Shader of shader "+name+":");
            Log.e(TAG, glGetProgramInfoLog(program));
            glDeleteProgram(program);
            throw new Exception("Unable to link Vertex Shader and Fragment Shader of shader "+name);
        }
        return program;
    }
    public static int makeShaderProgram(StringBuilder VSsource, StringBuilder FSsource, String name) throws Exception
    {
        return makeShaderProgram(VSsource.toString(), FSsource.toString(), name);
    }
    public static int makeShaderProgram(String VSsource, StringBuilder FSsource, String name) throws Exception
    {
        return makeShaderProgram(VSsource, FSsource.toString(), name);
    }
    public static int makeShaderProgram(StringBuilder VSsource, String FSsource, String name) throws Exception
    {
        return makeShaderProgram(VSsource.toString(), FSsource, name);
    }


    /**
     * supprime un shader dont on fournit l'identifiant
     * @param id : identifiant du shader
     */
    public static void deleteShaderProgram(int id)
    {
        // détacher et supprimer tous ses shaders

        // supprimer le programme
        glDeleteProgram(id);
    }


    /**
     * cette fonction crée un VBO contenant des float
     * @param values : tableau de float à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    public static int makeFloatVBO(Collection<Float> values, int vbo_type, int usage)
    {
        // mettre les données dans un Buffer android
        int size = values.size() * Float.SIZE/8;
        ByteBuffer bb = ByteBuffer.allocateDirect(size);
        bb.order(ByteOrder.nativeOrder());
        FloatBuffer buffer = bb.asFloatBuffer();
        for (Float val: values) {
            buffer.put(val);
        }
        buffer.position(0);

        // créer un VBO et le remplir avec les données
        int[] ids = new int[1];
        glGenBuffers(1, ids, 0);
        glBindBuffer(vbo_type, ids[0]);
        glBufferData(vbo_type, size, buffer, usage);
        glBindBuffer(vbo_type, 0);

        return ids[0];
    }


    /**
     * cette fonction crée un VBO contenant des float
     * @param values : tableau de float à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    public static int makeFloatVBO(float[] values, int vbo_type, int usage)
    {
        // mettre les données dans un Buffer android
        int size = values.length * Float.SIZE/8;
        ByteBuffer bb = ByteBuffer.allocateDirect(size);
        bb.order(ByteOrder.nativeOrder());
        FloatBuffer buffer = bb.asFloatBuffer();
        buffer.put(values);
        buffer.position(0);

        // créer un VBO et le remplir avec les données
        int[] ids = new int[1];
        glGenBuffers(1, ids, 0);
        glBindBuffer(vbo_type, ids[0]);
        glBufferData(vbo_type, size, buffer, usage);
        glBindBuffer(vbo_type, 0);

        return ids[0];
    }


    /**
     * cette fonction crée un VBO contenant des short
     * @param values : tableau de short à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    public static int makeShortVBO(Collection<Short> values, int vbo_type, int usage)
    {
        // mettre les données dans un Buffer android
        int size = values.size() * Short.SIZE/8;
        ByteBuffer bb = ByteBuffer.allocateDirect(size);
        bb.order(ByteOrder.nativeOrder());
        ShortBuffer buffer = bb.asShortBuffer();
        for (Short val: values) {
            buffer.put(val);
        }
        buffer.position(0);

        // créer un VBO et le remplir avec les données
        int[] ids = new int[1];
        glGenBuffers(1, ids, 0);
        glBindBuffer(vbo_type, ids[0]);
        glBufferData(vbo_type, size, buffer, usage);
        glBindBuffer(vbo_type, 0);

        return ids[0];
    }


    /**
     * cette fonction crée un VBO contenant des short
     * @param values : tableau de short à mettre dans le VBO
     * @param vbo_type : type OpenGL du VBO, par exemple GL_ELEMENT_ARRAY_BUFFER
     * @param usage : type de stockage OpenGL des données, par exemple GL_STATIC_DRAW
     * @return identifiant OpenGL du VBO
     */
    public static int makeShortVBO(short[] values, int vbo_type, int usage)
    {
        // mettre les données dans un Buffer android
        int size = values.length * Short.SIZE/8;
        ByteBuffer bb = ByteBuffer.allocateDirect(size);
        bb.order(ByteOrder.nativeOrder());
        ShortBuffer buffer = bb.asShortBuffer();
        buffer.put(values);
        buffer.position(0);

        // créer un VBO et le remplir avec les données
        int[] ids = new int[1];
        glGenBuffers(1, ids, 0);
        glBindBuffer(vbo_type, ids[0]);
        glBufferData(vbo_type, size, buffer, usage);
        glBindBuffer(vbo_type, 0);

        return ids[0];
    }


    /**
     * supprime un buffer VBO dont on fournit l'identifiant
     * @param id : identifiant du VBO
     */
    public static void deleteVBO(int id)
    {
        int[] buffer = new int[1];
        buffer[0] = id;
        glDeleteBuffers(1, buffer, 0);
    }


    /**
     * convertit en radians l'angle fourni en degrés
     * @param deg : angle en degrés
     * @return angle converti en radians
     */
    public static float radians(float deg)
    {
        return (float) (deg * Math.PI / 180.0);
    }


    /**
     * convertit en radians l'angle fourni en degrés
     * @param deg : angle en degrés
     * @return angle converti en radians
     */
    public static float radians(double deg)
    {
        return (float) (deg * Math.PI / 180.0);
    }


    /**
     * convertit en degrés l'angle fourni en radians
     * @param rad : angle en radians
     * @return angle converti en degrés
     */
    public static float degrees(float rad)
    {
        return (float) (rad * 180.0 / Math.PI);
    }


    /**
     * convertit en degrés l'angle fourni en radians
     * @param rad : angle en radians
     * @return angle converti en degrés
     */
    public static float degrees(double rad)
    {
        return (float) (rad * 180.0 / Math.PI);
    }


    /**
     * cette fonction convertit un vec3(teinte, saturation, valeur) en (rouge, vert, bleu)
     * NB : les composantes sont toutes comprises entre 0 et 1
     * @param hsv : vec3(h,s,v) contenant la teinte à convertir
     * @return vec3(r,v,b)
     */
    public static vec3 hsv2rgb(vec3 hsv)
    {
        float h = (float) ((hsv.get(0) - Math.floor(hsv.get(0))) * 6.0f);
        float s = hsv.get(1);
        float v = hsv.get(2);

        if (s <= 0.0) return vec3.fromValues(v,v,v);

        int i = (int) Math.floor(h);
        float ff = h - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - (s * ff));
        float t = v * (1.0f - (s * (1.0f - ff)));

        switch(i) {
        case 0:
            return vec3.fromValues(v,t,p);
        case 1:
            return vec3.fromValues(q,v,p);
        case 2:
            return vec3.fromValues(p,v,t);
        case 3:
            return vec3.fromValues(p,q,v);
        case 4:
            return vec3.fromValues(t,p,v);
        case 5:
        default:
            return vec3.fromValues(v,p,q);
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
    public static float clamp(float value, float min, float max)
    {
        if (value <= min) return min;
        if (value >= max) return max;
        return value;
    }
    public static int clamp(int value, int min, int max)
    {
        if (value <= min) return min;
        if (value >= max) return max;
        return value;
    }
}
