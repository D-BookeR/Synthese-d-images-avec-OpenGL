package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.utils.Utils;

import java.util.ArrayList;


@SuppressWarnings("unused")
public class JointDebug extends Joint
{
    /** pour dessiner la jointure */
    protected int m_ShaderId;
    protected int m_VertexLoc;
    protected int m_ColorLoc;
    protected int m_MatProjectionLoc;
    protected int m_MatModelViewLoc;
    protected int m_VertexBufferId;
    protected int m_ColorBufferId;
    protected int m_TriangleCount;
    protected boolean m_IsBone;


    /**
     * Constructeur
     * @param nom de la jointure
     * @param parent jointure parente
     * @param os indique s'il faut dessiner l'os ou les poids
     */
    public JointDebug(String nom, Joint parent, boolean os) throws Exception
    {
        super(nom, parent);
        init(os);
    }
    public JointDebug(String nom, Joint parent) throws Exception
    {
        super(nom, parent);
        init(true);
    }

    /**
     * initialisation
     * @param os indique s'il faut dessiner l'os ou les poids
     */
    private void init(boolean os) throws Exception
    {
        /// ajouter le shader et les VBOs permettant de dessiner la jointure

        // construire le vertex shader
        String srcVertexShader =
            "#version 310 es\n" +
            "in vec3 glVertex;\n" +
            "in vec3 glColor;\n" +
            "out vec3 frgColor;\n" +
            "uniform mat4 matMV;\n" +
            "uniform mat4 matProjection;\n" +
            "void main()\n" +
            "{\n" +
            "    gl_Position = matProjection * matMV * vec4(glVertex, 1.0);\n" +
            "    frgColor = glColor;\n" +
            "}";

        // construire le fragment shader
        String srcFragmentShader =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "in vec3 frgColor;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "    glFragColor = vec4(frgColor, 1.0);\n" +
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "JointDebug");

        // déterminer où sont les variables attribute et uniform
        m_VertexLoc        = glGetAttribLocation( m_ShaderId, "glVertex");
        m_ColorLoc         = glGetAttribLocation( m_ShaderId, "glColor");
        m_MatProjectionLoc = glGetUniformLocation(m_ShaderId, "matProjection");
        m_MatModelViewLoc  = glGetUniformLocation(m_ShaderId, "matMV");

        // créer et remplir les buffers des coordonnées et des couleurs
        ArrayList<Float> vertices = new ArrayList<>();
        ArrayList<Float> colors = new ArrayList<>();

        m_IsBone = os;
        if (m_IsBone) {

            // Ce mode permet de voir les os du squelette, il affiche des sortes
            // de pointes de flèches colorées de la taille des os

            // dessiner 4 triangles formant une sorte de pointe de flèche
            vertices.add(0.0f); vertices.add(0.0f); vertices.add(0.0f);
            colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);
            vertices.add(0.0f); vertices.add(0.0f); vertices.add(-1.0f);
            colors.add(0.0f); colors.add(1.0f); colors.add(0.0f);
            vertices.add(0.0f); vertices.add(0.1f); vertices.add(-0.1f);
            colors.add(1.0f); colors.add(0.0f); colors.add(0.0f);

