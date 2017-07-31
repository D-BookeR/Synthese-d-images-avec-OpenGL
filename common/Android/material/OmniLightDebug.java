/**
 * Définition de la classe OmniLight, une base pour construire des éclairages diffus et localisés
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.utils.Utils;


@SuppressWarnings("unused")
public class OmniLightDebug extends OmniLight
{
    /** pour dessiner la lampe */
    protected int m_DebugShaderId;
    protected int m_DebugVertexLoc;
    protected int m_DebugMatProjectionLoc;
    protected int m_DebugMatModelViewLoc;
    protected int m_DebugVertexBufferId;
    protected int m_DebugLineCount;


    /**
     * constructeur
     * C'est une lampe toute simple, positionnelle ou directionnelle, sans ombre
     * @throws Exception
     */
    public OmniLightDebug() throws Exception
    {
        // initialisation des variables membre spécifiques
        super();
        m_Name = "OmniLightDebug";

        /// ajouter le shader et le VBO permettant de dessiner la lampe : une sorte d'étoile

        // construire le vertex shader
        String srcVertexShader =
            "#version 310 es\n" +
            "in vec3 glVertex;\n" +
            "uniform mat4 matMV;\n" +
            "uniform mat4 matProjection;\n" +
            "void main()\n" +
            "{\n" +
            "    gl_Position = matProjection * (matMV * vec4(glVertex, 1.0));\n" +
            "}";

        // construire le fragment shader
        String srcFragmentShader =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "    glFragColor = vec4(1.0, 1.0, 0.0, 1.0);\n" +
            "}";

        // compiler le shader de dessin
        m_DebugShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "OmniLightDebug");

        // déterminer où sont les variables attribute et uniform
        m_DebugVertexLoc        = glGetAttribLocation( m_DebugShaderId, "glVertex");
        m_DebugMatProjectionLoc = glGetUniformLocation(m_DebugShaderId, "matProjection");
        m_DebugMatModelViewLoc  = glGetUniformLocation(m_DebugShaderId, "matMV");

        // créer et remplir le buffer des coordonnées
        ArrayList<Float> vertices = new ArrayList<>();
        final float s = 0.5f;    // taille des branches

        // dessiner une sorte d'étoile sur tous les axes (lampe omnidirectionnelle)
        vertices.add(-s); vertices.add( 0.0f); vertices.add( 0.0f);
        vertices.add(+s); vertices.add( 0.0f); vertices.add( 0.0f);

        vertices.add( 0.0f); vertices.add(-s); vertices.add( 0.0f);
        vertices.add( 0.0f); vertices.add(+s); vertices.add( 0.0f);

        vertices.add( 0.0f); vertices.add( 0.0f); vertices.add(-s);
        vertices.add( 0.0f); vertices.add( 0.0f); vertices.add(+s);

        final float k = 0.577f * s; // c'est à dire 1/racine carrée de 3, pour faire une longueur unitaire
        vertices.add(-k); vertices.add(-k); vertices.add(-k);
        vertices.add(+k); vertices.add(+k); vertices.add(+k);

        vertices.add(-k); vertices.add(+k); vertices.add(-k);
        vertices.add(+k); vertices.add(-k); vertices.add(+k);

        vertices.add(-k); vertices.add(-k); vertices.add(+k);
        vertices.add(+k); vertices.add(+k); vertices.add(-k);

        vertices.add(-k); vertices.add(+k); vertices.add(+k);
        vertices.add(+k); vertices.add(-k); vertices.add(-k);
        m_DebugLineCount = vertices.size() / 3;

        // créer le VBO des coordonnées
        m_DebugVertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * destructeur
     */
    @Override
    public void destroy()
    {
        Utils.deleteVBO(m_DebugVertexBufferId);
        Utils.deleteShaderProgram(m_DebugShaderId);
        super.destroy();
    }


    /**
     * dessine une représentation de la lampe
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue (nb: pas utilisée)
     * @param scale : taille de l'étoile représentant la lampe
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4View, float scale)
    {
        // activer le shader
        glUseProgram(m_DebugShaderId);

        // décaler l'origine à la position caméra de la lampe
        mat4 mat4ModelView = mat4.create();
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromVec(m_PositionCamera));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(scale, scale, scale));

        // fournir les matrices MV et P
        mat4.glUniformMatrix(m_DebugMatModelViewLoc, mat4ModelView);
        mat4.glUniformMatrix(m_DebugMatProjectionLoc,  mat4Projection);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_DebugVertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_DebugVertexBufferId);
        glVertexAttribPointer(m_DebugVertexLoc, 3, GL_FLOAT, false, 0, 0);

        // dessiner le maillage
        glDrawArrays(GL_LINES, 0, m_DebugLineCount);

        // désactiver les buffers et le shader
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_DebugVertexLoc);
        glUseProgram(0);
    }


    /**
     * dessine la shadowmap de cette lampe sur l'écran (remplit tout l'écran)
     * NB : cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     */
    public void onDrawShadowMap()
    {
    }
}
