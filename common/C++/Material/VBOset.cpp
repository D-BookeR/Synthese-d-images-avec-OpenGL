/**
 * La classe VBOset représente un groupe de VBO, soit un seul quand les données sont entrelacées
 * soit plusieurs quand les données sont séparées. Chaque donnée est représentée par un VBOvar.
 * Elle représente une variable attribute d'un shader : nom, emplacement et informations pour
 * la lier correctement au VBO.
 */


#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdexcept>

#include <utils.h>
#include <VBOset.h>



/**
 * constructeur de la classe VBOvar
 * @param shaderId : identifiant du shader employant cette variable attribute
 * @param name : nom de la variable attribute dans le shader
 * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
 * @param components : nombre de composantes float de la variable
 */
VBOvar::VBOvar(GLint shaderId, std::string name, int idattr, int components)
{
    m_Name = name;
    m_IdAttr = idattr;
    m_ComponentsNumber = components;
    m_AttrLoc  = -1;
    m_VBOId = -1;
    m_IsOwned = false;
    m_Offset = 0;

    // il est possible que cette variable ne soit pas présente dans le shader
    // mais on va quand même créer le VBO associé au cas où le shader soit recompilé plus tard
    m_AttrLoc  = glGetAttribLocation(shaderId, name.c_str());
    //if (m_AttrLoc < 0) {
    //    m_ComponentsNumber = 0;
    //} else {
    //    m_ComponentsNumber = components;
    //}
}


/** destructeur */
VBOvar::~VBOvar()
{
    // libérer le VBO sauf s'il est copié
    if (m_IsOwned) {
        Utils::deleteVBO(m_VBOId);
    }
}


/**
 * modifie l'identifiant de VBO
 * @param id : identifiant OpenGL
 */
void VBOvar::setId(GLuint id)
{
    m_VBOId = id;
}


/**
 * modifie le booléen perso
 * @param owned : true si le VBO est créé par cette variable, false s'il est partagé
 */
void VBOvar::setIsOwned(bool owned)
{
    m_IsOwned = owned;
}


/**
 * modifie l'offset de la variable
 * @param offset à affecter
 */
#ifdef __x86_64__
void VBOvar::setOffset(GLulong offset)
#else
void VBOvar::setOffset(GLuint offset)
#endif
{
    m_Offset = offset;
}


/**
 * vide le tableau m_Data
 */
void VBOvar::clear()
{
    m_Data.clear();
}


/**
 * ajoute une valeur au tableau m_Data
 * @param value : valeur à ajouter en fin
 */
void VBOvar::push(float value)
{
    m_Data.push_back(value);
}


/**
 * Cette méthode effectue la liaison entre une variable attribute et un VBO
 * @param stride : grand pas des données dans le VBO
 */
void VBOvar::bindVBO(int stride)
{
    if (m_AttrLoc != -1 && m_VBOId != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBOId);
        glEnableVertexAttribArray(m_AttrLoc);
        glVertexAttribPointer(m_AttrLoc, m_ComponentsNumber, GL_FLOAT, GL_FALSE, stride, (const GLvoid*) m_Offset);
    }
}


/**
 * Désactive le VBO concerné
 */
void VBOvar::unbindVBO()
{
    if (m_AttrLoc != -1) {
        glDisableVertexAttribArray(m_AttrLoc);
    }
}


/**
 * met à jour les emplacements de la variable suite à une recompilation du shader
 * @param shaderId : identifiant du shader employant cette variable attribute
 */
void VBOvar::updateShader(GLint shaderId)
{
    m_AttrLoc = glGetAttribLocation(shaderId, m_Name.c_str());
}



/**
 * constructeur de la classe VBOset
 * @param material : matériau avec lequel on va dessiner ce VBOset
 */
VBOset::VBOset(Material* material)
{
    m_Material = material;
    m_VBOdataStride = 0;
    m_IndexBufferId = -1;
    m_IndexBufferSize = 0;
    m_DrawingPrimitive = GL_POINTS;
    m_VAO = -1;
}


/** destructeur */
VBOset::~VBOset()
{
    // libérer les VBOvar qui ont été allouées
    for (VBOvar* vbovar: m_VBOvariables) {
        delete vbovar;
    }

    // supprimer le VAO
    if (m_VAO >= 0) glDeleteVertexArrays(1, &m_VAO);
}


