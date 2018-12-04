/**
 * La classe VBOset représente un groupe de VBO, soit un seul quand les données sont entrelacées
 * soit plusieurs quand les données sont séparées. Chaque donnée est représentée par un VBOvar.
 * Elle représente une variable attribute d'un shader : nom, emplacement et informations pour
 * la lier correctement au VBO.
 */


package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.utils.Utils;


@SuppressWarnings("unused")
public class VBOset
{
    private Material m_Material;                // matériau à activer pour dessiner ce VBOset
    private ArrayList<VBOvar> m_VBOvariables;   // variables attribute qui sont gérées par ce VBOset
    private int m_VBOdataStride;                // grand pas global
    private int m_IndexBufferId;                // identifiant du VBO des indices
    private int m_IndexBufferType;              // type des entiers du VBO des indices, GL_UNSIGNED_SHORT ou GL_UNSIGNED_INT
    private int m_IndexBufferSize;              // nombre d'indices à dessiner
    private int m_DrawingPrimitive;             // primitive à employer pour dessiner, ex: gl.TRIANGLES

    @SuppressWarnings("unused")
    class VBOvar
    {
        private String m_Name;              // nom de la variable dans le shader
        private int m_IdAttr;               // identifiant de l'attribut de sommet
        private int m_ComponentsNumber;     // nombre de float, ex: 3 pour un vec3
        private int m_AttrLoc;              // emplacement de la variable dans le shader
        private int m_VBOId;                // identifiant OpenGL du VBO à lier
        private boolean m_IsOwned;          // vrai si le VBO est spécifique (cas de VBO multiples)
        private int m_Offset;               // décalage, cas d'un VBO entrelacé
        private ArrayList<Float> m_Data;    // tableau temporaire pour stocker les données


        /**
         * constructeur de la classe VBOvar
         * @param shaderId : identifiant du shader employant cette variable attribute
         * @param name : nom de la variable attribute dans le shader
         * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
         * @param components : nombre de composantes float de la variable
         */
        VBOvar(int shaderId, String name, int idattr, int components)
        {
            m_Name = name;
            m_IdAttr = idattr;
            m_ComponentsNumber = components;
            m_AttrLoc  = -1;
            m_VBOId = -1;
            m_IsOwned = false;
            m_Offset = 0;
            m_Data = new ArrayList<>();

            // il est possible que cette variable ne soit pas présente dans le shader
            // mais on va quand même créer le VBO associé au cas où le shader soit recompilé plus tard
            if (shaderId > 0) {
                m_AttrLoc = glGetAttribLocation(shaderId, name);
                //if (m_AttrLoc < 0) {
                //    m_ComponentsNumber = 0;
                //} else {
                //    m_ComponentsNumber = components;
                //}
            }
        }


        /** destructeur */
        public void destroy()
        {
            // libérer le VBO sauf s'il est copié
            if (m_IsOwned) {
                Utils.deleteVBO(m_VBOId);
            }
        }


        /**
         * retourne l'emplacement de la variable dans le shader
         * @return emplacement
         */
        public int getAttrLoc()
        {
            return m_AttrLoc;
        }


        /**
         * retourne l'identifiant de l'attribut
         * @return identifiant d'attribut
         */
        public int getIdAttr()
        {
            return m_IdAttr;
        }


        /**
         * retourne le nombre de composantes
         * @return composantes
         */
        public int getComponentsCount()
        {
            return m_ComponentsNumber;
        }


        /**
         * retourne l'identifiant de VBO
         * @return identifiant OpenGL
         */
        public int getId()
        {
            return m_VBOId;
        }


        /**
         * retourne le tableau des données de la variable
         * @return vecteur de GLfloat
         */
        public ArrayList<Float> getData()
        {
            return m_Data;
        }


        /**
         * modifie l'identifiant de VBO
         * @param id : identifiant OpenGL
         */
        void setId(int id)
        {
            m_VBOId = id;
        }


        /**
         * modifie le booléen perso
         * @param owned : true si le VBO est créé par cette variable, false s'il est partagé
         */
        void setIsOwned(boolean owned)
        {
            m_IsOwned = owned;
        }


        /**
         * modifie l'offset de la variable
         * @param offset à affecter
         */
        void setOffset(int offset)
        {
            m_Offset = offset;
        }


        /**
         * vide le tableau m_Data
         */
        void clear()
        {
            m_Data.clear();
        }


        /**
         * ajoute une valeur au tableau m_Data
         * @param value : valeur à ajouter en fin
         */
        void push(float value)
        {
            m_Data.add(value);
        }


