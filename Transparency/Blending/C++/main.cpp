#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <SDL_image.h>

#include <iostream>
#include <stdlib.h>

#include <utils.h>
#include "Scene.h"


/**
 * Scène à dessiner
 * NB: son constructeur doit être appelé après avoir initialisé OpenGL
 **/
Scene* scene = nullptr;

/**
 * Callback pour GLFW : prendre en compte la taille de la vue OpenGL
 **/
static void onSurfaceChanged(GLFWwindow* window, int width, int height)
{
    if (scene == nullptr) return;
    scene->onSurfaceChanged(width, height);
}

/**
 * Callback pour GLFW : redessiner la vue OpenGL
 **/
static void onDrawRequest(GLFWwindow* window)
{
    if (scene == nullptr) return;
    Utils::UpdateTime();
    scene->onDrawFrame();
    static bool premiere = true;
    if (premiere) {
        // copie écran automatique
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        Utils::ScreenShotPPM("image.ppm", width, height);
        premiere = false;
    }

    // afficher le back buffer
    glfwSwapBuffers(window);
}

enum MouseMode {
    NONE, NORMAL, ZOOM, PAN
};

static MouseMode mMode = MouseMode::NONE;
static double oldX, oldY;


static void onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (scene == nullptr) return;
    if (action == GLFW_PRESS) {
        glfwGetCursorPos(window, &oldX, &oldY);
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            mMode = MouseMode::NORMAL;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mMode = MouseMode::ZOOM;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mMode = MouseMode::PAN;
            break;
        }
    } else {
        mMode = MouseMode::NONE;
    }
}

static void onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (mMode == MouseMode::NONE) return;
    if (scene == nullptr) return;
    int width, height;
    switch (mMode) {
    case MouseMode::NORMAL:
        glfwGetWindowSize(window, &width, &height);
        scene->onRotate(oldX, oldY, xpos, ypos, width, height);
        break;
    case MouseMode::PAN:
        scene->onPan(xpos-oldX, oldY-ypos);
        break;
    case MouseMode::ZOOM:
        scene->onZoom((xpos-oldX) - (ypos-oldY));
        break;
    }
    oldX = xpos;
    oldY = ypos;
}

static void onKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) return;
    if (key == GLFW_KEY_SPACE) {
        // copie écran
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        Utils::ScreenShotPPM("image.ppm", width, height);
    } else {
        if (scene == nullptr) return;
        scene->onKeyDown(key);
    }
}


void onExit()
{
    // libération des ressources demandées par la scène
    if (scene != nullptr) delete scene;
    scene = nullptr;

    // terminaisons statiques
    FrameBufferObject::staticdestroy();
    Process::staticdestroy();

    // terminaison de SDL_Image
    IMG_Quit();

    // terminaison de GLFW
    glfwTerminate();

    // retour à la ligne final
    std::cout << std::endl;
}


/** appelée en cas d'erreur */
void error_callback(int error, const char* description)
{
    std::cerr << "GLFW error : " << description << std::endl;
}


/** point d'entrée du programme **/
int main(int argc,char **argv)
{
    // initialisation de GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwSetErrorCallback(error_callback);

    // caractéristiques de la fenêtre à ouvrir
    //glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    // initialisation de la fenêtre
    GLFWwindow* window = glfwCreateWindow(640,480, "Livre OpenGL", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 200, 200);
    glfwSetWindowTitle(window, "Transparency - Blending");

    // initialisation de glew
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Unable to initialize Glew : " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Initialisation de SDL_Image
    int ok = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    if (!ok) {
        std::cerr << "Unable to initialize SDL" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // pour spécifier ce qu'il faut impérativement faire à la sortie
    atexit(onExit);

    // initialisations statiques
    FrameBufferObject::staticinit();
    ShadowMap::staticinit();
    Process::staticinit();

    // création de la scène => création des objets...
    scene = new Scene();
    debugGLFatal("new Scene()");

    // enregistrement des fonctions callbacks
    glfwSetFramebufferSizeCallback(window, onSurfaceChanged);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, onKeyboard);

    // boucle principale
    try {
        onSurfaceChanged(window, 640,480);
        do {
            // dessiner
            onDrawRequest(window);
            // attendre les événements
            glfwPollEvents();
        } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));
    } catch (const std::invalid_argument& e) {
        std::cerr << "Something went wrong with " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
