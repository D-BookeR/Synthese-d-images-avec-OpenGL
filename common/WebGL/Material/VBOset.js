/**
 * La classe VBOset représente un groupe de VBO, soit un seul quand les données sont entrelacées
 * soit plusieurs quand les données sont séparées. Chaque donnée est représentée par un VBOvar.
 * Elle représente une variable attribute d'un shader : nom, emplacement et informations pour
 * la lier correctement au VBO.
 */


class VBOvar
{
    /** variables membre
    private String m_Name;              // nom de la variable dans le shader
    private int m_IdAttr;               // identifiant de l'attribut de sommet
    private int m_ComponentsNumber;     // nombre de float, ex: 3 pour un vec3
    private int m_AttrLoc;              // emplacement de la variable dans le shader
    private int m_VBOId;                // identifiant OpenGL du VBO à lier
    private boolean m_IsOwned;          // vrai si le VBO est spécifique (cas de VBO multiples)
    private int m_Offset;               // décalage, cas d'un VBO entrelacé
    private ArrayList<Float> m_Data;    // tableau temporaire pour stocker les données
    **/


    /**
     * constructeur de la classe VBOvar
     * @param shaderId : identifiant du shader employant cette variable attribute
     * @param name : nom de la variable attribute dans le shader
     * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
     * @param components : nombre de composantes float de la variable
     */
    constructor(shaderId, name, idattr, components)
    {
        this.m_Name = name;
        this.m_IdAttr = idattr;
        this.m_ComponentsNumber = components;
        this.m_AttrLoc  = -1;
        this.m_VBOId = -1;
        this.m_IsOwned = false;
        this.m_Offset = 0;
        this.m_Data = [];

        // il est possible que cette variable ne soit pas présente dans le shader
        // mais on va quand même créer le VBO associé au cas où le shader soit recompilé plus tard
        this.m_AttrLoc  = gl.getAttribLocation(shaderId, name);
        //if (this.m_AttrLoc < 0) {
        //    this.m_ComponentsNumber = 0;
        //} else {
        //    this.m_ComponentsNumber = components;
        //}
    }


    /** destructeur */
    destroy()
    {
        // libérer le VBO sauf s'il est copié
        if (this.m_IsOwned) {
            Utils.deleteVBO(this.m_VBOId);
        }
    }


    /**
     * retourne l'emplacement de la variable dans le shader
     * @return emplacement
     */
    getAttrLoc()
    {
        return this.m_AttrLoc;
    }


    /**
     * retourne l'identifiant de l'attribut
     * @return identifiant d'attribut
     */
    getIdAttr()
    {
        return this.m_IdAttr;
    }


    /**
     * retourne le nombre de composantes
     * @return composantes
     */
    getComponentsCount()
    {
        return this.m_ComponentsNumber;
    }


    /**
     * retourne l'identifiant de VBO
     * @return identifiant OpenGL
     */
    getId()
    {
        return this.m_VBOId;
    }


    /**
     * retourne le tableau des données de la variable
     * @return vecteur de GLfloat
     */
    getData()
    {
        return this.m_Data;
    }


    /**
     * modifie l'identifiant de VBO
     * @param id : identifiant OpenGL
     */
    setId(id)
    {
        this.m_VBOId = id;
    }


    /**
     * modifie le booléen perso
     * @param owned : true si le VBO est créé par cette variable, false s'il est partagé
     */
    setIsOwned(owned)
    {
        this.m_IsOwned = owned;
    }


    /**
     * modifie l'offset de la variable
     * @param offset à affecter
     */
    setOffset(offset)
    {
        this.m_Offset = offset;
    }


    /**
     * vide le tableau m_Data
     */
    clear()
    {
        this.m_Data = [];
    }


    /**
     * ajoute une valeur au tableau m_Data
     * @param value : valeur à ajouter en fin
     */
    push(value)
    {
        this.m_Data.push(value);
    }


    /**
     * Cette méthode effectue la liaison entre une variable attribute et un VBO
     * @param stride : grand pas des données dans le VBO
     */
    bindVBO(stride)
    {
        if (this.m_AttrLoc != -1 && this.m_VBOId != null) {
            gl.bindBuffer(gl.ARRAY_BUFFER, this.m_VBOId);
            gl.enableVertexAttribArray(this.m_AttrLoc);
            gl.vertexAttribPointer(this.m_AttrLoc, this.m_ComponentsNumber, gl.FLOAT, gl.FALSE, stride, this.m_Offset);
        }
    }


    /**
    * Désactive le VBO concerné
    */
    unbindVBO()
    {
        if (this.m_AttrLoc != -1) {
            gl.disableVertexAttribArray(this.m_AttrLoc);
        }
    }


    /**
     * met à jour les emplacements de la variable suite à une recompilation du shader
     * @param shaderId : identifiant du shader employant cette variable attribute
     */
    updateShader(shaderId)
    {
        this.m_AttrLoc = gl.getAttribLocation(shaderId, this.m_Name);
    }
}



