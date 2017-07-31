/**
 * Définition de la classe DeferredShadingMaterial, une spécialisation de Material
 * Ce matériau est destiné à dessiner en mode Multiple Render Targets
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.utils.Utils;


@SuppressWarnings("unused")
public class DeferredShadingMaterial extends Material
{
    private static final String TAG = "LivreOpenGL";

    /// paramètres de construction des shaders
    protected boolean m_KdIsInterpolated;
    protected vec4 m_Kd;
    protected vec3 m_Ks;
    protected float m_Ns;

    /** textures utilisées par le matériau **/
    protected Texture2D m_TxDiffuse;
    protected Texture2D m_TxSpecular;

    protected int m_TxDiffuseLoc;
    protected int m_TxSpecularLoc;

    /*** MODE MRT SIMULÉ ***/
    /** mode MRT émulé sur Android (rustine en attendant des GPU complets) **/
    protected int[] m_NoMRTTxDiffuseLoc;
    protected int[] m_NoMRTTxSpecularLoc;
    /*** MODE MRT SIMULÉ ***/


    /**
     * initialisations communes à tous les constructeurs
     */
    protected void init()
    {
        // couleurs
        m_Kd = vec4.fromValues(1.0, 0.0, 1.0, 1.0);
        m_Ks = vec3.fromValues(0.5, 0.5, 0.5);
        m_Ns = 60.0f;

        // textures
        m_TxDiffuse = null;
        m_TxDiffuseLoc = -1;
        m_TxSpecular = null;
        m_TxSpecularLoc = -1;

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            m_NoMRTTxDiffuseLoc = new int[GBufferSize];
            m_NoMRTTxSpecularLoc = new int[GBufferSize];
            for (int i = 0; i < GBufferSize; i++) {
                m_NoMRTTxDiffuseLoc[i] = -1;
                m_NoMRTTxSpecularLoc[i] = -1;
            }
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
     * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
     * @param Kd : vec3
     * @throws Exception
     */
    public DeferredShadingMaterial(vec4 Kd) throws Exception
    {
        super("DeferredShadingMaterial");
        init();

        m_Kd = Kd;
        m_KdIsInterpolated = false;
        m_Ks = vec3.fromValues(0,0,0);
        m_Ns = -1.0f;

        // compiler le shader
        compileShader();
    }


    /**
     * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
     * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
     * @param Ks : vec3
     * @param Ns : poli du matériau
     * @throws Exception
     */
    public DeferredShadingMaterial(vec3 Ks, float Ns) throws Exception
    {
        super("DeferredShadingMaterial");
        init();

        m_KdIsInterpolated = true;
        m_Ks = Ks;
        m_Ns = Ns;

        // compiler le shader
        compileShader();
    }


    /**
     * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
     * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
     * @param Kd : vec3
     * @param Ks : vec3
     * @param Ns : poli du matériau
     * @throws Exception
     */
    public DeferredShadingMaterial(vec4 Kd, vec3 Ks, float Ns) throws Exception
    {
        super("DeferredShadingMaterial");
        init();

        m_Kd = Kd;
        m_KdIsInterpolated = false;
        m_Ks = Ks;
        m_Ns = Ns;

        // compiler le shader
        compileShader();
    }


    /**
     * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
     * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
     * @param diffuse : nom d'une texture
     * @param Ks : vec3
     * @param Ns : poli du matériau
     * @throws Exception
     */
    public DeferredShadingMaterial(String diffuse, vec3 Ks, float Ns) throws Exception
    {
        super("DeferredShadingMaterial");
        init();

        m_TxDiffuse = new Texture2D(diffuse, GL_LINEAR, GL_REPEAT);
        m_KdIsInterpolated = false;
        m_Ks = Ks;
        m_Ns = Ns;

        // compiler le shader
        compileShader();
    }


    /** destructeur */
    public void destroy()
    {
        // libérer les textures qui ont été chargées
        if (m_TxDiffuse != null) m_TxDiffuse.destroy();
        if (m_TxSpecular != null) m_TxSpecular.destroy();
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    @Override
    protected String getVertexShader()
    {
        // début du vertex shader
        String srcVertexShader = new String();
        srcVertexShader += "#version 310 es\n";
        srcVertexShader += "\n";
        srcVertexShader += "// paramètres uniform\n";
        srcVertexShader += "uniform mat4 mat4ModelView;\n";
        srcVertexShader += "uniform mat4 mat4Projection;\n";
        srcVertexShader += "uniform mat3 mat3Normal;\n";
        srcVertexShader += "\n";
        srcVertexShader += "// attributs de sommets\n";
        srcVertexShader += "in vec3 glVertex;\n";
        srcVertexShader += "in vec3 glNormal;\n";
        srcVertexShader += "\n";
        srcVertexShader += "// interpolation vers les fragments\n";
        srcVertexShader += "out vec4 frgPosition;\n";
        srcVertexShader += "out vec3 frgNormal;\n";

        // interpoler l'attribut couleur diffuse ?
        if (m_KdIsInterpolated) {
            // la couleur diffuse est un attribut de vertex
            srcVertexShader += "in vec4 glColor;\n";
            srcVertexShader += "out vec4 frgColor;\n";
        }

        // interpoler les coordonnées de texture ?
        if (m_TxDiffuse != null || m_TxSpecular != null) {
            // passer les coordonnées de texture
            srcVertexShader += "in vec2 glTexCoord;\n";
            srcVertexShader += "out vec2 frgTexCoord;\n";
        }

        srcVertexShader += "\n";
        srcVertexShader += "void main()\n";
        srcVertexShader += "{\n";
        srcVertexShader += "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n";
        srcVertexShader += "    gl_Position = mat4Projection * frgPosition;\n";
        srcVertexShader += "    frgNormal = mat3Normal * glNormal;\n";
        if (m_KdIsInterpolated) {
            // interpoler la couleur diffuse
            srcVertexShader += "    frgColor = glColor;\n";
        }
        if (m_TxDiffuse != null || m_TxSpecular != null) {
            // interpoler les coordonnées de texture
            srcVertexShader += "    frgTexCoord = glTexCoord;\n";
        }
        srcVertexShader += "}\n";
        return srcVertexShader.toString();
    }


    /**
     * retourne le source du Fragment Shader
     * @param num sert pour simuler le mode MRT sur les machines où il est absent
     */
    @Override
    protected String getFragmentShader(int num)
    {
        String srcFragmentShader = new String();
        srcFragmentShader += "#version 310 es\n";
        srcFragmentShader += "precision mediump float;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "// coordonnées et normale du fragment\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "in vec3 frgNormal;\n";

        // coordonnées de texture s'il y a une texture
        if (m_TxDiffuse != null || m_TxSpecular != null) {
            // coordonnées de texture interpolées
            srcFragmentShader += "in vec2 frgTexCoord;\n";
        }

        srcFragmentShader += "\n";
        srcFragmentShader += "// caractéristiques du matériau\n";

        // couleur ou texture diffuse
        if (m_TxDiffuse != null) {
            srcFragmentShader += "uniform sampler2D txDiffuse;\n";
        } else if (m_KdIsInterpolated) {
            srcFragmentShader += "in vec4 frgColor;\n";
        } else {
            srcFragmentShader += "const vec4 Kd = " + vec4.str(m_Kd) + ";\n";
        }

        // couleur ou texture spéculaire
        if (m_Ns >= 0.0) {
            if (m_TxSpecular != null) {
                srcFragmentShader += "uniform sampler2D txSpecular;\n";
            } else {
                srcFragmentShader += "const vec3 Ks = " + vec3.str(m_Ks) + ";\n";
            }
            srcFragmentShader += "const float Ns = " + m_Ns + ";\n";
        }

        // plan de coupe
        if (m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }

        srcFragmentShader += "\n";
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

        // plan de coupe
        if (m_ClipPlaneOn) {
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        }

        // couleur diffuse du fragment
        if (m_TxDiffuse != null) {
            srcFragmentShader += "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
        } else if (m_KdIsInterpolated) {
            srcFragmentShader += "    vec4 Kd = frgColor;\n";
        }

        // couleur spéculaire du fragment
        if (m_Ns >= 0.0 && m_TxSpecular != null) {
            srcFragmentShader += "    vec3 Ks = texture(txSpecular, frgTexCoord).rgb;\n";
        }

        // remplir les buffers MRT avec les informations nécessaires pour Phong plus tard
        srcFragmentShader += "    // remplir les buffers avec les informations\n";
        /*** MODE MRT SIMULÉ ***/
        switch (num) {
            case 0:
                srcFragmentShader += "    glFragColor = Kd;\n";
                break;
            case 1:
                if (m_Ns >= 0.0) {
                    srcFragmentShader += "    glFragColor = vec4(Ks, Ns);\n";
                } else {
                    srcFragmentShader += "    glFragColor = vec4(0.0);\n";
                }
                break;
            case 2:
                srcFragmentShader += "    glFragColor = frgPosition;\n";
                break;
            case 3:
                if (m_Ns >= 0.0) {
                    srcFragmentShader += "    glFragColor = vec4(normalize(frgNormal), 0.0);\n";
                } else {
                    srcFragmentShader += "    glFragColor = vec4(1.0, 1.0, 1.0, 0.0);\n";
                }
                break;
            default:
        /*** MODE MRT SIMULÉ ***/
                srcFragmentShader += "    glFragData[0] = Kd;\n";
                if (m_Ns >= 0.0) {
                    srcFragmentShader += "    glFragData[1] = vec4(Ks, Ns);\n";
                } else {
                    srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
                }
                srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
                if (m_Ns >= 0.0) {
                    srcFragmentShader += "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
                } else {
                    srcFragmentShader += "    glFragData[3] = vec4(1.0, 1.0, 1.0, 0.0);\n";
                }
        /*** MODE MRT SIMULÉ ***/
                break;
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "}\n";
        return srcFragmentShader.toString();
    }


    /**
     * recompile le shader du matériau
     * @throws Exception
     */
    @Override
    protected void compileShader() throws Exception
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            for (int i = 0; i < GBufferSize; i++) {
                // déterminer où sont les variables uniform
                m_NoMRTTxDiffuseLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "txDiffuse");
                m_NoMRTTxSpecularLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "txSpecular");
            }
        } else {
        /*** MODE MRT SIMULÉ ***/

            // déterminer où sont les variables uniform spécifiques
            m_TxDiffuseLoc = glGetUniformLocation(m_ShaderId, "txDiffuse");
            m_TxSpecularLoc = glGetUniformLocation(m_ShaderId, "txSpecular");

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    @Override
    public VBOset createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        VBOset vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_COLOR,    Utils.VEC4, "glColor");
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
        return vboset;
    }


    /**
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelView : mat4 contenant la transformation vers la caméra
     */
    @Override
    public void enable(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // appeler la méthode de la superclasse
        super.enable(mat4Projection, mat4ModelView);

        // prochaine unité de texture à utiliser
        int unit = GL_TEXTURE0;

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize>0 && c_NumBuffer>=0 && c_NumBuffer<GBufferSize) {

            if (m_NoMRTTxDiffuseLoc[c_NumBuffer] >= 0) {
                m_TxDiffuse.setTextureUnit(unit, m_NoMRTTxDiffuseLoc[c_NumBuffer]);
                unit += 1;
            }

            // activer la texture spéculaire si elle est utilisée
            if (m_NoMRTTxSpecularLoc[c_NumBuffer] >= 0) {
                m_TxSpecular.setTextureUnit(unit, m_NoMRTTxSpecularLoc[c_NumBuffer]);
                //unit += 1;
            }

        } else {
        /*** MODE MRT SIMULÉ ***/

            // activer la texture diffuse si elle est utilisée
            if (m_TxDiffuseLoc >= 0) {
                m_TxDiffuse.setTextureUnit(unit, m_TxDiffuseLoc);
                unit += 1;
            }

            // activer la texture spéculaire si elle est utilisée
            if (m_TxSpecularLoc >= 0) {
                m_TxSpecular.setTextureUnit(unit, m_TxSpecularLoc);
                //unit += 1;
            }

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * Cette méthode désactive le matériau
     * NB : le shader doit être activé
     */
    @Override
    public void disable()
    {
        // prochaine unité de texture à utiliser
        int unit = GL_TEXTURE0;

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize>0 && c_NumBuffer>=0 && c_NumBuffer<GBufferSize) {

            // désactiver la texture diffuse si elle est utilisée
            if (m_NoMRTTxDiffuseLoc[c_NumBuffer] >= 0) {
                m_TxDiffuse.setTextureUnit(unit);
                unit += 1;
            }

            // désactiver la texture spéculaire si elle est utilisée
            if (m_NoMRTTxSpecularLoc[c_NumBuffer] >= 0) {
                m_TxSpecular.setTextureUnit(unit);
                //unit += 1;
            }

        } else {
            /*** MODE MRT SIMULÉ ***/

            // désactiver la texture diffuse si elle est utilisée
            if (m_TxDiffuseLoc >= 0) {
                m_TxDiffuse.setTextureUnit(unit);
                unit += 1;
            }

            // désactiver la texture spéculaire si elle est utilisée
            if (m_TxSpecularLoc >= 0) {
                m_TxSpecular.setTextureUnit(unit);
                //unit += 1;
            }

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/

        // appeler la méthode de la superclasse
        super.disable();
    }
}