/**
 * Cette méthode définit le type des informations pour une variable attribut
 * @param idattr : voir la classe MeshVertex et getAttribute, par exemple MeshVertex::ATTR_ID_VERTEX, MeshVertex::ATTR_ID_COLOR
 * @param components : ex: 2 ou mieux Utils::VEC2
 * @param name : nom de la variable concernée ex: "glTexCoord"
 */
void VBOset::addAttribute(int idattr, int components, std::string name)
{
    m_VBOvariables.push_back(new VBOvar(m_Material->getShaderId(), name, idattr, components));
}


/**
 * ajoute les composantes du vertex au tableau data, s'il est fourni (VBO entrelacé), sinon
 * les composantes sont mises dans les tableaux m_Data des VBOvars du VBOset (VBO multiples)
 * @param vertex : sommet dont il faut ajouter les informations au VBO
 * @param data : tableau contenant les composantes
 */
void VBOset::appendVertexComponents(MeshVertex* vertex, std::vector<GLfloat>& data)
{
    for (VBOvar* vbovar: m_VBOvariables) {

        // nombre de composantes de cette variable (éventuellement 0 si pas utilisée)
        const int components = vbovar->getComponentsCount();
        if (components <= 0) continue;

        // valeur de cette variable sur le sommet courant
        vec4 value = vertex->getAttribute(vbovar->getIdAttr());
        if (value == vec4::null) continue;

        // ajouter les coordonnées s'il le faut
        for (int i=0; i<components; i++) {
            data.push_back(value[i]);
        }
    }
}
void VBOset::appendVertexComponents(MeshVertex* vertex)
{
    for (VBOvar* vbovar: m_VBOvariables) {

        // nombre de composantes de cette variable (éventuellement 0 si pas utilisée)
        const int components = vbovar->getComponentsCount();
        if (components <= 0) continue;

        // valeur de cette variable sur le sommet courant
        vec4 value = vertex->getAttribute(vbovar->getIdAttr());
        if (value == vec4::null) continue;

        // ajouter les coordonnées s'il le faut
        for (int i=0; i<components; i++) {
            vbovar->push(value[i]);
        }
    }
}


/**
 * Cette méthode crée le VBO contenant les attributs entrelacés
 * @param data : données à placer dans le VBO entrelacé
 */
