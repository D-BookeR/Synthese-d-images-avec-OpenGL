/**
 * Cette classe permet de dessiner l'arrière plan uniforme en mode Deferred Shading
 * Le shader colore les zones non dessinées du FBO mrt (la coordonnée w des fragments vaut 0)
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>

#include <SkyBackground.h>


/**
 * constructeur
 */
SkyBackground::SkyBackground() :
    Light()
{
    // récupérer la couleur d'effacement
    GLfloat color[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
    m_Color = vec3::fromValues(color[0], color[1], color[2]);

    // compiler le shader
    compileShader();
}


/** destructeur */
SkyBackground::~SkyBackground()
{
}


/**
 * construit le Fragment Shader qui colore les zones non dessinées du FBO mrt
 */
std::string SkyBackground::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D MapPosition;\n"
        "in vec2 frgTexCoord;\n"
        "uniform vec3 LightColor;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 position = texture(MapPosition, frgTexCoord);\n"
        "    if (position.w > 0.8) discard;\n"
        "    gl_FragDepth = 1.0;\n"
        "    glFragColor = vec4(LightColor, 1.0);\n"
        "}";
}
