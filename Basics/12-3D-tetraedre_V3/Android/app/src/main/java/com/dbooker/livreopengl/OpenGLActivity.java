package com.dbooker.livreopengl;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.dbooker.livreopengl.utils.ApplicationContext;
import com.dbooker.livreopengl.utils.Utils;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;


public class OpenGLActivity extends Activity
{
    private static final String TAG = "LivreOpenGL";

    private OpenGLSurfaceView openglsurfaceview;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // init context
        ApplicationContext.setContext(getApplicationContext());

        // Create a GLSurfaceView instance and set it
        // as the ContentView for this Activity.
        openglsurfaceview = new OpenGLSurfaceView(this);
        setContentView(openglsurfaceview);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        openglsurfaceview.onPause();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        openglsurfaceview.onResume();
    }

    /** Cette classe représente la vue OpenGL de l'activité */
    private class OpenGLSurfaceView extends GLSurfaceView
    {
        public OpenGLSurfaceView(Context context)
        {
            super(context);

            // créer un "contexte" OpenGL ES 2.0
            setEGLContextClientVersion(2);

            // configuration des buffers graphiques
            int redSize     =  8;
            int greenSize   =  8;
            int blueSize    =  8;
            int alphaSize   =  8;
            int depthSize   = 16;
            int stencilSize =  0;
            setEGLConfigChooser(redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize);

            // définir le renderer OpenGL pour la GLSurfaceView
            setRenderer(new OpenGLRenderer());

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
                }
            }

            /** appelée à chaque fois qu'il faut redessiner la vue OpenGL */
            @Override
            public void onDrawFrame(GL10 gl10)
            {
                // mettre à jour le temps global
                Utils.Time = (float)(SystemClock.uptimeMillis() - m_StartTime) / 1000.0f;

                // dessiner la scène
                if (scene != null) scene.onDrawFrame();
            }

            /** appelée quand la zone de dessin change de taille */
            @Override
            public void onSurfaceChanged(GL10 gl10, int width, int height)
            {
                if (scene != null) scene.onSurfaceChanged(width, height);
            }
        }
    }
}
