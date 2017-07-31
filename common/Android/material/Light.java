/**
 * Définition de la classe Light, une base pour construire des éclairages
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import java.nio.IntBuffer;

import com.dbooker.livreopengl.misc.SceneBase;
import com.dbooker.livreopengl.misc.SceneElement.Drawable;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.process.FrameBufferObject;
import com.dbooker.livreopengl.process.Process;



@SuppressWarnings("unused")
public class Light extends Process implements Drawable
{

    /** définitions spécifiques à ce type de lampe **/
    protected vec3 m_Color;

    /** emplacement des uniform communs à toutes les lampes **/
    protected int m_MapPositionLoc;
    protected int m_MapNormalLoc;
    protected int m_MapDiffuseLoc;
    protected int m_MapSpecularLoc;
    protected int m_MapDepthLoc;
    protected int m_LightColorLoc;


    /**
     * constructeur
     * C'est une lampe ambiante toute simple, sans ombre
     * @throws Exception
     */
    public Light() throws Exception
    {
        // constructeur de la superclasse
        super("Light");

        // initialisation des variables membre spécifiques
        m_Color = vec3.fromValues(1,1,1);

        // compiler le shader
        compileShader();
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * Cette méthode définit la couleur de la lampe
     * @param color : vec3 donnant la couleur (intensité) de la lampe
     */
    public void setColor(vec3 color)
    {
        vec3.copy(m_Color, color);
    }


    /**
     * retourne la couleur de la lampe
     */
    public vec3 getColor()
    {
        return m_Color;
    }


    /**
     * indique si cette lampe a une position (true) ou est ambiante (false)
     * @return false pour une lampe ambiante
     */
    public boolean hasPosition()
    {
        return false;
    }


    /**
     * retourne la position de la lampe dans le repère de la caméra
     */
    public vec4 getPositionCamera()
    {
        return vec4.create();
    }


    /**
     * applique une matrice sur la position et la cible de la lampe
     * @param matrix : mat4
     */
    public void transform(mat4 matrix)
    {
    }


    /**
     * dessine la scène dans la shadowmap de cette lampe
     * NB : Cette méthode ne fait rien dans le cas d'une lampe abstraite
     * @param scene à dessiner (on va appeler sa méthode onDraw, vue de la lampe)
     * @param mat4ViewScene : matrice de transformation dans laquelle sont dessinés les objets
     */
    public void makeShadowMap(SceneBase scene, mat4 mat4ViewScene)
    {
    }


    /**
     * "dessine" cette lampe dans le cas où elle est un SceneElement d'une scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    @Override
    public void onDraw(mat4 mat4Projection, mat4 mat4View)
    {
    }


    /**
     * construit le Fragment Shader qui calcule l'éclairement de cette lampe
     */
    @Override
    protected String getFragmentShader()
    {
        String srcFragmentShader =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "in vec2 frgTexCoord;\n" +
            "\n" +
            "uniform sampler2D MapDiffuse;\n" +
            "uniform sampler2D MapPosition;\n" +
            "uniform sampler2D MapDepth;\n" +
            "uniform vec3 LightColor;\n" +
            "\n" +
            "out vec4 glFragColor;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "    vec4 position = texture(MapPosition, frgTexCoord);\n" +
            "    if (position.w != 1.0) discard;\n" +
            "    //gl_FragDepth = texture(MapDepth, frgTexCoord).r;\n" +
            "    vec4 color = texture(MapDiffuse, frgTexCoord);\n" +
            "    glFragColor = vec4(color.rgb * LightColor, 1.0) * color.a;\n" +
            "}";
        return srcFragmentShader;
    }


    /**
     * détermine où sont les variables uniform spécifiques de cette lampe
     */
    @Override
    protected void findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

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
    protected void startProcess(FrameBufferObject gbuffer)
    {
        // appeler la méthode de la superclasse
        super.startProcess();

        // fournir la couleur de la lampe
        vec3.glUniform(m_LightColorLoc, m_Color);

        // fournir les buffers du FBO MRT
        setTextureUnit(GL_TEXTURE0, m_MapDiffuseLoc,  gbuffer.getColorBuffer(0));
        setTextureUnit(GL_TEXTURE1, m_MapSpecularLoc, gbuffer.getColorBuffer(1));
        setTextureUnit(GL_TEXTURE2, m_MapPositionLoc, gbuffer.getColorBuffer(2));
        setTextureUnit(GL_TEXTURE3, m_MapNormalLoc,  gbuffer.getColorBuffer(3));
        setTextureUnit(GL_TEXTURE4, m_MapDepthLoc,    gbuffer.getDepthBuffer());
    }


    /**
     * applique l'éclairement défini par cette lampe
     * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
     */
    public void process(FrameBufferObject gbuffer)
    {
        // préparer le shader pour le traitement
        startProcess(gbuffer);

        // mettre le depth test en mode <=
        IntBuffer tmp = IntBuffer.allocate(1);
        glGetIntegerv(GL_DEPTH_FUNC, tmp);
        int depth_func = tmp.get(0);
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
    @Override
    protected void endProcess()
    {
        // désactiver les textures
        setTextureUnit(GL_TEXTURE0);
        setTextureUnit(GL_TEXTURE1);
        setTextureUnit(GL_TEXTURE2);
        setTextureUnit(GL_TEXTURE3);
        setTextureUnit(GL_TEXTURE4);

        // appeler la méthode de la superclasse
        super.endProcess();
    }
}
