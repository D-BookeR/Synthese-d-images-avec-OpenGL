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
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;


@SuppressLint("ClickableViewAccessibility")
public class OpenGLSurfaceView extends GLSurfaceView implements OnClickListener, OnTouchListener
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

    private enum MouseMode {
        NORMAL, ZOOM, PAN
    }

    private MouseMode mMode = MouseMode.NORMAL;
    private float oldX, oldY;


    @Override
    public boolean onTouch(View view, MotionEvent event)
    {
        switch (event.getAction()) {

        case MotionEvent.ACTION_DOWN:
            switch (view.getId()) {
            case R.id.zoom:
                mMode = MouseMode.ZOOM;
                view.setPressed(true);
                break;
            case R.id.pan:
                mMode = MouseMode.PAN;
                view.setPressed(true);
                break;
            }
            break;

        case MotionEvent.ACTION_UP:
            mMode = MouseMode.NORMAL;
            view.setPressed(false);
            break;
        }
        return true;
    }


    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        float x = event.getX();
        float y = event.getY();

        switch (event.getAction()) {

        case MotionEvent.ACTION_DOWN:
            // toucher de l'écran
            oldX = x;
            oldY = y;
            break;

        case MotionEvent.ACTION_MOVE:
            // déplacement du doigt sur l'écran
            switch (mMode) {
            case NORMAL:
                mRenderer.onRotate(oldX, oldY, x, y, getWidth(), getHeight());
                break;
            case PAN:
                mRenderer.onPan(x-oldX, oldY-y);
                break;
            case ZOOM:
                mRenderer.onZoom((x-oldX) - (y-oldY));
                break;
            default:
                break;
            }
            requestRender();
            oldX = x;
            oldY = y;
            break;

        case MotionEvent.ACTION_UP:
            break;
        }
        return true;
    }


    @Override
    public void onClick(View v)
    {
        switch (v.getId()) {
        case R.id.DisplayTriangles: mRenderer.onKeyDown('T'); break;
        case R.id.DisplayAretes:    mRenderer.onKeyDown('Y'); break;
        case R.id.DisplayNormales:  mRenderer.onKeyDown('U'); break;
        }
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

        void onZoom(float dz)
        {
            if (scene != null) scene.onZoom(dz);
        }

        void onPan(float dx, float dy)
        {
            if (scene != null) scene.onPan(dx, dy);
        }

        void onRotate(float oldX, float oldY, float x, float y, int width, int height)
        {
            if (scene != null) scene.onRotate(oldX, oldY, x, y, width, height);
        }

        void onKeyDown(char c)
        {
            if (scene != null) scene.onKeyDown(c);
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