            vertices.add(0.0f); vertices.add(0.0f); vertices.add(0.0f);
            colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);
            vertices.add(0.0f); vertices.add(0.0f); vertices.add(-1.0f);
            colors.add(0.0f); colors.add(1.0f); colors.add(0.0f);
            vertices.add(0.0f); vertices.add(-0.1f);vertices.add(-0.1f);
            colors.add(1.0f); colors.add(0.0f); colors.add(0.0f);

            vertices.add(0.0f); vertices.add(0.0f); vertices.add(0.0f);
            colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);
            vertices.add(0.0f); vertices.add(0.0f); vertices.add(-1.0f);
            colors.add(0.0f); colors.add(1.0f); colors.add(0.0f);
            vertices.add(0.1f); vertices.add(0.0f); vertices.add(-0.1f);
            colors.add(1.0f); colors.add(0.0f); colors.add(0.0f);

            vertices.add(0.0f); vertices.add(0.0f); vertices.add(0.0f);
            colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);
            vertices.add(0.0f); vertices.add(0.0f); vertices.add(-1.0f);
            colors.add(0.0f); colors.add(1.0f); colors.add(0.0f);
            vertices.add(-0.1f);vertices.add(0.0f); vertices.add(-0.1f);
            colors.add(1.0f); colors.add(0.0f); colors.add(0.0f);

            m_TriangleCount = 4;

        } else {

            // Ce mode permet d'ajuster les zones m_Radius, m_Min et m_Max d'influence des jointures
            // Il affiche une sorte de cylindre dont la longueur dépend de Min et Max et le rayon
            // dépend de m_Radius. La couleur bleue indique des poids nuls, la couleur rouge un poids 1

            // dessiner un cylindre dirigé vers -Z
            int nbs = 12;
            for (int i=0; i<nbs; i++) {
                // angle, cosinus et sinus du point courant
                float a = Utils.radians(360 * (i+0.0f) / nbs);
                float cosa = (float) Math.cos(a);
                float sina = (float) Math.sin(a);
                // angle, cosinus et sinus du point suivant
                float b = Utils.radians(360 * (i+0.5f) / nbs);
                float cosb = (float) Math.cos(b);
                float sinb = (float) Math.sin(b);
                // angle, cosinus et sinus du point suivant
                float c = Utils.radians(360 * (i+1.0f) / nbs);
                float cosc = (float) Math.cos(c);
                float sinc = (float) Math.sin(c);
                // triangles
                vertices.add(cosa); vertices.add(sina); vertices.add(0.0f);
                colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);

                vertices.add(cosb); vertices.add(sinb); vertices.add(1.0f);
                colors.add(1.0f); colors.add(0.0f); colors.add(0.0f);

                vertices.add(cosc); vertices.add(sinc); vertices.add(0.0f);
                colors.add(0.0f); colors.add(0.0f); colors.add(1.0f);
            }
            m_TriangleCount = nbs;
        }

        // créer les VBOs des coordonnées et des couleurs
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_ColorBufferId  = Utils.makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteVBO(m_ColorBufferId);
        Utils.deleteShaderProgram(m_ShaderId);
        super.destroy();
    }


    /**
     * dessine une représentation de la jointure
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4View)
    {
        // afficher tous les triangles
        boolean cullFacePrec = glIsEnabled(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
        // les os doivent apparaître devant le maillage, mais pas les poids
        boolean depthTestPrec = glIsEnabled(GL_DEPTH_TEST);
        if (m_IsBone) glDisable(GL_DEPTH_TEST);

        // activer le shader
        glUseProgram(m_ShaderId);

        // décaler l'origine au pivot de la jointure
        mat4 mat4ModelView = mat4.create();
        vec3 target = vec3.create();
        vec3.add(target, m_Pivot, m_Direction);
        mat4.lookAt(mat4ModelView, m_Pivot, target, vec3.fromValues(0,1,0));
        mat4.invert(mat4ModelView, mat4ModelView);

        // modifier la taille de la jointure (impossible de faire ça dans le constructeur)
        if (m_IsBone) {
            mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(m_Length, m_Length, m_Length));
        } else {
            mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(m_Radius, m_Radius, m_Length*(m_Max-m_Min)));
            mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0, 0.0, m_Length*m_Min));
        }

        // transformations par la matrice globale et celle de vue
        mat4.multiply(mat4ModelView, m_MatGlobal, mat4ModelView);
        mat4.multiply(mat4ModelView, mat4View, mat4ModelView);

        // fournir les matrices P et MV
        mat4.glUniformMatrix(m_MatProjectionLoc,  mat4Projection);
        mat4.glUniformMatrix(m_MatModelViewLoc, mat4ModelView);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_VertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertexLoc, 3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les couleurs
        glEnableVertexAttribArray(m_ColorLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
        glVertexAttribPointer(m_ColorLoc, 3, GL_FLOAT, false, 0, 0);

        // dessiner le maillage
        glDrawArrays(GL_TRIANGLES, 0, m_TriangleCount);

        // désactiver les buffers et le shader
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexLoc);
        glDisableVertexAttribArray(m_ColorLoc);
        glUseProgram(0);

        if (cullFacePrec) glEnable(GL_CULL_FACE);
        if (depthTestPrec) glEnable(GL_DEPTH_TEST);
    }
}