        /**
         * Cette méthode effectue la liaison entre une variable attribute et un VBO
         * @param stride : grand pas des données dans le VBO
         */
        void bindVBO(int stride)
        {
            if (m_AttrLoc >= 0 && m_VBOId >= 0) {
                glBindBuffer(GL_ARRAY_BUFFER, m_VBOId);
                glEnableVertexAttribArray(m_AttrLoc);
                glVertexAttribPointer(m_AttrLoc, m_ComponentsNumber, GL_FLOAT, false, stride, m_Offset);
            }
        }


        /**
         * Désactive le VBO concerné
         */
        void unbindVBO()
        {
            if (m_AttrLoc != -1) {
                glDisableVertexAttribArray(m_AttrLoc);
            }
        }


        /**
         * met à jour les emplacements de la variable suite à une recompilation du shader
         * @param shaderId : identifiant du shader employant cette variable attribute
         */
        void updateShader(int shaderId)
        {
            m_AttrLoc = glGetAttribLocation(shaderId, m_Name);
        }
    }


    /**
     * constructeur de la classe VBOset
     * @param material : matériau avec lequel on va dessiner ce VBOset
     */
    public VBOset(Material material)
    {
        m_Material = material;
        m_VBOvariables = new ArrayList<>();
        m_VBOdataStride = 0;
        m_IndexBufferId = -1;
        m_IndexBufferSize = 0;
        m_DrawingPrimitive = GL_POINTS;
    }


    /** destructeur */
    public void destroy()
    {
        // libérer les VBOvar qui ont été allouées
        for (VBOvar vbovar: m_VBOvariables) {
            vbovar.destroy();
        }
    }


    /**
     * Cette méthode définit le type des informations pour une variable attribut
     * @param idattr : voir la classe MeshVertex et getAttribute, par exemple MeshVertex.ID_ATTR_VERTEX, MeshVertex.ID_ATTR_COLOR
     * @param components : ex: 2 ou mieux Utils.VEC2
     * @param name : nom de la variable concernée ex: "glTexCoord"
     */
    public void addAttribute(int idattr, int components, String name)
    {
        m_VBOvariables.add(new VBOvar(m_Material.getShaderId(), name, idattr, components));
    }


    /**
     * ajoute les composantes du vertex au tableau data, s'il est fourni (VBO entrelacé), sinon
     * les composantes sont mises dans les tableaux m_Data des VBOvars du VBOset (VBO multiples)
     * @param vertex sommet dont il faut ajouter les informations au VBO
     * @param data : s'il est null ou absent, alors c'est dans vbovar.m_Data que sont mises les composantes
     */
    public void appendVertexComponents(MeshVertex vertex, ArrayList<Float> data)
    {
        for (VBOvar vbovar: m_VBOvariables) {

            // nombre de composantes de cette variable (éventuellement 0 si pas utilisée)
            int components = vbovar.getComponentsCount();
            if (components <= 0) continue;

            // valeur de cette variable sur le sommet courant
            vec4 value = vertex.getAttribute(vbovar.getIdAttr());
            if (value == null) continue;

            // ajouter les coordonnées s'il le faut
            for (int i=0; i<components; i++) {
                if (data == null) {
                    vbovar.push(value.get(i));
                } else {
                    data.add(value.get(i));
                }
            }
        }
    }


