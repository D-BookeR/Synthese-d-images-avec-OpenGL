package com.dbooker.livreopengl;

import android.app.Activity;
import android.os.Bundle;

import com.dbooker.livreopengl.utils.ApplicationContext;


public class OpenGLActivity extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // init context
        ApplicationContext.setContext(getApplicationContext());

        setContentView(R.layout.main);
    }
}