class VBOset
{
    /** variables membre
    private Material m_Material;                // matériau à activer pour dessiner ce VBOset
    private ArrayList<VBOvar> m_VBOvariables;   // variables attribute qui sont gérées par ce VBOset
    private int m_VBOdataStride;                // grand pas global
    private int m_IndexBufferId;                // identifiant du VBO des indices, null si dessin direct
    private int m_IndexBufferSize;              // nombre d'indices à dessiner
    private int m_DrawingPrimitive;             // primitive à employer pour dessiner, ex: gl.TRIANGLES
    private int m_VAO;                          // VAO regroupant tous les VBO
    private boolean m_IsReady;                  // passe à true quand les VBO sont complets, prêts à être dessinés
    **/


    /**
     * constructeur de la classe VBOset
     * @param material : matériau avec lequel on va dessiner ce VBOset
     */
    constructor(material)
    {
        this.m_Material = material;
        this.m_VBOvariables = [];
        this.m_VBOdataStride = 0;
        this.m_IndexBufferId = null;
        this.m_IndexBufferSize = 0;
        this.m_DrawingPrimitive = gl.POINTS;
        this.m_VAO = null;
        this.m_IsReady = false;
    }


    /** destructeur */
    destroy()
    {
        // libérer les VBOvar qui ont été allouées
        for (let vbovar of this.m_VBOvariables) {
            vbovar.destroy();
        }

        // supprimer le VAO
        if (this.m_VAO != null) gl.deleteVertexArrays(this.m_VAO);
    }


    /**
     * Cette méthode définit le type des informations pour une variable attribut
     * @param idattr : voir la classe MeshVertex et getAttribute, par exemple MeshVertex.ID_ATTR_VERTEX, MeshVertex.ID_ATTR_COLOR
     * @param components : ex: 2 ou mieux Utils.VEC2
     * @param name : nom de la variable concernée ex: "glTexCoord"
     */
    addAttribute(idattr, components, name)
    {
        this.m_VBOvariables.push(new VBOvar(this.m_Material.getShaderId(), name, idattr, components));
    }


    /**
     * ajoute les composantes du vertex au tableau data, s'il est fourni (VBO entrelacé), sinon
     * les composantes sont mises dans les tableaux m_Data des VBOvars du VBOset (VBO multiples)
     * @param vertex : sommet dont il faut ajouter les informations au VBO
     * @param data : s'il est null ou absent, alors c'est dans vbovar.m_Data que sont mises les composantes
     */
    appendVertexComponents(vertex, data)
    {
        for (let vbovar of this.m_VBOvariables) {

            // nombre de composantes de cette variable (éventuellement 0 si pas utilisée)
            let components = vbovar.m_ComponentsNumber;
            if (components <= 0) continue;

            // valeur de cette variable sur le sommet courant
            let value = vertex.getAttribute(vbovar.m_IdAttr);
            if (value == null) continue;

            // ajouter les coordonnées s'il le faut
            for (let i=0; i<components; i++) {
                if (data == null) {
                    vbovar.m_Data.push(value[i]);
                } else {
                    data.push(value[i]);
                }
            }
        }
    }


    /**
     * Cette méthode crée le VBO contenant les attributs entrelacés
     * @param data : données à placer dans le VBO entrelacé
     */
    createInterleavedDataAttributesVBO(data)
    {
        // allouer le VBO
        let VBOid = Utils.makeFloatVBO(data, gl.ARRAY_BUFFER, gl.STATIC_DRAW);

        // calculer le grand pas global, mettre à jour les décalages
        this.m_VBOdataStride = 0;
        for (let vbovar of this.m_VBOvariables) {
            vbovar.setId(VBOid);
            vbovar.setIsOwned((this.m_VBOdataStride == 0));
            vbovar.setOffset(this.m_VBOdataStride);
            this.m_VBOdataStride += vbovar.getComponentsCount() * Utils.SIZEOF_FLOAT;
        }
    }


    /**
     * Cette méthode crée les VBO des attributs entrelacés
     * @param mesh : fournit la liste des sommets à placer dans les VBO
     */
    createInterleavedAttributesVBO(mesh)
    {
        // rassembler les coordonnées, couleurs, normales et coordonnées de texture
        let data = [];
        let iv = 0;
        for (let vertex of mesh.getVertexList()) {
            // renuméroter le sommet (numéro dans les VBOs)
            vertex.setNumber(iv);
            iv++;

            // ajouter les valeurs des variables attributs (VBOvar)
            this.appendVertexComponents(vertex, data);
        }

        // créer le VBO entrelacé
        this.createInterleavedDataAttributesVBO(data);
    }


