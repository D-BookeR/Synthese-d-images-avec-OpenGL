package com.dbooker.livreopengl;

import android.app.Activity;
import android.os.Bundle;

import com.dbooker.livreopengl.utils.ApplicationContext;


public class OpenGLActivity extends Activity
{
    private OpenGLSurfaceView openglsurfaceview;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // init context
        ApplicationContext.setContext(getApplicationContext());

        setContentView(R.layout.main);

        // récupérer l'objet correspondant à la vue OpenGL
        openglsurfaceview = (OpenGLSurfaceView) findViewById(R.id.openglsurfaceview);

        // associer la vue OpenGL aux touchers sur les boutons de l'interface
        findViewById(R.id.zoom).setOnTouchListener(openglsurfaceview);
        findViewById(R.id.pan).setOnTouchListener(openglsurfaceview);
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
}
