package com.dbooker.livreopengl.misc;

import static android.opengl.GLES20.*;

import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.utils.Utils;


/**
 * Cette classe permet de dessiner un maillage en forme d'étoile
 */
@SuppressWarnings("unused")
public class Star
{
    /** épaisseur des lignes et nombre de lignes */
    private float m_Size;
    private float m_Width;
    private float m_Alpha;

    /** plan de coupe */
    private boolean m_ClipPlaneOn;
    private vec4 m_ClipPlane;

    /** tableau des coordonnées, x,y,z à la suite */
    private int m_VertexBufferId;
    private int m_DebugLineCount;

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
     * @param alpha : transparence
     * @param width : largeur des lignes
     * @throws Exception
     */
    Star(float size, float alpha, float width) throws Exception
    {
        // paramètres
        m_Size = size;
        m_Alpha = alpha;
        m_Width = width;

        // plan de coupe par défaut à l'infini
        m_ClipPlaneOn = false;
        m_ClipPlane = vec4.fromValues(0,0,1,-1e38);

        // compiler le shader
        m_ShaderId = 0;
        compileShader();

        // créer et remplir le buffer des coordonnées
        ArrayList<Float> vertices = new ArrayList<>();
        float s = m_Size;    // taille des branches

        // dessiner une sorte d'étoile sur tous les axes (lampe omnidirectionnelle)
        vertices.add(-s); vertices.add( 0.0f); vertices.add( 0.0f);
        vertices.add(+s); vertices.add( 0.0f); vertices.add( 0.0f);

        vertices.add( 0.0f); vertices.add(-s); vertices.add( 0.0f);
        vertices.add( 0.0f); vertices.add(+s); vertices.add( 0.0f);

        vertices.add( 0.0f); vertices.add( 0.0f); vertices.add(-s);
        vertices.add( 0.0f); vertices.add( 0.0f); vertices.add(+s);

        float k = 0.577f * s; // c'est à dire 1/racine carrée de 3, pour faire une longueur unitaire
        vertices.add(-k); vertices.add(-k); vertices.add(-k);
        vertices.add(+k); vertices.add(+k); vertices.add(+k);

        vertices.add(-k); vertices.add(+k); vertices.add(-k);
        vertices.add(+k); vertices.add(-k); vertices.add(+k);

        vertices.add(-k); vertices.add(-k); vertices.add(+k);
        vertices.add(+k); vertices.add(+k); vertices.add(-k);

        vertices.add(-k); vertices.add(+k); vertices.add(+k);
        vertices.add(+k); vertices.add(-k); vertices.add(-k);

        // créer les VBOs des coordonnées
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_DebugLineCount = vertices.size() / 3;
    }


    /**
     * retourne le source du Vertex Shader
     */
    private String getVertexShader()
    {
        return
            "#version 310 es\n" +
            "in vec3 glVertex;\n" +
            "out vec4 frgPosition;\n" +
            "uniform mat4 mat4ModelView;\n" +
            "uniform mat4 mat4Projection;\n" +
            "void main()\n" +
            "{\n" +
            "    frgPosition = mat4ModelView * vec4(glVertex*" + m_Size + ", 1.0);\n" +
            "    gl_Position = mat4Projection * frgPosition;\n" +
            "}";
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
        srcFragmentShader += "out vec4 glFragData[4];\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (m_ClipPlaneOn) {
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        }
        srcFragmentShader += "    glFragData[0] = vec4(1.0, 1.0, 0.0, " + m_Alpha + ");\n";
        srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(1.0, 1.0, 1.0, 0.0);\n";
        srcFragmentShader += "}\n";
        return srcFragmentShader;
    }


    /**
     * recompile le shader
     */
    private void compileShader() throws Exception
    {
        // supprimer l'ancien shader s'il y en avait un
        if (m_ShaderId != 0) Utils.deleteShaderProgram(this.m_ShaderId);

        // construire le vertex shader
        String srcVertexShader = this.getVertexShader();

        // construire le fragment shader
        String srcFragmentShader = this.getFragmentShader();

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "Star");

        // déterminer où sont les variables attribute et uniform
        m_VertexLoc    = glGetAttribLocation(m_ShaderId, "glVertex");
        m_MatPloc      = glGetUniformLocation(m_ShaderId, "mat4Projection");
        m_MatMVloc     = glGetUniformLocation(m_ShaderId, "mat4ModelView");
        m_ClipPlaneLoc = glGetUniformLocation(m_ShaderId, "ClipPlane");
    }


    /** destructeur */
    public void destroy()
    {
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteShaderProgram(m_ShaderId);
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
        glVertexAttribPointer(m_VertexLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // dessiner les lignes
        glLineWidth(this.m_Width);
        glDrawArrays(GL_LINES, 0, m_DebugLineCount);

        // désactiver les buffers et le shader
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexLoc);
        glUseProgram(0);
    }


    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
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
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     */
    public void setClipPlane(boolean active) throws Exception
    {
        // il faut recompiler s'il y a un changement d'état
        boolean recompile = (active != m_ClipPlaneOn);

        // nouvel état et plan à l'infini
        m_ClipPlaneOn = active;
        m_ClipPlane = vec4.fromValues(0,0,1,1e38);

        // recompiler le shader
        if (recompile) compileShader();
    }


    /**
     * désactive le plan de coupe
     */
    public void resetClipPlane() throws Exception
    {
        boolean recompile = m_ClipPlaneOn;
        m_ClipPlaneOn = false;
        m_ClipPlane = vec4.fromValues(0,0,1,-1e38);
        if (recompile) compileShader();
    }
}
