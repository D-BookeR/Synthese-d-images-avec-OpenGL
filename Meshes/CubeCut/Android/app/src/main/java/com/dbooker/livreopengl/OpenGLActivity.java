package com.dbooker.livreopengl;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.os.Bundle;
import android.util.Log;

import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.utils.ApplicationContext;


public class OpenGLActivity extends Activity
{
    private static final String TAG = "LivreOpenGL";

    private OpenGLSurfaceView openglsurfaceview;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // test de la version OpenGL ES
        final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
        if (configurationInfo.reqGlEsVersion < 0x030000) {
            // la version d'OpenGL ES n'est pas suffisante pour faire fonctionner les exemples
            String version = configurationInfo.getGlEsVersion();
            Log.w(TAG, "This device has only OpenGL ES "+version+" but this example needs at least OpenGL ES 3.0 for fully functioning");
        }

        // mémoriser le contexte afin de pouvoir l'utiliser dans les classes Mesh et Material
        ApplicationContext.setContext(getApplicationContext());

        // mettre en place l'interface
        setContentView(R.layout.main);

        // récupérer l'objet correspondant à la vue OpenGL
        openglsurfaceview = (OpenGLSurfaceView) findViewById(R.id.openglsurfaceview);

        // associer la vue OpenGL aux touchers sur les boutons de l'interface
        findViewById(R.id.zoom).  setOnTouchListener(openglsurfaceview);
        findViewById(R.id.pan).   setOnTouchListener(openglsurfaceview);
        findViewById(R.id.DisplayTriangles).setOnClickListener(openglsurfaceview);
        findViewById(R.id.DisplayAretes).   setOnClickListener(openglsurfaceview);
        findViewById(R.id.DisplayNormales). setOnClickListener(openglsurfaceview);
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