    /**
     * Cette méthode crée le VBO contenant les attributs entrelacés
     * @param data : données à placer dans le VBO entrelacé
     */
    public void createInterleavedDataAttributesVBO(ArrayList<Float> data)
    {
        // allouer le VBO
        int VBOid = Utils.makeFloatVBO(data, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // calculer le grand pas global, mettre à jour les décalages
        m_VBOdataStride = 0;
        for (VBOvar vbovar: m_VBOvariables) {
            vbovar.setId(VBOid);
            vbovar.setIsOwned((m_VBOdataStride == 0));
            vbovar.setOffset(m_VBOdataStride);
            m_VBOdataStride += vbovar.getComponentsCount() * Utils.SIZEOF_FLOAT;
        }
    }


    /**
     * Cette méthode crée les VBO des attributs entrelacés
     * @param mesh : fournit la liste des sommets à placer dans les VBO
     */
    private void createInterleavedAttributesVBO(Mesh mesh)
    {
        // rassembler les coordonnées, couleurs, normales et coordonnées de texture
        ArrayList<Float> data = new ArrayList<>();
        int iv = 0;
        for (MeshVertex vertex: mesh.getVertexList()) {
            // renuméroter le sommet (numéro dans les VBOs)
            vertex.setNumber(iv);
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
    private void createMultipleAttributesVBO(Mesh mesh)
    {
        // rassembler les coordonnées, couleurs et/ou normales demandées
        int iv = 0;
        for (MeshVertex vertex: mesh.getVertexList()) {
            // renuméroter le sommet (numéro dans les VBOs)
            vertex.setNumber(iv);
            iv++;

            // ajouter les valeurs des variables attributs (VBOvar)
            appendVertexComponents(vertex, null);
        }

        // calculer le grand pas global, mettre à jour les décalages et fournir le VBO
        m_VBOdataStride = 0;
        for (VBOvar vbovar: m_VBOvariables) {
            vbovar.setId(Utils.makeFloatVBO(vbovar.getData(), GL_ARRAY_BUFFER, GL_STATIC_DRAW));
            vbovar.setIsOwned(true);
            vbovar.setOffset(0);
            vbovar.clear();
        }
    }


    /**
     * Cette méthode crée les VBO des attributs : coordonnées, couleurs, normales et coordonnées de texture
     * @param mesh : fournit la liste des sommets à placer dans les VBO
     * @param interleaved : true (par défaut) s'il faut entrelacer les données
     */
    public void createAttributesVBO(Mesh mesh, boolean interleaved)
    {
        if (interleaved) {
            createInterleavedAttributesVBO(mesh);
        } else {
            createMultipleAttributesVBO(mesh);
        }
    }
    public void createAttributesVBO(Mesh mesh)
    {
        createInterleavedAttributesVBO(mesh);
    }


    /**
     * Cette méthode initialise le VBOset pour dessiner la primitive sans indices
     * @param primitive : par exemple GL_TRIANGLES
     * @param number : nombre de données à dessiner
     */
    public void createDirectPrimitiveVBO(int primitive, int number)
    {
        m_DrawingPrimitive = primitive;
        m_IndexBufferId = -1;
        m_IndexBufferSize = number;
    }


    /**
     * Cette méthode initialise le VBOset pour dessiner la primitive avec les indices
     * @param primitive : par exemple GL_TRIANGLES
     * @param indexlist : tableau des indices
     * @return nombre d'indices présents dans le VBO
     */
    public int createIndexedPrimitiveVBO(int primitive, int[] indexlist)
    {
        m_DrawingPrimitive = primitive;
        m_IndexBufferSize = indexlist.length;
        if (m_IndexBufferSize > 65534) {
            m_IndexBufferId = Utils.makeIntVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
            m_IndexBufferType = GL_UNSIGNED_INT;
        } else {
            m_IndexBufferId = Utils.makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
            m_IndexBufferType = GL_UNSIGNED_SHORT;
        }
        return m_IndexBufferSize;
    }
    public int createIndexedPrimitiveVBO(int primitive, ArrayList<Integer> indexlist)
    {
        m_DrawingPrimitive = primitive;
        m_IndexBufferSize = indexlist.size();
        if (m_IndexBufferSize > 65534) {
            m_IndexBufferId = Utils.makeIntVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
            m_IndexBufferType = GL_UNSIGNED_INT;
        } else {
            m_IndexBufferId = Utils.makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
            m_IndexBufferType = GL_UNSIGNED_SHORT;
        }
        return m_IndexBufferSize;
    }


    /**
     * Cette méthode active les VBOs et fait la liaison avec les attribute du shader
     */
    private void enable()
    {
        // activer et lier les VBOs liés aux variables attribute
        for (VBOvar vbovar: m_VBOvariables) {
            vbovar.bindVBO(m_VBOdataStride);
        }

        // NB : avec un VAO, il suffit d'activer le VAO
    }


    /**
     * Cette méthode désactive les VBOs
     */
    private void disable()
    {
        // activer et lier les VBOs liés aux variables attribute
        for (VBOvar vbovar: m_VBOvariables) {
            vbovar.unbindVBO();
        }

        // NB : avec un VAO, il suffit de désactiver le VAO
    }


    /**
     * Cette méthode dessine le VBOset, avec les éléments demandés :
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        if (m_IndexBufferSize <= 0) return;

        // activer le matériau (shader <-> VBOs)
        m_Material.enable(mat4Projection, mat4ModelView);

        /*** MODE MRT SIMULÉ ***/
        // mettre à jour les identifiants des variables attribute à cause du changement de shader
        updateLocations();
        /*** MODE MRT SIMULÉ ***/

        // activer et lier les VBO
        enable();

        // dessin indexé ?
        if (m_IndexBufferId >= 0) {

            // liaison du VBO des indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

            // dessin des triangles
            glDrawElements(m_DrawingPrimitive, m_IndexBufferSize, m_IndexBufferType, 0);

            // libération du VBO des indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        } else {

            // dessin non indexé
            glDrawArrays(m_DrawingPrimitive, 0, m_IndexBufferSize);
        }

        // libération du shader et des autres VBOs
        disable();
        m_Material.disable();
    }


    /**
     * met à jour les emplacements des variables du VBOset suite à une recompilation du shader
     */
    public void updateLocations()
    {
        for (VBOvar vbovar: m_VBOvariables) {
            vbovar.updateShader(m_Material.getShaderId());
        }
    }
}
