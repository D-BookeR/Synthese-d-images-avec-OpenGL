/**
 * Définition de la classe SoftSpotLight, un type de lampe plus sophistiqué
 * voir http://codeflow.org/entries/2013/feb/15/soft-shadow-mapping/
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.utils.Utils;

import java.util.ArrayList;


@SuppressWarnings("unused")
public class SoftSpotLightDebug extends SoftSpotLight
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
     * @param maxangle : angle en degrés d'ouverture total de la lampe
     * @param minangle : angle dans lequel l'intensité est 100%, il y a un dégradé entre minangle et maxangle
     * @param radius : largeur de la source de lumière en unités à rapporter à far
     * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
     * @param near : distance la plus proche dans la shadowmap
     * @param far : distance la plus lointaine dans la shadowmap
     * @param pcss : algorithme employé pour le calcul du flou des ombres true:PCSS, false:PCF
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     * @throws Exception
     */
    public SoftSpotLightDebug(float maxangle, float minangle, float radius, int shadowmapsize, float near, float far, boolean pcss, boolean offsetfill, int cullface) throws Exception
    {
        super(maxangle, minangle, radius, shadowmapsize, near, far, pcss, offsetfill, cullface);
        init();
    }
    public SoftSpotLightDebug(float maxangle, float minangle, float radius, int shadowmapsize, float near, float far, boolean pcss) throws Exception
    {
        super(maxangle, minangle, radius, shadowmapsize, near, far, pcss);
        init();
    }
    public SoftSpotLightDebug(float maxangle, float minangle, float radius, int shadowmapsize, float near, float far) throws Exception
    {
        super(maxangle, minangle, radius, shadowmapsize, near, far);
        init();
    }

    /** initialisations communes à tous les constructeurs */
    private void init() throws Exception
    {
        m_Name = "SoftSpotLightDebug";

        /// ajouter le shader et le VBO permettant de dessiner la lampe

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
        m_DebugShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "SoftSpotLightDebug");

        // déterminer où sont les variables attribute et uniform
        m_DebugVertexLoc        = glGetAttribLocation( m_DebugShaderId, "glVertex");
        m_DebugMatProjectionLoc = glGetUniformLocation(m_DebugShaderId, "matProjection");
        m_DebugMatModelViewLoc  = glGetUniformLocation(m_DebugShaderId, "matMV");

        // créer et remplir les buffers des coordonnées et des couleurs
        ArrayList<Float> vertices = new ArrayList<>();
        // dessiner un cône dirigé vers -Z
        float rFar = (float) (Math.tan(m_MaxAngle * 0.5f) * m_Far);
        float rNear = (float) (Math.tan(m_MinAngle * 0.5f) * m_Near);
        int nbs = 12;
        for (int i=0; i<nbs; i++) {
            // angle, cosinus et sinus du point courant
            float a = Utils.radians(360.0f * (i+0.0f) / nbs);
            float cosa = (float) Math.cos(a);
            float sina = (float) Math.sin(a);
            // angle, cosinus et sinus du point suivant
            float b = Utils.radians(360.0f * (i+1.0f) / nbs);
            float cosb = (float) Math.cos(b);
            float sinb = (float) Math.sin(b);
            // cercle à la distance Near et correspondant à minangle
            vertices.add(rNear*cosa); vertices.add(rNear*sina); vertices.add(-m_Near);
            vertices.add(rNear*cosb); vertices.add(rNear*sinb); vertices.add(-m_Near);
            // rayons du spot vers la distance Far et correspondant à maxangle
            vertices.add(0.0f);       vertices.add(0.0f);       vertices.add(0.0f);
            vertices.add(rFar*cosa);  vertices.add(rFar*sina);  vertices.add(-m_Far);
            // cercle à la distance Far et correspondant à maxangle
            vertices.add(rFar*cosa);  vertices.add(rFar*sina);  vertices.add(-m_Far);
            vertices.add(rFar*cosb);  vertices.add(rFar*sinb);  vertices.add(-m_Far);
        }
        m_DebugLineCount = vertices.size() / 3;

        // créer les VBOs des coordonnées et des couleurs
        m_DebugVertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * destructeur
     */
    public void destroy()
    {
        Utils.deleteVBO(m_DebugVertexBufferId);
        Utils.deleteShaderProgram(m_DebugShaderId);
        super.destroy();
    }


    /**
     * dessine une représentation de la lampe
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue, ne sert pas car on prend directement les positions caméra
     */
    @Override
    public void onDraw(mat4 mat4Projection, mat4 mat4View)
    {
        // activer le shader
        glUseProgram(m_DebugShaderId);

        // décaler l'origine à la position caméra de la lampe
        mat4 mat4ModelView = mat4.create();
        mat4.lookAt(mat4ModelView, vec3.fromVec(m_PositionCamera), vec3.fromVec(m_TargetCamera), vec3.fromValues(0,1,0));
        mat4.invert(mat4ModelView, mat4ModelView);

        // fournir les matrices MV et P
        mat4.glUniformMatrix(m_DebugMatModelViewLoc,  mat4ModelView);
        mat4.glUniformMatrix(m_DebugMatProjectionLoc, mat4Projection);

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
     */
    public void onDrawShadowMap()
    {
        // s'il n'y a pas de shadowmap, alors sortir sans rien faire
        if (m_ShadowMap == null) return;

        m_ShadowMap.onDraw();
    }
}