void VBOset::createInterleavedDataAttributesVBO(std::vector<GLfloat>& data)
{
    // allouer le VBO
    GLint VBOid = Utils::makeFloatVBO(data,GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // calculer le grand pas global, mettre à jour les décalages
    m_VBOdataStride = 0;
    for (VBOvar* vbovar: m_VBOvariables) {
        vbovar->setId(VBOid);
        vbovar->setIsOwned((m_VBOdataStride == 0));
        vbovar->setOffset(m_VBOdataStride);
        m_VBOdataStride += vbovar->getComponentsCount() * Utils::SIZEOF_FLOAT;
    }
}


/**
 * Cette méthode crée les VBO des attributs entrelacés
 * @param mesh : fournit la liste des sommets à placer dans les VBO
 */
void VBOset::createInterleavedAttributesVBO(Mesh* mesh)
{
    // rassembler les coordonnées, couleurs, normales et coordonnées de texture
    std::vector<GLfloat> data;
    int iv = 0;
    for (MeshVertex* vertex: mesh->getVertexList()) {
        // renuméroter le sommet (numéro dans les VBOs)
        vertex->setNumber(iv);
        iv++;

        // ajouter les valeurs des variables attributs (VBOvar)
        appendVertexComponents(vertex, data);
    }

    // créer le VBO entrelacé
    createInterleavedDataAttributesVBO(data);
}


/**
 * Cette méthode crée les VBO des attributs multiples
 * @param mesh : fournit la liste des sommets à placer dans les VBO
 */
void VBOset::createMultipleAttributesVBO(Mesh* mesh)
{
    // rassembler les coordonnées, couleurs et/ou normales demandées
    int iv = 0;
    for (MeshVertex* vertex: mesh->getVertexList()) {
        // renuméroter le sommet (numéro dans les VBOs)
        vertex->setNumber(iv);
        iv++;

        // ajouter les valeurs des variables attributs (VBOvar)
        appendVertexComponents(vertex);
    }

    // calculer le grand pas global, mettre à jour les décalages et fournir le VBO
    m_VBOdataStride = 0;
    for (VBOvar* vbovar: m_VBOvariables) {
        vbovar->setId(Utils::makeFloatVBO(vbovar->getData(), GL_ARRAY_BUFFER, GL_STATIC_DRAW));
        vbovar->setIsOwned(true);
        vbovar->setOffset(0);
        vbovar->clear();
    }
}


/**
 * Cette méthode crée les VBO des attributs : coordonnées, couleurs, normales et coordonnées de texture
 * @param mesh : fournit la liste des sommets à placer dans les VBO
 * @param interleaved : true (par défaut) s'il faut entrelacer les données
 */
void VBOset::createAttributesVBO(Mesh* mesh, bool interleaved)
{
    // création des VBOs
    if (interleaved) {
        createInterleavedAttributesVBO(mesh);
    } else {
        createMultipleAttributesVBO(mesh);
    }
}


/**
 * Cette méthode initialise le VBOset pour dessiner la primitive sans indices
 * @param primitive : par exemple GL_TRIANGLES
 * @param number : nombre de données à dessiner
 */
void VBOset::createDirectPrimitiveVBO(int primitive, int number)
{
    // initialisations
    m_DrawingPrimitive = primitive;
    m_IndexBufferId = -1;
    m_IndexBufferSize = number;

    // création du VAO
    if (m_VAO >= 0) glDeleteVertexArrays(1, &m_VAO);
    glGenVertexArrays(1, &m_VAO);

    // activation du VAO
    glBindVertexArray(m_VAO);

    // activer et lier les VBOs liés aux variables attribute
    for (VBOvar* vbovar: m_VBOvariables) {
        vbovar->bindVBO(m_VBOdataStride);
    }

    // désactivation du VAO et des VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/**
 * Cette méthode initialise le VBOset pour dessiner la primitive avec les indices
 * @param primitive : par exemple GL_TRIANGLES
 * @param indexlist : tableau des indices
 * @return nombre d'indices présents dans le VBO
 */
int VBOset::createIndexedPrimitiveVBO(int primitive, std::vector<int>& indexlist)
{
    // initialisations
    m_DrawingPrimitive = primitive;
    m_IndexBufferSize = indexlist.size();
    if (m_IndexBufferSize > 65534) {
        m_IndexBufferId = Utils::makeIntVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_IndexBufferType = GL_UNSIGNED_INT;
    } else {
        m_IndexBufferId = Utils::makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
        m_IndexBufferType = GL_UNSIGNED_SHORT;
    }

    // création du VAO
    if (m_VAO >= 0) glDeleteVertexArrays(1, &m_VAO);
    glGenVertexArrays(1, &m_VAO);

    // activation du VAO
    glBindVertexArray(m_VAO);

    // activer et lier les VBOs liés aux variables attribute
    for (VBOvar* vbovar: m_VBOvariables) {
        vbovar->bindVBO(m_VBOdataStride);
    }

    // liaison du VBO des indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

    // désactivation du VAO et des VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return m_IndexBufferSize;
}


/**
 * Cette méthode active les VBOs et fait la liaison avec les attribute du shader
 */
void VBOset::enable()
{
    // avec un VAO, il suffit d'activer le VAO
    glBindVertexArray(m_VAO);
}


/**
 * Cette méthode désactive les VBOs
 */
void VBOset::disable()
{
    // avec un VAO, il suffit de désactiver le VAO
    glBindVertexArray(0);
}


/**
 * Cette méthode dessine le VBOset, avec les éléments demandés :
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void VBOset::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    if (m_IndexBufferSize <= 0) return;

    // activer le matériau (shader <-> VBOs)
    m_Material->enable(mat4Projection, mat4ModelView);

    // activer et lier les VBO
    enable();

    // dessin indexé ?
    if (m_IndexBufferId >= 0) {

        // dessin des triangles
        glDrawElements(m_DrawingPrimitive, m_IndexBufferSize, m_IndexBufferType, 0);

    } else {

        // dessin non indexé
        glDrawArrays(m_DrawingPrimitive, 0, m_IndexBufferSize);
    }

    // libération du shader et des autres VBOs
    disable();
    m_Material->disable();
}


/**
 * met à jour les emplacements des variables du VBOset suite à une recompilation du shader
 */
void VBOset::updateLocations()
{
    for (VBOvar* vbovar: m_VBOvariables) {
        vbovar->updateShader(m_Material->getShaderId());
    }
}
