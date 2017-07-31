// Définition de la classe SimpleColorMaterial, une spécialisation de Material
// Ce matériau permet de dessiner en mode Multiple Render Targets

package com.dbooker.livreopengl.material;

import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.utils.Utils;

/**
 * Une instance de cette classe dessine d'une couleur uniforme
 */
@SuppressWarnings("unused")
public class SimpleColorMaterial extends Material
{
    private static final String TAG = "LivreOpenGL";

    private vec4 m_Color;
    /**
     * constructeur
     * @param color : vec4(r,v,b,a) qui donne la couleur du matériau
     */
    public SimpleColorMaterial(vec4 color) throws Exception
    {
        super("SimpleColorMaterial");

        // couleur propre imposée
        m_Color = vec4.clone(color);

        // compiler le shader
        compileShader();
    }


    /**
     * constructeur, crée une couleur aléatoire
     */
    public SimpleColorMaterial() throws Exception
    {
        super("SimpleColorMaterial");

        // couleur propre aléatoire
        m_Color = vec4.create();
        vec4.random(m_Color, 1.0f);

        // compiler le shader
        compileShader();
    }


    /** destructeur */
    @Override
    public void destroy()
    {
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    @Override
    protected String getVertexShader()
    {
        return
            "#version 310 es\n"+
            "\n"+
            "// paramètres uniform\n"+
            "uniform mat4 mat4ModelView;\n"+
            "uniform mat4 mat4Projection;\n"+
            "\n"+
            "// attributs de sommets\n"+
            "in vec3 glVertex;\n"+
            "\n"+
            "// interpolation vers les fragments\n"+
            "out vec4 frgPosition;\n"+
            "\n"+
            "void main()\n"+
            "{\n"+
            "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"+
            "    gl_Position = mat4Projection * frgPosition;\n"+
            "}";
    }


    /**
     * retourne le source du Fragment Shader
     * @param num sert pour simuler le mode MRT sur les machines où il est absent
     */
    @Override
    protected String getFragmentShader(int num)
    {
        String srcFragmentShader = "";
        srcFragmentShader += "#version 310 es\n";
        srcFragmentShader += "precision mediump float;\n";
        if (m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        /*** MODE MRT SIMULÉ ***/
        if (num>=0) {
            srcFragmentShader += "out vec4 glFragColor;\n";
        } else {
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "out vec4 glFragData[4];\n";
        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (m_ClipPlaneOn) {
            srcFragmentShader += "    // plan de coupe\n";
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
            srcFragmentShader += "\n";
        }

        /*** MODE MRT SIMULÉ ***/
        switch (num) {
            case 0:
                srcFragmentShader += "    glFragColor = "+vec4.str(m_Color)+";\n";
                break;
            case 1:
                srcFragmentShader += "    glFragColor = vec4(0.0);\n";
                break;
            case 2:
                srcFragmentShader += "    glFragColor = frgPosition;\n";
                break;
            case 3:
                srcFragmentShader += "    glFragColor = vec4(0.0,0.0,0.0, 1.0);\n";
                break;
            default:
        /*** MODE MRT SIMULÉ ***/

        srcFragmentShader += "    glFragData[0] = "+vec4.str(m_Color)+";\n";
        srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(0.0,0.0,0.0, 1.0);\n";

        /*** MODE MRT SIMULÉ ***/
                break;
        }
        /*** MODE MRT SIMULÉ ***/

        srcFragmentShader += "}";
        return srcFragmentShader;
    }
}
