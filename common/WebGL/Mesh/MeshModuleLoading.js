// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModule.js");


/**
 * Ce module lit un fichier OBJ et en fait un maillage
 */
class MeshModuleLoading extends MeshModule
{
    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    constructor(mesh)
    {
        super(mesh);
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * Cette fonction reçoit un tableau de chaînes ["nv", "nt", "nn"]
     * Elle retourne la conversion l'une de ces chaînes en entier.
     * La chaîne est indiquée par l'indice.
     * Si la valeur lue est négatif, alors la fonction retourne max+indice
     * @param strings : tableau contenant ["nv", "nt", "nn"]
     * @param index : numéro de la chaîne à convertir
     * @param max : nombre de valeurs présentes dans les tableaux de coordonnées...
     * @return -1 si pb, nv ou nt ou nn selon l'index
     */
    splitNumbers(strings, index, max)
    {
        if (index >= strings.length) return -1;
        let word = strings[index];
        if (word == "") return -1;
        let value = parseInt(word);
        if (value > max) return -1;
        if (value < 0) return max + value;
        return value - 1;
    }


    /**
     * Cette méthode recherche dans vertexlist celui qui possède exactement les
     * mêmes coordonnées : 3D, de texture et normale qu'indiqué
     * @param nvntnn : mot contenant les numéros nn/nt/nn
     * @param vertexlist : tableau indexé par iv des sommets qui sont au même endroit
     * @param coordlist : tableau de vec3 donnant les coordonnées des sommets
     * @param texcoordlist : tableau de vec2 donnant les coordonnées de texture
     * @param normallist : tableau de vec3 donnant les normales
     * @return le sommet correspondant au triplet (nv,nt,nn)
     */
    findOrCreateVertex(nvntnn, vertexlist, coordlist, texcoordlist, normallist)
    {
        // indices des coordonnées 3D, des coordonnées de texture et de la normale
        let strings = nvntnn.split('/');
        let nv = this.splitNumbers(strings, 0, coordlist.length);
        let nt = this.splitNumbers(strings, 1, texcoordlist.length);
        let nn = this.splitNumbers(strings, 2, normallist.length);
        if (nv < 0) return null;

        // nom du sommet courant, c'est son identifiant
        let name = "v("+nv+","+nt+","+nn+")";

        // parcourir les sommets de la liste nv et voir s'il y a le même
        let siblings = [];
        if (vertexlist.has(nv)) {
            siblings = vertexlist.get(nv);
            for (let other of siblings) {
                // si les noms sont égaux, c'est le même sommet
                if (name == other.getName()) return other;
            }
        } else {
            // initialiser la liste des sommets ayant l'indice nv
            vertexlist.set(nv, siblings);
        }

        // il faut créer un nouveau sommet car soit nouveau, soit un peu différent des autres
        let vertex = this.m_Mesh.addVertex(name);
        vertex.setCoord(coordlist[nv]);
        if (nt >= 0) vertex.setTexCoord(texcoordlist[nt]);
        if (nn >= 0) vertex.setNormal(normallist[nn]);

        // on ajoute ce sommet dans la liste de ceux qui ont le même numéro nv
        siblings.push(vertex);
        return vertex;
    }


    /**
     * Charge les données du fichier OBJ indiqué et applique une homothétie aux sommets.
     * Appelle la fonction callback(maillage, param) une fois le chargement fini.
     * NB: le chargement est asynchrone !
     * @param objfilename : nom complet du fichier à charger
     * @param materialname : nom du matériau à lire, les autres sont ignorés, s'il vaut "" ou null, tous sont acceptés
     * @param scale : rapport d'agrandissement à appliquer
     * @param that : désigne l'objet en cours de chargement, au lieu de this
     * @param callback : fonction à appeler quand le chargement est terminé,
     * @param param : paramètre qui est passé à la fonction callback avant de créer les VBOs en plus du maillage
     */
    loadObjFile(objfilename, materialname, scale, that, callback, param)
    {
        // marquer le maillage comme pas encore prêt à être dessiné
        // il ne le sera que lorsque les données auront été récupérées et insérées, voir onLoadObj
        this.m_Mesh.m_IsReady = false;

        // essayer de lire le fichier en tant que div dans le document HTML
        let obj_content = document.getElementById(objfilename);
        if (obj_content != null) {
            this.onLoadObj(obj_content.innerHTML, scale, callback, param);
        } else {
            let mesh = this.m_Mesh;
            let module = this;
            // faire une requête HTTP pour demander le fichier obj
            let request = new XMLHttpRequest();
            request.overrideMimeType('text/plain; charset=x-user-defined');
            request.open("GET", objfilename, true);
            request.responseType = "text";
            request.onreadystatechange = function() {
                if (request.readyState == 4) {
                    if (request.status === 200) {
                        module.onLoadObj(request.responseText, materialname, scale, mesh, that, callback, param);
                    } else {
                        alert(objfilename+"\n"+request.statusText);
                    }
                }
            }
            request.onerror = function() {
                console.error(objfilename+" cannot be loaded, please check path and permissions of this file");
            }
            request.send();
        }
    }


    /**
     * Cette méthode est appelée automatiquement, quand le contenu du fichier obj
     * est devenu disponible.
     * @param obj_content : contenu du fichier obj
     * @param materialname : nom du matériau à lire, les autres sont ignorés
     * @param scale : rapport d'agrandissement à appliquer
     * @param mesh : maillage dans lequel il faut ajouter les triangles
     * @param that : objet sur lequel il faut appeler la callback, ex: la classe qui gère le maillage
     * @param callback : méthode de that à appeler après avoir chargé l'objet
     * @param param : paramètre à passer à la callback
     */
    onLoadObj(obj_content, materialname, scale, mesh, that, callback, param)
    {
        // remettre le maillage concerné (il est perdu lors du chargment asynchrone de plusieurs matériaux)
        this.m_Mesh = mesh;

        // table des sommets qu'on va extraire du fichier obj, ils sont groupés par indice nv
        let vertexlist = new Map();

        // tableau des coordonnées, des textures et des normales
        let coordlist    = [];
        let texcoordlist = [];
        let normallist   = [];
        let computeNormals = true;

        // matériau à prendre en compte (tous si materialname est null)
        let usemtl = null;

        // parcourir le fichier obj ligne par ligne
        let lines = obj_content.split('\n');
        for (let l=0; l<lines.length; l++) {
            let line = lines[l].replace(/^\s+|\s+$/g, '').replace(/\s+/g, ' ');
            // séparer la ligne en mots
            let words = line.split(' ');
            // mettre le premier mot en minuscules
            let word0 = words[0].toLowerCase();
            if (word0 == 'f' && words.length > 3) {
                // le matériau est-il celui qu'on veut ?
                if (materialname != null && materialname != "" && materialname != usemtl) continue;
                // lire les numéros du premier point
                let v1 = this.findOrCreateVertex(words[1], vertexlist, coordlist, texcoordlist, normallist);
                // lire les numéros du deuxième point
                let v2 = this.findOrCreateVertex(words[2], vertexlist, coordlist, texcoordlist, normallist);
                // lire et traiter les points suivants
                for (let i=3; i<words.length; i++) {
                    let v3 = this.findOrCreateVertex(words[i], vertexlist, coordlist, texcoordlist, normallist);
                    // ajouter un triangle v1,v2,v3
                    this.m_Mesh.addTriangle(v1,v2,v3);
                    // préparer le passage au triangle suivant
                    v2 = v3;
                }
            } else
            if (word0 == 'v') {
                // coordonnées 3D d'un sommet
                let x = parseFloat(words[1]);
                let y = parseFloat(words[2]);
                let z = parseFloat(words[3]);
                let coords = vec3.fromValues(x,y,z);
                vec3.scale(coords, coords, scale);
                coordlist.push(coords);
            } else
            if (word0 == 'vt') {
                // coordonnées de texture
                let u = parseFloat(words[1]);
                let v = parseFloat(words[2]);
                texcoordlist.push(vec2.fromValues(u,v));
            } else
            if (word0 == 'vn') {
                // coordonnées de la normale
                let nx = parseFloat(words[1]);
                let ny = parseFloat(words[2]);
                let nz = parseFloat(words[3]);
                normallist.push(vec3.fromValues(nx,ny,nz));
                computeNormals = false;
            } else
            if (word0 == 'usemtl') {
                usemtl = words[1];
            }
        }
        if (computeNormals) this.m_Mesh.computeNormals();

        // noter le maillage comme prêt à être utilisé
        this.m_Mesh.m_IsReady = true;

        // appeler la callback s'il y en a une (son rôle est généralement de créer un VBOset)
        if (callback != null) {
            callback(that, param);
        }
    }
}
