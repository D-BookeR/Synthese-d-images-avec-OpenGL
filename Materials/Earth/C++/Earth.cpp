// Définition de la classe Triangle

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>

#include <utils.h>

#include <Earth.h>



/**
 * Constructeur
 * @param nblon : nombre de méridiens
 * @param nblat : nombre de parallèles
 */
Earth::Earth(int nblon, int nblat) throw (std::string)
{
    /// Textures
    m_TxDiffuseDay = new Texture2D("data/textures/earth/earth_map.jpg");
    m_TxDiffuseNight = new Texture2D("data/textures/earth/earth_lights.jpg");
    m_TxSpecular  = new Texture2D("data/textures/earth/earth_spec.jpg");

    /** shader */

    const char* Phong_vs = "#version 300 es                     \n\
        uniform mat4 matMV;                                     \n\
        uniform mat4 matProjection;                             \n\
        uniform mat3 matNormal;                                 \n\
        in vec3 glVertex;                                \n\
        in vec3 glNormal;                                \n\
        in vec2 glTexCoord;                              \n\
        out vec3 frgNormal;                                     \n\
        out vec4 frgPosition;                                   \n\
        out vec2 frgTexCoord;                                   \n\
        void main()                                             \n\
        {                                                       \n\
            frgPosition = matMV * vec4(glVertex, 1.0);          \n\
            gl_Position = matProjection * frgPosition;          \n\
            frgNormal =  matNormal * glNormal;                  \n\
            frgTexCoord = glTexCoord;                           \n\
        }";

    const char* Phong_fs = "#version 300 es                     \n\
        precision mediump float;                                \n\
        uniform vec4 LightPosition;                             \n\
        uniform sampler2D diffuseJour;                          \n\
        uniform sampler2D diffuseNuit;                          \n\
        uniform sampler2D speculaire;                           \n\
        in vec3 frgNormal;                                      \n\
        in vec4 frgPosition;                                    \n\
        in vec2 frgTexCoord;                                    \n\
        out vec4 glFragColor;                                   \n\
        void main()                                             \n\
        {                                                       \n\
            vec3 N =  normalize(frgNormal);                     \n\
            vec3 L =  normalize(                                \n\
                vec3(LightPosition-frgPosition));               \n\
            vec3 R = reflect(normalize(frgPosition.xyz), N);    \n\
            float NL = dot(N,L);                                \n\
            float k = smoothstep(-0.2, 0.2, NL);                \n\
            NL = clamp(NL,0.0,1.0);                             \n\
            vec4 Jour = texture(diffuseJour, frgTexCoord);      \n\
            vec4 Nuit = texture(diffuseNuit, frgTexCoord);      \n\
            vec3 Kd = mix(Nuit.rgb, Jour.rgb*NL, k);            \n\
            float RL = clamp(dot(R,L), 0.0, 1.0);               \n\
            vec3 Ks = texture(speculaire, frgTexCoord).rgb;     \n\
            vec3 color = Kd + Ks*pow(RL, 256.0);                \n\
            glFragColor = vec4(color,1);                       \n\
        }";


    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(Phong_vs, Phong_fs, "shader terre");

    // déterminer où sont les variables attribute
    m_VertexLoc   = glGetAttribLocation(m_ShaderId, "glVertex");
    m_NormalLoc   = glGetAttribLocation(m_ShaderId, "glNormal");
    m_TexCoordLoc = glGetAttribLocation(m_ShaderId, "glTexCoord");

    // déterminer où sont les variables uniform
    m_MatMVLoc          = glGetUniformLocation(m_ShaderId, "matMV");
    m_MatPLoc           = glGetUniformLocation(m_ShaderId, "matProjection");
    m_MatNLoc           = glGetUniformLocation(m_ShaderId, "matNormal");
    m_LightPositionLoc  = glGetUniformLocation(m_ShaderId, "LightPosition");
    m_TxDiffuseDayLoc   = glGetUniformLocation(m_ShaderId, "diffuseJour");
    m_TxDiffuseNightLoc = glGetUniformLocation(m_ShaderId, "diffuseNuit");
    m_TxSpecularLoc     = glGetUniformLocation(m_ShaderId, "speculaire");

    // créer et remplir le buffer des vertices et des normales
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    float dlat = 180.0/nblat;
    float dlon = 360.0/nblon;
    for (int ilat=0; ilat<=nblat; ilat++) {
        float coslat = cos(Utils::radians(ilat*dlat-90));
        float sinlat = sin(Utils::radians(ilat*dlat-90));
        for (int ilon=0; ilon<=nblon; ilon++) {
            float coslon = cos(Utils::radians(ilon*dlon));
            float sinlon = sin(Utils::radians(ilon*dlon));
            // ajouter les coordonnées du point (lon,lat)
            vertices.push_back(coslat*coslon);
            vertices.push_back(sinlat);
            vertices.push_back(coslat*sinlon);
            // ajouter la normale au point (lon,lat)
            normals.push_back(coslat*coslon);
            normals.push_back(sinlat);
            normals.push_back(coslat*sinlon);
            // ajouter les coordonnées de texture
            texcoords.push_back(1.0 - (ilon+0.0)/nblon);
            texcoords.push_back((ilat+0.0)/nblat);
        }
    }
    m_VertexBufferId   = Utils::makeFloatVBO(vertices,  GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    m_NormalBufferId   = Utils::makeFloatVBO(normals,  GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    m_TexcoordBufferId = Utils::makeFloatVBO(texcoords, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des indices
    std::vector<GLushort> indexlist;
    for (int ilat=0; ilat<nblat; ilat++) {
        for (int ilon=0; ilon<nblon; ilon++) {
            // ajouter deux triangles
            indexlist.push_back((ilat+0)*(nblon+1) + ilon+1);
            indexlist.push_back((ilat+0)*(nblon+1) + ilon+0);
            indexlist.push_back((ilat+1)*(nblon+1) + ilon+0);

            indexlist.push_back((ilat+0)*(nblon+1) + ilon+1);
            indexlist.push_back((ilat+1)*(nblon+1) + ilon+0);
            indexlist.push_back((ilat+1)*(nblon+1) + ilon+1);
        }
    }
    m_IndexBufferId = Utils::makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    m_IndexBufferSize = indexlist.size();
}


/**
 * définit la position de la lampe
 * @param position : vec4 donnant les coordonnées de la lampe
 */
void Earth::setLightPosition(vec4& position)
{
    glUseProgram(m_ShaderId);
    vec4::glUniform(m_LightPositionLoc, position);
}


/**
 * dessiner l'objet
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Earth::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // fournir les textures
    m_TxDiffuseDay->setTextureUnit(GL_TEXTURE0, m_TxDiffuseDayLoc);
    m_TxDiffuseNight->setTextureUnit(GL_TEXTURE1, m_TxDiffuseNightLoc);
    m_TxSpecular->setTextureUnit(GL_TEXTURE2, m_TxSpecularLoc);

    // fournir les variables uniform au shader
    mat4::glUniformMatrix(m_MatMVLoc, mat4ModelView);
    mat4::glUniformMatrix(m_MatPLoc, mat4Projection);

    // calculer la matrice normale (coûteux)
    mat3 normalMatrix = mat3::create();
    mat3::fromMat4(normalMatrix, mat4ModelView);
    mat3::invert(normalMatrix, normalMatrix);
    mat3::transpose(normalMatrix, normalMatrix);
    mat3::glUniformMatrix(m_MatNLoc, normalMatrix);

    // activer et lier le buffer contenant les coordonnées
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glEnableVertexAttribArray(m_VertexLoc);
    glVertexAttribPointer(m_VertexLoc, Utils::VEC3, GL_FLOAT, false, 0, 0);

    // activer et lier le buffer contenant les normales
    glBindBuffer(GL_ARRAY_BUFFER, m_NormalBufferId);
    glEnableVertexAttribArray(m_NormalLoc);
    glVertexAttribPointer(m_NormalLoc, Utils::VEC3, GL_FLOAT, false, 0, 0);

    // activer et lier le buffer contenant les coords de texture
    glBindBuffer(GL_ARRAY_BUFFER, m_TexcoordBufferId);
    glEnableVertexAttribArray(m_TexCoordLoc);
    glVertexAttribPointer(m_TexCoordLoc, Utils::VEC2, GL_FLOAT, false, 0, 0);

    // activer le buffer contenant les indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

    // dessiner l'objet
    glDrawElements(GL_TRIANGLES, m_IndexBufferSize, GL_UNSIGNED_SHORT, NULL);

    // désactiver les buffers
    glDisableVertexAttribArray(m_VertexLoc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // désactiver le shader
    glUseProgram(0);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Earth::~Earth()
{
    Utils::deleteVBO(m_IndexBufferId);
    Utils::deleteVBO(m_TexcoordBufferId);
    Utils::deleteVBO(m_NormalBufferId);
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteShaderProgram(m_ShaderId);
    delete m_TxSpecular;
    delete m_TxDiffuseNight;
    delete m_TxDiffuseDay;
}
