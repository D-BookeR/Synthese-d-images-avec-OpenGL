package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat3;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.Texture2D;
import com.dbooker.livreopengl.utils.Utils;

public class Earth
{
    /** VBO des coordonnées, x,y,z à la suite */
    int m_VertexBufferId;

    /** VBO des coordonnées, x,y,z à la suite */
    int m_NormalBufferId;

    /** VBO des coordonnées de texture */
    int m_TexcoordBufferId;

    /** VBO des index */
    int m_IndexBufferId;
    int m_IndexBufferSize;

    /** identifiants liés au shader */
    int m_ShaderId;
    int m_VertexLoc;
    int m_NormalLoc;
    int m_TexCoordLoc;
    int m_MatMVLoc;
    int m_MatPLoc;
    int m_MatNLoc;
    int m_LightPositionLoc;
    int m_TxDiffuseDayLoc;
    int m_TxDiffuseNightLoc;
    int m_TxSpecularLoc;

    /** textures */
    Texture2D m_TxDiffuseDay;
    Texture2D m_TxDiffuseNight;
    Texture2D m_TxSpecular;


    public Earth(int nblon, int nblat) throws Exception
    {
        /** Textures */
        m_TxDiffuseDay = new Texture2D("data/textures/earth/earth_map.jpg");
        m_TxDiffuseNight = new Texture2D("data/textures/earth/earth_lights.jpg");
        m_TxSpecular = new Texture2D("data/textures/earth/earth_spec.jpg");

        /** shader */

        final String Phong_vs =
            "#version 310 es\n"+
            "uniform mat4 matMV;\n" +
            "uniform mat4 matProjection;\n" +
            "uniform mat3 matNormal;\n" +
            "in vec3 glVertex;\n" +
            "in vec3 glNormal;\n" +
            "in vec2 glTexCoord;\n" +
            "out vec3 frgNormal;\n" +
            "out vec4 frgPosition;\n" +
            "out vec2 frgTexCoord;\n" +
            "void main()\n" +
            "{\n" +
            "   frgPosition = matMV * vec4(glVertex, 1.0);\n" +
            "   gl_Position = matProjection * frgPosition;\n" +
            "   frgNormal =  matNormal * glNormal;\n" +
            "   frgTexCoord = glTexCoord;\n" +
            "}";

        final String Phong_fs =
            "#version 310 es\n"+
            "precision mediump float;\n" +
            "uniform vec4 LightPosition;\n" +
            "uniform sampler2D diffuseJour;\n" +
            "uniform sampler2D diffuseNuit;\n" +
            "uniform sampler2D speculaire;\n" +
            "in vec3 frgNormal;\n" +
            "in vec4 frgPosition;\n" +
            "in vec2 frgTexCoord;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "   vec3 N =  normalize(frgNormal);\n" +
            "   vec3 L =  normalize(\n" +
            "   vec3(LightPosition-frgPosition));\n" +
            "   vec3 R = reflect(normalize(frgPosition.xyz), N);\n" +
            "   float NL = dot(N,L);\n" +
            "   float k = smoothstep(-0.2, 0.2, NL);\n" +
            "   NL = clamp(NL,0.0,1.0);\n" +
            "   vec4 Jour = texture(diffuseJour, frgTexCoord);\n" +
            "   vec4 Nuit = texture(diffuseNuit, frgTexCoord);\n" +
            "   vec3 Kd = mix(Nuit.rgb, Jour.rgb*NL, k);\n" +
            "   float RL = clamp(dot(R,L), 0.0, 1.0);\n" +
            "   vec3 Ks = texture(speculaire, frgTexCoord).rgb;\n" +
            "   vec3 color = Kd + Ks*pow(RL, 256.0);\n" +
            "   glFragColor = vec4(color,1);\n" +
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(Phong_vs, Phong_fs, "shader terre");

        // déterminer où sont les variables attribute
        m_VertexLoc   = glGetAttribLocation(m_ShaderId, "glVertex");
        m_NormalLoc   = glGetAttribLocation(m_ShaderId, "glNormal");
        m_TexCoordLoc = glGetAttribLocation(m_ShaderId, "glTexCoord");

        // déterminer où sont les variables uniform
        m_MatMVLoc         = glGetUniformLocation(m_ShaderId, "matMV");
        m_MatPLoc          = glGetUniformLocation(m_ShaderId, "matProjection");
        m_MatNLoc          = glGetUniformLocation(m_ShaderId, "matNormal");
        m_LightPositionLoc = glGetUniformLocation(m_ShaderId, "LightPosition");
        m_TxDiffuseDayLoc = glGetUniformLocation(m_ShaderId, "diffuseJour");
        m_TxDiffuseNightLoc = glGetUniformLocation(m_ShaderId, "diffuseNuit");
        m_TxSpecularLoc  = glGetUniformLocation(m_ShaderId, "speculaire");

        // créer et remplir le buffer des vertices et des normales
        float[] vertices = new float[Utils.VEC3*(nblat+1)*(nblon+1)];
        int iv = 0;
        float[] normals = new float[Utils.VEC3*(nblat+1)*(nblon+1)];
        int in = 0;
        float[] texcoords = new float[Utils.VEC2*(nblat+1)*(nblon+1)];
        int it = 0;
        float dlat = 180.0f/nblat;
        float dlon = 360.0f/nblon;
        for (int ilat=0; ilat<=nblat; ilat++) {
            float coslat = (float) Math.cos(Utils.radians(ilat*dlat-90));
            float sinlat = (float) Math.sin(Utils.radians(ilat*dlat-90));
            for (int ilon=0; ilon<=nblon; ilon++) {
                float coslon = (float) Math.cos(Utils.radians(ilon*dlon));
                float sinlon = (float) Math.sin(Utils.radians(ilon*dlon));
                // ajouter les coordonnées du point (lon,lat)
                vertices[iv++] = coslat*coslon;
                vertices[iv++] = sinlat;
                vertices[iv++] = coslat*sinlon;
                // ajouter la normale au point (lon,lat)
                normals[in++] = coslat*coslon;
                normals[in++] = sinlat;
                normals[in++] = coslat*sinlon;
                // ajouter les coordonnées de texture
                texcoords[it++] = 1.0f - (ilon+0.0f)/nblon;
                texcoords[it++] = (ilat+0.0f)/nblat;
            }
        }
        m_VertexBufferId   = Utils.makeFloatVBO(vertices,  GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_NormalBufferId   = Utils.makeFloatVBO(normals,  GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_TexcoordBufferId = Utils.makeFloatVBO(texcoords, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des indices
        short[] indexlist = new short[6 * nblat * nblon];
        int i = 0;
        for (int ilat=0; ilat<nblat; ilat++) {
            for (int ilon=0; ilon<nblon; ilon++) {
                // ajouter deux triangles
                indexlist[i++] = (short) ((ilat+0)*(nblon+1) + ilon+1);
                indexlist[i++] = (short) ((ilat+0)*(nblon+1) + ilon+0);
                indexlist[i++] = (short) ((ilat+1)*(nblon+1) + ilon+0);

                indexlist[i++] = (short) ((ilat+0)*(nblon+1) + ilon+1);
                indexlist[i++] = (short) ((ilat+1)*(nblon+1) + ilon+0);
                indexlist[i++] = (short) ((ilat+1)*(nblon+1) + ilon+1);
            }
        }
        m_IndexBufferId = Utils.makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_IndexBufferSize = i;
    }


    /**
     * définit la position de la lampe
     * @param position : vec4 donnant les coordonnées de la lampe
     */
    void setLightPosition(vec4 position)
    {
        glUseProgram(m_ShaderId);
        vec4.glUniform(m_LightPositionLoc, position);
    }


    /**
     * dessiner l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // activer le shader
        glUseProgram(m_ShaderId);

        // fournir les textures
        m_TxDiffuseDay.setTextureUnit(GL_TEXTURE0, m_TxDiffuseDayLoc);
        m_TxDiffuseNight.setTextureUnit(GL_TEXTURE1, m_TxDiffuseNightLoc);
        m_TxSpecular.setTextureUnit(GL_TEXTURE2, m_TxSpecularLoc);

        // fournir les variables uniform au shader
        mat4.glUniformMatrix(m_MatMVLoc, mat4ModelView);
        mat4.glUniformMatrix(m_MatPLoc, mat4Projection);

        // calculer la matrice normale (coûteux)
        mat3 normalMatrix = mat3.create();
        mat3.fromMat4(normalMatrix, mat4ModelView);
        mat3.invert(normalMatrix, normalMatrix);
        mat3.transpose(normalMatrix, normalMatrix);
        mat3.glUniformMatrix(m_MatNLoc, normalMatrix);

        // activer et lier le buffer contenant les coordonnées
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glEnableVertexAttribArray(m_VertexLoc);
        glVertexAttribPointer(m_VertexLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les normales
        glBindBuffer(GL_ARRAY_BUFFER, m_NormalBufferId);
        glEnableVertexAttribArray(m_NormalLoc);
        glVertexAttribPointer(m_NormalLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les coords de texture
        glBindBuffer(GL_ARRAY_BUFFER, m_TexcoordBufferId);
        glEnableVertexAttribArray(m_TexCoordLoc);
        glVertexAttribPointer(m_TexCoordLoc, Utils.VEC2, GL_FLOAT, false, 0, 0);

        // activer le buffer contenant les indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

        // dessiner l'objet
        glDrawElements(GL_TRIANGLES, m_IndexBufferSize, GL_UNSIGNED_SHORT, 0);

        // désactiver les buffers
        glDisableVertexAttribArray(m_VertexLoc);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // désactiver le shader
        glUseProgram(0);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        Utils.deleteVBO(m_IndexBufferId);
        Utils.deleteVBO(m_TexcoordBufferId);
        Utils.deleteVBO(m_NormalBufferId);
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteShaderProgram(m_ShaderId);
        m_TxSpecular.destroy();
        m_TxDiffuseNight.destroy();
        m_TxDiffuseDay.destroy();
    }
}
