package com.dbooker.livreopengl;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.dbooker.livreopengl.utils.Utils;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.util.Log;


@SuppressLint("ClickableViewAccessibility")
public class OpenGLSurfaceView extends GLSurfaceView
{
    private static final String TAG = "LivreOpenGL";

    private OpenGLRenderer mRenderer;

    public OpenGLSurfaceView(Context context, AttributeSet attrs)
    {
        super(context, attrs);

        // créer un "contexte" OpenGL ES 3.0 ou 2.0 si c'est pas possible
        final ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
        if (configurationInfo.reqGlEsVersion >= 0x030000) {
            setEGLContextClientVersion(3);
        } else {
            setEGLContextClientVersion(2);
        }

        // renderer OpenGL
        mRenderer = new OpenGLRenderer();

        // configuration des buffers graphiques
        int redSize     =  8;
        int greenSize   =  8;
        int blueSize    =  8;
        int alphaSize   =  8;
        int depthSize   = 16;
        int stencilSize =  0;
        setEGLConfigChooser(redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize);

        // définir le rendu OpenGL pour la GLSurfaceView
        setRenderer(mRenderer);

        // demander que le dessin soit fait en permanence ou seulement quand nécessaire
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        //setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    /** Cette classe gère la "vie" de la vue OpenGL */
    private class OpenGLRenderer implements GLSurfaceView.Renderer
    {
        private Scene scene = null;
        long m_StartTime = 0;

        /** appelée quand on crée la zone de dessin OpenGL */
        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig config)
        {
            // créer la scène : allouer et remplir les buffers
            try {
                scene = new Scene();
                m_StartTime = SystemClock.uptimeMillis();
            } catch (Exception e) {
                Log.e(TAG, "Unable to create Scene : "+e.getMessage());
                e.printStackTrace();
            }
        }

        /** appelée à chaque fois qu'il faut redessiner la vue OpenGL */
        @Override
        public void onDrawFrame(GL10 gl10)
        {
            // mettre à jour le temps global
            Utils.Time = (float)(SystemClock.uptimeMillis() - m_StartTime) / 1000.0f;

            // dessiner la scène
            if (scene != null) try {
                scene.onDrawFrame();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        /** appelée quand la zone de dessin change de taille */
        @Override
        public void onSurfaceChanged(GL10 gl10, int width, int height)
        {
            if (scene != null) scene.onSurfaceChanged(width, height);
        }
    }
}
