#ifndef MATERIAL_VBOSET_H
#define MATERIAL_VBOSET_H

// Définition des classes VBOset et VBOvar

#include <vector>
#include <map>
#include <list>
#include <string>

#include <gl-matrix.h>
#include <utils.h>

#include <Mesh.h>


/**
 * Définit la classe VBOvar. Elle associe un VBO à une variable
 * attribute d'un shader
 */
class VBOvar
{
private:

    /// nom de la variable attribute
    std::string m_Name;

    /// emplacement de cet variable attribute dans le shader
    GLint m_AttrLoc;

    /// identifiant de l'attribut de vertex concerné, ex: MeshVertex.ID_ATTR_VERTEX
    int m_IdAttr;

    /// nombre de composantes GL_FLOAT de cet attribut
    GLint m_ComponentsNumber;

    /// identifiant du VBO à lier avec la variable attribute
    GLuint m_VBOId;

    /// vrai si le VBO est spécifique (cas de VBO multiples)
    bool m_IsOwned;

    /// décalage des données de la variable dans un VBO entrelacé
#ifdef __x86_64__
    GLulong m_Offset;
#else
    GLuint m_Offset;
#endif

    /// tableau temporaire servant à construire le VBO multiple
    std::vector<GLfloat> m_Data;


public:

    /**
     * constructeur de la classe VBOvar
     * @param shaderId : identifiant du shader employant cette variable attribute
     * @param name : nom de la variable attribute dans le shader
     * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
     * @param components : nombre de composantes float de la variable
     */
    VBOvar(GLint shaderId, std::string name, int idattr, int type);

    /**
     * destructeur
     */
    ~VBOvar();

    /**
     * retourne l'emplacement de la variable dans le shader
     * @return emplacement
     */
     GLuint getAttrLoc()
     {
         return m_AttrLoc;
     }

    /**
     * retourne l'identifiant de l'attribut
     * @return identifiant d'attribut
     */
     GLuint getIdAttr()
     {
         return m_IdAttr;
     }

    /**
     * retourne le nombre de composantes
     * @return composantes
     */
     GLint getComponentsCount()
     {
         return m_ComponentsNumber;
     }

    /**
     * retourne l'identifiant de VBO
     * @return identifiant OpenGL
     */
     GLuint getId()
     {
         return m_VBOId;
     }

    /**
     * retourne le tableau des données de la variable
     * @return vecteur de GLfloat
     */
     std::vector<GLfloat>& getData()
     {
         return m_Data;
     }

    /**
     * modifie l'identifiant de VBO
     * @param id : identifiant OpenGL
     */
    void setId(GLuint id);

    /**
     * modifie le booléen perso
     * @param owned : true si le VBO est créé par cette variable, false s'il est partagé
     */
    void setIsOwned(bool owned);

    /**
     * modifie l'offset de la variable
     * @param offset à affecter
     */
#ifdef __x86_64__
    void setOffset(GLulong offset);
#else
    void setOffset(GLuint offset);
#endif

    /**
     * vide le tableau m_Data
     */
    void clear();

    /**
     * ajoute une valeur au tableau m_Data
     * @param value : valeur à ajouter en fin
     */
    void push(float value);

    /**
     * Cette méthode effectue la liaison entre une variable attribute et un VBO
     * @param stride : grand pas des données dans le VBO
     */
    void bindVBO(int stride);

    /**
     * Désactive le VBO concerné
     */
    void unbindVBO();

    /**
     * met à jour les emplacements de la variable suite à une recompilation du shader
     * @param shaderId : identifiant du shader employant cette variable attribute
     */
    void updateShader(GLint shaderId);

};


/**
 * Définit la classe VBOset. Elle permet d'associer des VBO à des
 * variables attribute de shader
 */
class VBOset
{
private:

    /// matériau à activer pour dessiner ce VBOset
    Material* m_Material;

    /// variables attributes gérées par ce VBOset
    std::vector<VBOvar*> m_VBOvariables;

    /// grand pas global
    GLint m_VBOdataStride;

    /// identifiant du VBO des indices
    GLint m_IndexBufferId;

    /// nombre d'indices à dessiner
    int m_IndexBufferSize;

    /// primitive à employer pour dessiner, ex: GL_TRIANGLES
    GLenum m_DrawingPrimitive;

    /// identifiant du VAO
    GLuint m_VAO;


public:

    /**
     * constructeur de la classe VBOset
     * @param materiau : celui avec lequel on va dessiner ce VBOset
     */
    VBOset(Material* material);

    /**
     * destructeur
     */
     ~VBOset();

    /**
     * Cette méthode définit le type des informations pour une variable attribut
     * @param idattr : voir la classe MeshVertex et getAttribute, par exemple MeshVertex::ATTR_ID_VERTEX, MeshVertex::ATTR_ID_COLOR
     * @param components : ex: 2 ou mieux Utils::VEC2
     * @param name : nom de la variable concernée ex: "glTexCoord"
     */
    void addAttribute(int idattr, int components, std::string name);

    /**
     * ajoute les composantes du vertex au tableau data, s'il est fourni (VBO entrelacé), sinon
     * les composantes sont mises dans les tableaux m_Data des VBOvars du VBOset (VBO multiples)
     * @param vertex : sommet dont il faut ajouter les informations au VBO
     * @param data : tableau contenant les composantes
     */
    void appendVertexComponents(MeshVertex* vertex, std::vector<GLfloat>& data);
    void appendVertexComponents(MeshVertex* vertex);

    /**
     * Cette méthode crée le VBO contenant les attributs entrelacés
     * @param data : données à placer dans le VBO entrelacé
     */
    void createInterleavedDataAttributesVBO(std::vector<GLfloat>& data);

    /**
     * Cette méthode crée les VBO des attributs entrelacés
     * @param mesh : fournit la liste des sommets à placer dans les VBO
     */
    void createInterleavedAttributesVBO(Mesh* mesh);

    /**
     * Cette méthode crée les VBO des attributs multiples
     * @param mesh : fournit la liste des sommets à placer dans les VBO
     */
    void createMultipleAttributesVBO(Mesh* mesh);

    /**
     * Cette méthode crée les VBO des attributs : coordonnées, couleurs, normales et coordonnées de texture
     * @param mesh : fournit la liste des sommets à placer dans les VBO
     * @param interleaved : true (par défaut) s'il faut entrelacer les données
     */
    void createAttributesVBO(Mesh* mesh, bool interleaved=true);

    /**
     * Cette méthode initialise le VBOset pour dessiner la primitive sans indices
     * @param primitive : par exemple GL_TRIANGLES
     * @param number : nombre de données à dessiner
     */
    void createDirectPrimitiveVBO(int primitive, int number);

    /**
     * Cette méthode initialise le VBOset pour dessiner la primitive avec les indices
     * @param primitive : par exemple GL_TRIANGLES
     * @param indexlist : tableau des indices
     * @return nombre d'indices présents dans le VBO
     */
    int createIndexedPrimitiveVBO(int primitive, std::vector<GLushort>& indexlist);

    /**
     * Cette méthode active les VBOs et fait la liaison avec les attribute du shader
     */
    void enable();

    /**
     * Cette méthode désactive les VBOs
     */
    void disable();

    /**
     * Cette méthode dessine le VBOset, avec les éléments demandés :
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * met à jour les emplacements des variables du VBOset suite à une recompilation du shader
     */
    void updateLocations();

};


#endif