    /**
     * Cette méthode crée les VBO des attributs multiples
     * @param mesh : fournit la liste des sommets à placer dans les VBO
     */
    createMultipleAttributesVBO(mesh)
    {
        // rassembler les coordonnées, couleurs et/ou normales demandées
        let iv = 0;
        for (let vertex of mesh.getVertexList()) {
            // renuméroter le sommet (numéro dans les VBOs)
            vertex.setNumber(iv);
            iv++;

            // ajouter les valeurs des variables attributs (VBOvar)
            this.appendVertexComponents(vertex, null);
        }

        // calculer le grand pas global, mettre à jour les décalages et fournir le VBO
        this.m_VBOdataStride = 0;
        for (let vbovar of this.m_VBOvariables) {
            vbovar.setId(Utils.makeFloatVBO(vbovar.getData(), gl.ARRAY_BUFFER, gl.STATIC_DRAW));
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
    createAttributesVBO(mesh, interleaved=true)
    {
        // création des VBOs
        if (interleaved) {
            this.createInterleavedAttributesVBO(mesh);
        } else {
            this.createMultipleAttributesVBO(mesh);
        }
        this.m_IsReady = true;
    }


    /**
     * retourne vrai si le maillage est dessinable (en cas de chargement asynchrone)
     * @return
     */
    isReady()
    {
        return this.m_IsReady;
    }


    /**
     * indique si le maillage est dessinable (en cas de chargement asynchrone)
     * @param ready
     */
    setReady(ready=true)
    {
        this.m_IsReady = ready;
    }


    /**
     * Cette méthode initialise le VBOset pour dessiner la primitive sans indices
     * @param primitive : par exemple gl.TRIANGLES
     * @param number : nombre de données à dessiner
     */
    createDirectPrimitiveVBO(primitive, number)
    {
        // initialisations
        this.m_DrawingPrimitive = primitive;
        this.m_IndexBufferId = null;
        this.m_IndexBufferSize = number;

        // création du VAO
        if (this.m_VAO != null) gl.deleteVertexArrays(this.m_VAO);
        this.m_VAO = gl.createVertexArray();

        // activation du VAO
        gl.bindVertexArray(this.m_VAO);

        // activer et lier les VBOs liés aux variables attribute
        for (let vbovar of this.m_VBOvariables) {
            vbovar.bindVBO(this.m_VBOdataStride);
        }

        // désactivation du VAO et des VBO
        gl.bindVertexArray(null);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
    }


    /**
     * Cette méthode initialise le VBOset pour dessiner la primitive avec les indices
     * @param primitive : par exemple gl.TRIANGLES
     * @param indexlist : tableau des indices
     * @return nombre d'indices présents dans le VBO
     */
    createIndexedPrimitiveVBO(primitive, indexlist)
    {
        this.m_DrawingPrimitive = primitive;
        this.m_IndexBufferSize = indexlist.length;
        if (this.m_IndexBufferSize > 65534) {
            this.m_IndexBufferId = Utils.makeIntVBO(indexlist, gl.ELEMENT_ARRAY_BUFFER, gl.STATIC_DRAW);
            this.m_IndexBufferType = gl.UNSIGNED_INT;
            console.log("VBO int");
        } else {
            this.m_IndexBufferId = Utils.makeShortVBO(indexlist, gl.ELEMENT_ARRAY_BUFFER, gl.STATIC_DRAW);
            this.m_IndexBufferType = gl.UNSIGNED_SHORT;
            console.log("VBO short");
        }

        // création du VAO
        if (this.m_VAO != null) gl.deleteVertexArrays(this.m_VAO);
        this.m_VAO = gl.createVertexArray();

        // activation du VAO
        gl.bindVertexArray(this.m_VAO);

        // activer et lier les VBOs liés aux variables attribute
        for (let vbovar of this.m_VBOvariables) {
            vbovar.bindVBO(this.m_VBOdataStride);
        }

        // liaison du VBO des indices
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.m_IndexBufferId);

        // désactivation du VAO et des VBO
        gl.bindVertexArray(null);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);

        return this.m_IndexBufferSize;
    }


    /**
     * Cette méthode active les VBOs et fait la liaison avec les attribute du shader
     */
    enable()
    {
        // avec un VAO, il suffit d'activer le VAO
        gl.bindVertexArray(this.m_VAO);
    }


    /**
     * Cette méthode désactive les VBOs
     */
    disable()
    {
        // avec un VAO, il suffit de désactiver le VAO
        gl.bindVertexArray(null);
    }


    /**
     * Cette méthode dessine le VBOset, avec les éléments demandés :
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        if (! this.m_IsReady) return;
        if (this.m_IndexBufferSize <= 0) return;

        // activer le matériau (shader <-> VBOs)
        this.m_Material.enable(mat4Projection, mat4ModelView);

        // activer et lier les VBO
        this.enable();

        // dessin indexé ?
        if (this.m_IndexBufferId != null) {

            // dessin des triangles
            gl.drawElements(this.m_DrawingPrimitive, this.m_IndexBufferSize, this.m_IndexBufferType, 0);

        } else {

            // dessin non indexé
            gl.drawArrays(this.m_DrawingPrimitive, 0, this.m_IndexBufferSize);
        }

        // libération du shader et des autres VBOs
        this.disable();
        this.m_Material.disable();
    }


    /**
     * met à jour les emplacements des variables du VBOset suite à une recompilation du shader
     */
    updateLocations()
    {
        for (let vbovar of this.m_VBOvariables) {
            vbovar.updateShader(this.m_Material.getShaderId());
        }
    }
}
