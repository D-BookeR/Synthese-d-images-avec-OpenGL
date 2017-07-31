package com.dbooker.livreopengl.misc;

import static android.opengl.GLES20.*;

import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.utils.Utils;


/**
 * Cette classe permet de dessiner les trois axes X, Y et Z
 */
@SuppressWarnings("unused")
public class AxesXYZ
{
    /** caractéristiques des lignes */
    private float m_Alpha;
    private float m_Width;

    /** plan de coupe */
    private boolean m_ClipPlaneOn;
    private vec4 m_ClipPlane;

    /** tableau des coordonnées, x,y,z à la suite */
    private int m_VertexBufferId;
    private int m_ColorBufferId;

    /** identifiants liés au shader */
    private int m_ShaderId;
    private int m_VertexLoc;
    private int m_ColorLoc;
    private int m_MatPloc;
    private int m_MatMVloc;
    private int m_ClipPlaneLoc;



    /**
     * constructeur
     * @param size : longueur des branches
     * @param alpha : transparence du trièdre
     * @param width : largeur des lignes
     */
    public AxesXYZ(float size, float alpha, float width) throws Exception
    {
        // paramètres
        m_Alpha = alpha;
        m_Width = width;

        // plan de coupe par défaut à l'infini
        m_ClipPlaneOn = false;
        m_ClipPlane = vec4.fromValues(0,0,1,-1e38);

        // compiler le shader
        m_ShaderId = 0;
        compileShader();

        // créer et remplir le buffer des coordonnées et des couleurs
        ArrayList<Float> vertices = new ArrayList<>();
        ArrayList<Float> colors = new ArrayList<>();
        // axe X en rouge
        vertices.add(0.0f); vertices.add(0.0f); vertices.add(0.0f);
        colors.add(1.0f); colors.add(0.0f); colors.add(0.0f);
        vertices.add(size); vertices.add(0.0f); vertices.add(0.0f);
        colors.add(1.0f); colors.add(0.0f); colors.add(0.0f);
        // axe Y en vert
        vertices.add(0.0f); vertices.add(0.0f); vertices.add(0.0f);
        colors.add(0.0f); colors.add(1.0f); colors.add(0.0f);
        vertices.add(0.0f); vertices.add(size); vertices.add(0.0f);
        colors.add(0.0f); colors.add(1.0f); colors.add(0.0f);
        // axe Z en bleu
        vertices.add(0.0f); vertices.add(0.0f); vertices.add(0.0f);
        colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);
        vertices.add(0.0f); vertices.add(0.0f); vertices.add(size);
        colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);

        // créer les VBOs des coordonnées et des couleurs
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_ColorBufferId  = Utils.makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * retourne le source du Vertex Shader
     */
    private String getVertexShader()
    {
        String srcVertexShader = new String();
        srcVertexShader += "#version 310 es\n";
        srcVertexShader += "in vec3 glVertex;\n";
        srcVertexShader += "in vec3 glColor;\n";
        srcVertexShader += "out vec3 frgColor;\n";
        srcVertexShader += "out vec4 frgPosition;\n";
        srcVertexShader += "uniform mat4 mat4ModelView;\n";
        srcVertexShader += "uniform mat4 mat4Projection;\n";
        srcVertexShader += "void main()\n";
        srcVertexShader += "{\n";
        srcVertexShader += "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n";
        srcVertexShader += "    gl_Position = mat4Projection * frgPosition;\n";
        srcVertexShader += "    frgColor = glColor;\n";
        srcVertexShader += "}\n";
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    private String getFragmentShader()
    {
        String srcFragmentShader = new String();
        srcFragmentShader += "#version 310 es\n";
        srcFragmentShader += "precision mediump float;\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        if (m_ClipPlaneOn) {
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "in vec3 frgColor;\n";
        srcFragmentShader += "out vec4 glFragColor;\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (m_ClipPlaneOn) {
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        }
        srcFragmentShader += "    glFragColor = vec4(frgColor, " + m_Alpha + ");\n";
        srcFragmentShader += "}\n";
        return srcFragmentShader;
    }


    /**
     * recompile le shader
     * @throws Exception
     */
    private void compileShader() throws Exception
    {
        // supprimer l'ancien shader s'il y en avait un
        if (m_ShaderId > 0) Utils.deleteShaderProgram(m_ShaderId);

        // construire le vertex shader
        String srcVertexShader = getVertexShader();

        // construire le fragment shader
        String srcFragmentShader = getFragmentShader();

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "AxesXYZ");

        // déterminer où sont les variables attribute et uniform
        m_VertexLoc    = glGetAttribLocation( m_ShaderId, "glVertex");
        m_ColorLoc     = glGetAttribLocation( m_ShaderId, "glColor");
        m_MatPloc      = glGetUniformLocation(m_ShaderId, "mat4Projection");
        m_MatMVloc     = glGetUniformLocation(m_ShaderId, "mat4ModelView");
        m_ClipPlaneLoc = glGetUniformLocation(m_ShaderId, "ClipPlane");
    }


    /**
     * dessine l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // activer le shader
        glUseProgram(m_ShaderId);

        // fournir les matrices P et MV
        mat4.glUniformMatrix(m_MatPloc, mat4Projection);
        mat4.glUniformMatrix(m_MatMVloc, mat4ModelView);

        // si le plan de coupe est actif, alors le fournir
        if (m_ClipPlaneOn) {
            vec4.glUniform(m_ClipPlaneLoc, m_ClipPlane);
        }

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_VertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertexLoc, 3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les couleurs
        glEnableVertexAttribArray(m_ColorLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
        glVertexAttribPointer(m_ColorLoc, 3, GL_FLOAT, false, 0, 0);

        // dessiner les lignes
        glLineWidth(m_Width);
        glDrawArrays(GL_LINES, 0, 6);

        // désactiver les buffers et le shader
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexLoc);
        glDisableVertexAttribArray(m_ColorLoc);
        glUseProgram(0);
    }


    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane : vec4 en coordonnées caméra
     * @throws Exception
     */
    public void setClipPlane(boolean active, vec4 plane) throws Exception
    {
        // il faut recompiler s'il y a un changement d'état
        boolean recompile = (active != m_ClipPlaneOn);

        m_ClipPlaneOn = active;
        m_ClipPlane = plane;

        // recompiler le shader
        if (recompile) compileShader();
    }


    /**
     * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @throws Exception
     */
    public void setClipPlane(boolean active) throws Exception
    {
        // il faut recompiler s'il y a un changement d'état
        boolean recompile = (active != m_ClipPlaneOn);

        // nouvel état et plan à l'infini
        m_ClipPlaneOn = active;
        m_ClipPlane = vec4.fromValues(0,0,1,-1e38);

        // recompiler le shader
        if (recompile) compileShader();
    }


    /**
     * désactive le plan de coupe
     * @throws Exception
     */
    public void resetClipPlane() throws Exception
    {
        boolean recompile = m_ClipPlaneOn;
        m_ClipPlaneOn = false;
        m_ClipPlane = vec4.fromValues(0,0,1,-1e38);
        if (recompile) compileShader();
    }


    /** destructeur */
    public void destroy()
    {
        Utils.deleteShaderProgram(m_ShaderId);
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteVBO(m_ColorBufferId);
    }
}
