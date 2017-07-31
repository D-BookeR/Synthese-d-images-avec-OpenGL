/**
 * cette classe effectue le chargement d'un groupe de 6 images et en fait une texture cube
 * NB: le chargement est fait par une requête au serveur, donc peut prendre du temps
 * pendant lequel la texture n'est pas utilisable. La texture n'est utilisable que lorsque
 * les 6 images ont toutes été chargées.
 * @note il se peut qu'il y ait une erreur "It is forbidden to load a WebGL texture from a cross-domain element that has not been validated with CORS. See https://developer.mozilla.org/en/WebGL/Cross-Domain_Textures"
 * @param chemin : chemin terminé par un / indiquant un répertoire dans lequel on trouve 6 images : {pos|neg}[xyz].jpg
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.GL_CLAMP_TO_EDGE;
import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TEXTURE7;
import static android.opengl.GLES20.GL_TEXTURE_CUBE_MAP;
import static android.opengl.GLES20.GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
import static android.opengl.GLES20.GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
import static android.opengl.GLES20.GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
import static android.opengl.GLES20.GL_TEXTURE_CUBE_MAP_POSITIVE_X;
import static android.opengl.GLES20.GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
import static android.opengl.GLES20.GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
import static android.opengl.GLES20.GL_TEXTURE_MAG_FILTER;
import static android.opengl.GLES20.GL_TEXTURE_MIN_FILTER;
import static android.opengl.GLES20.GL_TEXTURE_WRAP_S;
import static android.opengl.GLES20.GL_TEXTURE_WRAP_T;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glDeleteTextures;
import static android.opengl.GLES20.glGenTextures;
import static android.opengl.GLES20.glTexParameteri;
import static android.opengl.GLES20.glUniform1i;

import java.io.IOException;
import java.io.InputStream;
import java.nio.IntBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;

import com.dbooker.livreopengl.utils.ApplicationContext;


@SuppressWarnings("unused")
class TextureCube
{
    // informations sur la texture
    protected int m_TextureID;                // numéro d'identification de OpenGL
    protected int m_Width, m_Height;       // dimensions

    /**
     * le constructeur lance le chargement de 6 images et en fait une texture Cube
     * @param dirname : chemin terminé par un / indiquant un répertoire dans lequel on trouve 6 images : {pos|neg}[xyz].jpg
     * @param filtering : mettre GL_LINEAR ou GL_NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
     */
    public TextureCube(String dirname, int filtering) throws IOException
    {
        // créer et sélectionner la texture Cube
        final int[] textureHandle = new int[1];
        glGenTextures(1, textureHandle, 0);
        if (textureHandle[0] == 0) {
            throw new RuntimeException("Error creating texture cube "+dirname);
        }
        m_TextureID = textureHandle[0];

        // activer la texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

        // filtrage antialiasing de la texture
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filtering);

        // mode de répétition de la texture
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // réglage de OpenGL pour avoir un bon rendu des textures
        //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        // charger les 6 images, chacune sur une face
        loadImageFace(dirname+"posx.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        loadImageFace(dirname+"negx.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        loadImageFace(dirname+"posy.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        loadImageFace(dirname+"negy.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        loadImageFace(dirname+"posz.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        loadImageFace(dirname+"negz.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

        // libérer l'unité de texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }


    /**
     * Cette méthode charge l'image nomface associée à la face identifiée par idface
     * @param face_name : nom complet du fichier contenant l'image
     * @param idface : identifiant OpenGL de la face, ex : GL_TEXTURE_CUBE_MAP_POSITIVE_X
     * @throws IOException
     */
    private void loadImageFace(String face_name, int idface) throws IOException
    {
        //final BitmapFactory.Options options = new BitmapFactory.Options();
        //options.inScaled = false;   // No pre-scaling

        // Read in the resource
        Context context = ApplicationContext.getContext();
        InputStream data = context.getAssets().open(face_name);
        final Bitmap bitmap = BitmapFactory.decodeStream(data);
        if (bitmap == null) {
            throw new RuntimeException("TextureCube : unable to open "+face_name);
        }

        // dimensions de la texture
        m_Width = bitmap.getWidth();
        m_Height = bitmap.getHeight();

        // envoyer le bitmap dans OpenGL
        GLUtils.texImage2D(idface, 0, bitmap, 0);

        // recycler le bitmap puisque son contenu est dans OpenGL
        bitmap.recycle();
    }


    /**
     * cette fonction associe cette texture à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
     * @throws Exception
     */
    public void setTextureUnit(int unit, int locSampler) throws Exception
    {
        /*****DEBUG*****/
        if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
            throw new Exception("TextureCube.setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
        }
        /*****DEBUG*****/
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
    public void setTextureUnit(int unit)
    {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }


    /**
     * supprime cette texture
     */
    public void destroy()
    {
        if (m_TextureID != 0) {
            IntBuffer tmp = IntBuffer.allocate(1);
            tmp.put(m_TextureID);
            glDeleteTextures(1, tmp);
            m_TextureID = 0;
        }
    }
}
