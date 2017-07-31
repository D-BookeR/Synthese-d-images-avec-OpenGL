/**
 * Définition de la classe Material, une base pour construire des matériaux
 */

Requires("libs/Material/VBOset.js");

class Material
{
    /**
     * constructeur
     * NB: chaque sous-classe doit appeler elle-même compileShader() pour créer le shader
     * @param nom : nom du matériau
     */
    constructor(nom)
    {
        // nom du matériau
        this.m_Name = nom;

        // identifiant OpenGL du shader
        this.m_ShaderId = null;

        // emplacement des matrices de changement de repère
        this.m_MatPloc  = -1;
        this.m_MatMVloc = -1;
        this.m_MatNloc = -1;

        // matrice normale
        this.m_Mat3Normal = mat3.create();

        // plan de coupe par défaut à l'infini
        this.m_ClipPlaneOn = false;
        this.m_ClipPlane = vec4.fromValues(0,0,1,1e38);
        this.m_ClipPlaneLoc = -1;

        // note : ce constructeur ne compile pas le shader, c'est aux sous-classes de le faire
    }


    /** destructeur */
    destroy()
    {
        // supprimer le shader
        if (this.m_ShaderId != null) Utils.deleteShaderProgram(this.m_ShaderId);
    }


    /**
     * retourne le nom du matériau
     * @return nom du matériau
     */
    getName()
    {
        return this.m_Name;
    }


