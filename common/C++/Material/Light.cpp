/**
 * Définition de la classe Light, une base pour construire des éclairages
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <gl-matrix.h>
#include <utils.h>
#include <Light.h>


/**
 * constructeur
 * C'est une lampe ambiante toute simple, sans ombre
 */
Light::Light():
    Process("Light")
{
    // initialisation des variables membre spécifiques
    m_Color = vec3::fromValues(1,1,1);

    // compiler le shader
    compileShader();
}


    /** destructeur */
Light::~Light()
{
}


/**
 * Cette méthode définit la couleur de la lampe
 * @param color : vec3 donnant la couleur (intensité) de la lampe
 */
void Light::setColor(vec3 color)
{
    vec3::copy(m_Color, color);
}


/**
 * retourne la couleur de la lampe
 */
vec3 Light::getColor()
{
    return m_Color;
}


/**
 * indique si cette lampe a une position (true) ou est ambiante (false)
 * @return false pour une lampe ambiante
 */
bool Light::hasPosition()
{
    return false;
}


/**
 * retourne la position de la lampe dans le repère de la caméra
 */
vec4 Light::getPositionCamera()
{
    return vec4::create();
}


/**
 * applique une matrice sur la position et la cible de la lampe
 * @param matrix : mat4
 */
void Light::transform(mat4& matrix)
{
}


/**
 * dessine la scène dans la shadowmap de cette lampe
 * NB : Cette méthode ne fait rien dans le cas d'une lampe abstraite
 * @param scene à dessiner (on va appeler sa méthode onDraw, vue de la lampe)
 * @param mat4ViewScene : matrice de transformation dans laquelle sont dessinés les objets
 */
void Light::makeShadowMap(SceneBase* scene, mat4& mat4ViewScene)
{
}


/**
 * "dessine" cette lampe dans le cas où elle est un SceneElement d'une scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Light::onDraw(mat4& mat4Projection, mat4& mat4View)
{
}


/**
 * construit le Fragment Shader qui calcule l'éclairement de cette lampe
 */
std::string Light::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "uniform sampler2D MapDiffuse;\n"
        "uniform sampler2D MapPosition;\n"
        "uniform sampler2D MapDepth;\n"
        "uniform vec3 LightColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 position = texture(MapPosition, frgTexCoord);\n"
        "    if (position.w != 1.0) discard;\n"
        "    gl_FragDepth = texture(MapDepth, frgTexCoord).r;\n"
        "    vec4 color = texture(MapDiffuse, frgTexCoord);\n"
        "    glFragColor = vec4(color.rgb * LightColor, 1.0) * color.a;\n"
        "}";
    return srcFragmentShader;
}


/**
 * détermine où sont les variables uniform spécifiques de cette lampe
 */
void Light::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // textures du FBO MRT (toutes ne sont pas forcément utilisées, c'est selon la lampe)
    m_MapDiffuseLoc  = glGetUniformLocation(m_ShaderId, "MapDiffuse");
    m_MapSpecularLoc = glGetUniformLocation(m_ShaderId, "MapSpecular");
    m_MapPositionLoc = glGetUniformLocation(m_ShaderId, "MapPosition");
    m_MapNormalLoc  = glGetUniformLocation(m_ShaderId, "MapNormale");
    m_MapDepthLoc    = glGetUniformLocation(m_ShaderId, "MapDepth");

    // emplacement de la couleur de la lampe
    m_LightColorLoc = glGetUniformLocation(m_ShaderId, "LightColor");
}


/**
 * active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe
 * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
 */
void Light::startProcess(FrameBufferObject* gbuffer)
{
    // appeler la méthode de la superclasse
    Process::startProcess();

    // fournir la couleur de la lampe
    vec3::glUniform(m_LightColorLoc, m_Color);

    // fournir les buffers du FBO MRT
    setTextureUnit(GL_TEXTURE0, m_MapDiffuseLoc,  gbuffer->getColorBuffer(0));
    setTextureUnit(GL_TEXTURE1, m_MapSpecularLoc, gbuffer->getColorBuffer(1));
    setTextureUnit(GL_TEXTURE2, m_MapPositionLoc, gbuffer->getColorBuffer(2));
    setTextureUnit(GL_TEXTURE3, m_MapNormalLoc,  gbuffer->getColorBuffer(3));
    setTextureUnit(GL_TEXTURE4, m_MapDepthLoc,    gbuffer->getDepthBuffer());
}


/**
 * applique l'éclairement défini par cette lampe
 * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
 */
void Light::process(FrameBufferObject* gbuffer)
{
    // préparer le shader pour le traitement
    startProcess(gbuffer);

    // mettre le depth test en mode <=
    GLint depth_func;
    glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
    glDepthFunc(GL_LEQUAL);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // remettre comme c'était
    glDepthFunc(depth_func);

    // libérer les ressources
    endProcess();
}


/**
 * désactive shader, VBO et textures
 */
void Light::endProcess()
{
    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);
    setTextureUnit(GL_TEXTURE1);
    setTextureUnit(GL_TEXTURE2);
    setTextureUnit(GL_TEXTURE3);
    setTextureUnit(GL_TEXTURE4);

    // appeler la méthode de la superclasse
    Process::endProcess();
}
