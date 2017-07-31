package com.dbooker.livreopengl.utils;

import android.content.Context;


/**
 * Cette classe permet de stocker des informations uniques qui soient accessibles de n'importe où
 * - un lien sur le contexte
 *
 * source : http://stackoverflow.com/questions/14057273/android-singleton-with-global-context
 *
 */
@SuppressWarnings("unused")
public class ApplicationContext
{

    private Context m_Context = null;

    private ApplicationContext()
    {
    }

    public static Context getContext()
    {
        return getInstance().m_Context;
    }

    public static void setContext(Context context)
    {
        getInstance().m_Context = context;
    }


    /* création d'une seule instance d'ApplicationContext */

    private static ApplicationContext instance = null;

    private static ApplicationContext getInstance()
    {
        if (instance == null) instance = new ApplicationContext();
        return instance;
    }
}
