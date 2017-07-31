/**
 * cette classe lance le chargement d'une image et en fait une texture 2D
 * NB: le chargement est fait par une requête au serveur, donc peut prendre du temps
 * pendant lequel la texture n'est pas utilisable
 * @note il se peut qu'il y ait une erreur "It is forbidden to load a WebGL texture from a cross-domain element that has not been validated with CORS. See https://developer.mozilla.org/en/WebGL/Cross-Domain_Textures"
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import java.io.IOException;
import java.io.InputStream;
import java.nio.IntBuffer;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

import com.dbooker.livreopengl.utils.ApplicationContext;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.opengl.GLUtils;
import android.text.TextUtils;

@SuppressWarnings("unused")
public class Texture2D
{
    // informations sur la texture
    protected int m_TextureID;                // numéro d'identification de OpenGL
    protected int m_Width, m_Height;       // dimensions

    /**
     * le constructeur lance le chargement d'une image et en fait une texture 2D
     * @param filename : nom du fichier contenant l'image à charger
     * @param filtering : mettre GL_LINEAR ou GL_NEAREST ou GL_LINEAR_MIPMAP_LINEAR (mipmaps)
     * @param repetition : mettre GL_CLAMP_TO_EDGE ou GL_REPEAT
     */
    public Texture2D(String filename, int filtering, int repetition) throws IOException
    {
        init(filename, filtering, repetition);
    }
    public Texture2D(String filename, int filtering) throws IOException
    {
        init(filename, filtering, GL_CLAMP_TO_EDGE);
    }
    public Texture2D(String filename) throws IOException
    {
        init(filename, GL_LINEAR, GL_CLAMP_TO_EDGE);
    }

    private void init(String filename, int filtering, int repetition) throws IOException
    {
        final int[] textureHandle = new int[1];
        glGenTextures(1, textureHandle, 0);
        if (textureHandle[0] == 0) {
            throw new RuntimeException("Error creating texture "+filename);
        }
        m_TextureID = textureHandle[0];

        //final BitmapFactory.Options options = new BitmapFactory.Options();
        //options.inScaled = false;   // No pre-scaling

        // enlever les ".." du chemin
        List<String> dirname = new LinkedList<>();
        for (String item: Arrays.asList(filename.split("/"))) {
            if (item.equals("..")) {
                dirname.remove(dirname.size()-1);
            } else {
                dirname.add(dirname.size(), item);
            }
        }
        filename = TextUtils.join("/", dirname);

        // lire le fichier image
        Context context = ApplicationContext.getContext();
        InputStream data = context.getAssets().open(filename);
        final Bitmap temp = BitmapFactory.decodeStream(data);
        if (temp == null) {
            throw new RuntimeException("Error loading texture "+filename);
        }

        // dimensions de la texture
        m_Width = temp.getWidth();
        m_Height = temp.getHeight();

        // inverser la texture verticalement
        Matrix flip = new Matrix();
        flip.postScale(1, -1);
        final Bitmap bitmap = Bitmap.createBitmap(temp, 0, 0, m_Width, m_Height, flip, true);

        // activer la texture
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        // filtrage avec mipmaps ?
        if (filtering == GL_NEAREST_MIPMAP_NEAREST || filtering == GL_LINEAR_MIPMAP_NEAREST ||
            filtering == GL_NEAREST_MIPMAP_LINEAR  || filtering == GL_LINEAR_MIPMAP_LINEAR) {
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            // filtering antialiasing de la texture sans mipmaps
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
        }

        // mode de répétition de la texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repetition);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repetition);

        // réglage de OpenGL pour avoir un bon rendu des textures
        //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        // envoyer le bitmap dans OpenGL
        GLUtils.texImage2D(GL_TEXTURE_2D, 0, bitmap, 0);

        // recycler le bitmap puisque son contenu est dans OpenGL
        bitmap.recycle();
    }


    /**
     * cette fonction associe cette texture à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
     */
    public void setTextureUnit(int unit, int locSampler)
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


    /**
     * cette fonction désactive une texture à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     */
    public void setTextureUnit(int unit)
    {
        // si la texture n'est pas bien initialisée
        if (m_TextureID == 0) return;

        // activer l'unité de texture
        glActiveTexture(unit);

        // délier à la texture
        glBindTexture(GL_TEXTURE_2D, 0);
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
