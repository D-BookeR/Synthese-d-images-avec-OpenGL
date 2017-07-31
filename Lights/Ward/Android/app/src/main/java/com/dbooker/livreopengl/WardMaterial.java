package com.dbooker.livreopengl;

// Définition de la classe WardMaterial
// voir http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/

// superclasses et classes nécessaires

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat3;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.utils.Utils;


/**
 * Cette fonction définit un matériau pour dessiner le maillage
 */
public class WardMaterial extends Material
{
    // caractéristiques du matériau
    private vec3 m_DiffuseColor;
    private vec3 m_SpecularColor;
    private vec2 m_Alpha;

    // coordonnées et couleurs des sources de lumières
    private vec4[] m_LightsPositions;
    private vec3[] m_LightsColors;

    // emplacement des variables du shader
    int m_LightsPositionsLoc = -1;
    int m_LightsColorsLoc = -1;


    /**
     * Cette classe définit un matériau pour dessiner le maillage
     * @param lights_count : nombre de lampes à gérer
     * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
     * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
     * @param alpha : vec2 coefficient de Ward
     */
    public WardMaterial(int lights_count, vec3 diffuse, vec3 specular, vec2 alpha) throws Exception
    {
        super("WardMaterial");

        // caractéristiques du matériau
        m_DiffuseColor = diffuse;
        m_SpecularColor = specular;
        m_Alpha = alpha;

        // coordonnées et couleurs des sources de lumières
        m_LightsPositions = new vec4[lights_count];
        m_LightsColors = new vec3[lights_count];
        for (int i=0; i<lights_count; i++) {
            m_LightsPositions[i] = vec4.create();
            m_LightsColors[i] = vec3.create();
        }

        // compiler le shader
        compileShader();
    }


    /**
     * définit la position d'une lampe
     * @param num : numéro de la lampe
     * @param position : vec4 indiquant où elle se trouve dans l'espace caméra
     * @param color : vec3 indiquant sa couleur et son intensité
     */
    public void setLightPositionColor(int num, vec4 position, vec3 color)
    {
        vec4.copy(m_LightsPositions[num], position);
        vec3.copy(m_LightsColors[num], color);
    }


    /**
     * retourne le source du Vertex Shader
     */
    @Override
    protected String getVertexShader()
    {
        String srcVertexShader =
            "#version 310 es\n" +
            "in vec3 glVertex;\n" +
            "in vec3 glNormal;\n" +
            "in vec3 glTangent;\n" +
            "out vec4 frgPosition;\n" +
            "out vec3 frgNormal;\n" +
            "out vec3 frgTangent;\n" +
            "uniform mat4 mat4ModelView;\n" +
            "uniform mat4 mat4Projection;\n" +
            "uniform mat3 mat3Normal;\n" +
            "void main()\n" +
            "{\n" +
            "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n" +
            "    gl_Position = mat4Projection * frgPosition;\n" +
            "    frgNormal = mat3Normal * glNormal;\n" +
            "    frgTangent = mat3Normal * glTangent;\n" +
            "}";
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    @Override
    protected String getFragmentShader()
    {
        int lights_count = m_LightsPositions.length;

        String srcFragmentShader =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "\n" +
            "// informations géométriques\n" +
            "in vec4 frgPosition;\n" +
            "in vec3 frgNormal;\n" +
            "in vec3 frgTangent;\n" +
            "\n" +
            "// sources de lumière\n" +
            "uniform vec4 LightsPositions["+lights_count+"];\n" +
            "uniform vec3 LightsColors["+lights_count+"];\n" +
            "\n" +
            "// caractéristiques du matériau\n" +
            "const vec3 Kd = "+vec3.str(m_DiffuseColor)+";\n" +
            "const vec3 Ks = "+vec3.str(m_SpecularColor)+";\n" +
            "const vec2 alpha = "+vec2.str(m_Alpha)+";\n" +
            "\n" +
            "const float pi = 3.14159;\n" +
            "\n" +
            "out vec4 glFragColor;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "    // reconstruire le repère de Frenet normalisé\n" +
            "    vec3 N = normalize(frgNormal);\n" +
            "    vec3 T = normalize(frgTangent);\n" +
            "    vec3 B = cross(N, T);\n" +
            "\n" +
            "    // direction du vecteur Vue au niveau du fragment\n" +
            "    vec3 V = normalize(-frgPosition.xyz);\n" +
            "    float dotNV = clamp(dot(N, V), 0.0, 1.0);\n" +
            "\n" +
            "    // couleur finale = cumul des éclairements\n" +
            "    vec3 sum = vec3(0.0, 0.0, 0.0);\n" +
            "\n" +
            "    // boucle sur les lampes\n" +
            "    for (int i=0; i<"+lights_count+"; i++) {\n" +
            "\n" +
            "        // vecteur allant du point à la lumière\n" +
            "        vec3 L = normalize(LightsPositions[i].xyz - frgPosition.xyz);\n" +
            "\n" +
            "        // modulation de l'éclairement type Lambert\n" +
            "        float dotNL = clamp(dot(N, L), 0.0, 1.0);\n" +
            "\n" +
            "        // demi-vecteur H entre L et V, normalisé (!)\n" +
            "        vec3 H = normalize(L + V);\n" +
            "        float dotNH = clamp(dot(N, H), 0.0, 1.0);\n" +
            "\n" +
            "        // éclairement spéculaire Ward\n" +
            "        float hts = dot(H, T) / alpha.s;\n" +
            "        float hbs = dot(H, B) / alpha.t;\n" +
            "        float beta = -2.0 * (hts*hts + hbs*hbs)/(1.0 + dotNH);\n" +
            "        float s = exp(beta) / (4.0 * pi * alpha.s * alpha.t * sqrt(dotNL*dotNV));\n" +
            "\n" +
            "        // cumul des contributions lumineuses\n" +
            "        sum += LightsColors[i] * ( (Kd + Ks * s) * dotNL );\n" +
            "    }\n" +
            "\n" +
            "    glFragColor = vec4(sum, 1.0);\n" +
            "}";
        return srcFragmentShader;
    }


    /**
     * recompile le shader du matériau
     */
    @Override
    protected void compileShader() throws Exception
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        // déterminer où sont les variables uniform
        m_LightsPositionsLoc = glGetUniformLocation(m_ShaderId, "LightsPositions");
        m_LightsColorsLoc  = glGetUniformLocation(m_ShaderId, "LightsColors");
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
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_TANGENT,  Utils.VEC3, "glTangent");
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

        // initialisation des lampes
        vec4.glUniform(m_LightsPositionsLoc, m_LightsPositions);
        vec3.glUniform(m_LightsColorsLoc,  m_LightsColors);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        super.destroy();
    }
}