    /**
     * retourne l'identifiant du shader de ce matériau
     * @return identifiant du shader
     */
    getShaderId()
    {
        return this.m_ShaderId;
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es
            in vec3 glVertex;
            uniform mat4 mat4Projection;
            uniform mat4 mat4ModelView;
            out vec4 frgPosition;
            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = "";
        srcFragmentShader += "#version 300 es\n";
        srcFragmentShader += "precision mediump float;\n";
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "out vec4 glFragData[4];\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "    // plan de coupe\n";
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
            srcFragmentShader += "\n";
        }
        srcFragmentShader += "    glFragData[0] = vec4(1.0, 0.0, 1.0, 1.0);\n";
        srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(1.0, 1.0, 1.0, 1.0);\n";
        srcFragmentShader += "}";
        return srcFragmentShader;
    }


    /**
     * recompile le shader du matériau
     */
    compileShader()
    {
        // supprimer l'ancien shader s'il y en avait un
        if (this.m_ShaderId != null) Utils.deleteShaderProgram(this.m_ShaderId);

        // construire le vertex shader
        let srcVertexShader = this.getVertexShader();

        // construire le fragment shader
        let srcFragmentShader = this.getFragmentShader();

        // compiler et lier les shaders
        this.m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, this.m_Name);

        // déterminer où sont les variables uniform
        this.m_MatPloc      = gl.getUniformLocation(this.m_ShaderId, "mat4Projection");
        this.m_MatMVloc     = gl.getUniformLocation(this.m_ShaderId, "mat4ModelView");
        this.m_MatNloc      = gl.getUniformLocation(this.m_ShaderId, "mat3Normal");
        this.m_ClipPlaneLoc = gl.getUniformLocation(this.m_ShaderId, "ClipPlane");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = new VBOset(this);
        vboset.addAttribute(MeshVertex.ID_ATTR_VERTEX,   Utils.VEC3, "glVertex");
        return vboset;
    }


    /**
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelView : mat4 contenant la transformation vers la caméra
     */
    enable(mat4Projection, mat4ModelView)
    {
        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // fournir les matrices MV et P
        mat4.glUniformMatrix(this.m_MatPloc, mat4Projection);
        mat4.glUniformMatrix(this.m_MatMVloc, mat4ModelView);

        // calculer et fournir la matrice normale au shader
        if (this.m_MatNloc != null) {
            mat3.fromMat4(this.m_Mat3Normal, mat4ModelView);
            mat3.transpose(this.m_Mat3Normal, this.m_Mat3Normal);
            mat3.invert(this.m_Mat3Normal, this.m_Mat3Normal);
            mat3.glUniformMatrix(this.m_MatNloc, this.m_Mat3Normal);
        }

        // si le plan de coupe est actif, alors le fournir
        if (this.m_ClipPlaneOn && this.m_ClipPlaneLoc != null) {
            vec4.glUniform(this.m_ClipPlaneLoc, this.m_ClipPlane);
        }
    }


    /**
     * Cette méthode désactive le matériau
     * NB : le shader doit encore être activé
     */
    disable()
    {
        // désactiver le shader
        gl.useProgram(null);
    }


    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    setClipPlane(active=false, plane=null)
    {
        // il faut recompiler s'il y a un changement d'état
        let recompile = (active != this.m_ClipPlaneOn);

        if (active == false) {
            // désactiver le plan de coupe
            this.m_ClipPlane = vec4.fromValues(0,0,1,1e38);
        } else {
            // enregistrer le plan de coupe, l'activer s'il est présent
            if (plane == null) plane = vec4.fromValues(0,0,1,1e38);
            this.m_ClipPlane = plane;
        }
        this.m_ClipPlaneOn = active;

        // recompiler le shader
        if (recompile) this.compileShader();
    }


    /**
     * désactive le plan de coupe
     */
    resetClipPlane()
    {
        let recompile = this.m_ClipPlaneOn;
        this.m_ClipPlaneOn = false;
        this.m_ClipPlane = vec4.fromValues(0,0,1,1e38);
        if (recompile) this.compileShader();
    }


    /**
     * Cette méthode crée une collection de DeferredShadingMaterial lue d'un fichier MTL pour un fichier OBJ.
     * NB : le chargement du fichier est fait en asynchrone, par une requête HTTP sur le serveur
     * @param folder : contient le fichier mtl et les textures
     * @param mtlfilename : nom seul du fichier au format MTL (ne pas mettre de chemin)
     * @param callback : fonction(that, materiaux) à appeler après avoir chargé les matériaux
     * @param that : objet pour lequel on charge des matériaux, il est passé en premier paramètre de la callback
     */
    static loadMTL(folder, mtlfilename, callback, that)
    {
        if (callback == null) throw "Aucune callback définie pour Material.loadMTL";

        // faire une requête HTTP pour demander le fichier mtl
        let request = new XMLHttpRequest();
        request.overrideMimeType('text/plain; charset=x-user-defined');
        let fullpathname = folder + "/" + mtlfilename;
        request.open("GET", fullpathname, true);
        request.responseType = "text";
        request.onreadystatechange = function() {
            if (request.readyState == 4) {
                Material.onLoad(that, folder, request.responseText, callback);
            }
        }
        request.onerror = function() {
            console.error(fullpathname+" cannot be loaded, please check path and permissions of this file");
        }
        request.send();
    }


    /**
     * appelée quand le fichier MTL est reçu
     * @param that : objet qui traite la liste des matériaux
     * @param folder : contient le fichier mtl et les textures
     * @param source_mtl : contenu du fichier MTL
     * @param callback : fonction à appeler après avoir chargé les matériaux
     */
    static onLoad(that, folder, source_mtl, callback)
    {
        // dictionnaire des matériaux du fichier => dans le modèle
        let mat_descriptions = {};

        // matériau courant
        let mat_description = null;

        // parcourir le fichier ligne par ligne
        let lines = source_mtl.split('\n');
        let line_count = lines.length;
        for (let line_number=0; line_number<line_count; line_number++) {
            let line = lines[line_number].replace(/^\s+|\s+$/g, '').replace(/\s+/g, ' ');
            let words = line.split(' ');
            let word = words[0].toLowerCase();

            if (word == 'newmtl') {
                // créer un nouveau matériau et l'ajouter aux spécifs
                mat_description = {Kd:null, Ks:null, Ns:0.0, d:null};
                mat_descriptions[words[1]] = mat_description;
            } else
            if (mat_description != null) {
                if (word == 'map_kd') {
                    // construire le nom de la texture
                    mat_description.Kd = folder + "/" + words[1];
                } else
                if (word == 'map_ks') {
                    // construire le nom de la texture
                    mat_description.Ks = folder + "/" + words[1];
                } else
                if (word == 'kd') {
                    let r = parseFloat(words[1]);
                    let g = parseFloat(words[2]);
                    let b = parseFloat(words[3]);
                    mat_description.Kd = vec4.fromValues(r,g,b, 1.0);
                } else
                if (word == 'ks') {
                    let r = parseFloat(words[1]);
                    let g = parseFloat(words[2]);
                    let b = parseFloat(words[3]);
                    mat_description.Ks = vec3.fromValues(r,g,b);
                } else
                if (word == 'ns') {
                    mat_description.Ns = parseFloat(words[1]);
                } else
                if (word == 'd' && typeof mat_description.Kd === 'object') {
                    mat_description.Kd[3] = parseFloat(words[1]);
                }
            }
        }

        // repasser sur tous les matériaux et les créer effectivement
        let materials = {};
        for (let mat_name in mat_descriptions) {
            let mat_description = mat_descriptions[mat_name];
            materials[mat_name] = new DeferredShadingMaterial(mat_description.Kd, mat_description.Ks, mat_description.Ns);
        }

        // appeler la callback
        callback(that, materials);
    }
}
