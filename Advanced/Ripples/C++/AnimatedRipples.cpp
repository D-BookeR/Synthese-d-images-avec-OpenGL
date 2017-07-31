// Cette classe permet d'appliquer une déformation type tremblement sur un FBO

#include <iostream>
#include <math.h>

#include <utils.h>


#include <AnimatedRipples.h>


/**
 * Constructeur
 */
AnimatedRipples::AnimatedRipples():
    Process("AnimatedRipples")
{
    // charger la texture de distorsion
    m_Texture = new Texture2D("data/textures/distorsion.png", GL_LINEAR, GL_REPEAT);

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string AnimatedRipples::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D ColorMap;\n"
        "uniform sampler2D DistorsionMap;\n"
        "\n"
        "uniform float time;\n"
        "uniform float strength;\n"
        "uniform float speed;\n"
        "\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // coordonnées dans la texture de distorsion\n"
        "    vec2 texCoords = frgTexCoord + fract(vec2(0.0, time * speed));\n"
        "\n"
        "    // accès à la texture, elle retourne un décalage\n"
        "    vec2 offset = texture(DistorsionMap, texCoords).xy * 2.0 - 1.0;\n"
        "\n"
        "    // conditionnement du décalage\n"
        "    offset *= strength;\n"
        "\n"
        "    // coordonnées dans l'image à déformer\n"
        "    texCoords = frgTexCoord + offset;\n"
        "\n"
        "    // accès à l'image déformée\n"
        "    glFragColor = texture(ColorMap, texCoords);\n"
        "}";
}


/**
 * détermine où sont les variables uniform spécifiques de ce traitement
 */
void AnimatedRipples::findUniformLocations()
{
    // appeler la méthode de la superclasse
    Process::findUniformLocations();

    // déterminer où sont les variables uniform spécifiques
    m_DistorsionMapLoc = glGetUniformLocation(m_ShaderId, "DistorsionMap");
    m_StrengthLoc      = glGetUniformLocation(m_ShaderId, "strength");
    m_SpeedLoc         = glGetUniformLocation(m_ShaderId, "speed");
    m_TimeLoc          = glGetUniformLocation(m_ShaderId, "time");
}


/**
 * applique une distorsion à l'image du FBO
 * @param fbo : FBO contenant l'image à traiter
 * @param strength : importance de l'effet
 * @param speed : vitesse de l'effet
 */
void AnimatedRipples::process(FrameBufferObject* fbo, float strength, float speed)
{
    // préparer le shader pour le traitement
    startProcess();

    // fournir le color buffer du FBO à traiter
    setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo->getColorBuffer());
    m_Texture->setTextureUnit(GL_TEXTURE1, m_DistorsionMapLoc);

    // fournir les paramètres du traitement
    glUniform1f(m_StrengthLoc, strength);
    glUniform1f(m_SpeedLoc,    speed);
    glUniform1f(m_TimeLoc,     Utils::Time);

    // dessiner un quadrilatère avec les quatre vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // désactiver les textures
    setTextureUnit(GL_TEXTURE0);
    setTextureUnit(GL_TEXTURE1);

    // libérer les ressources
    endProcess();
}


/**
 * Cette méthode supprime les ressources allouées
 */
AnimatedRipples::~AnimatedRipples()
{
    delete m_Texture;
}
