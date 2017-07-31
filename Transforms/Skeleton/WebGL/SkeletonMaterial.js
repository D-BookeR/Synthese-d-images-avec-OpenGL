// Définition de la classe SkeletonMaterial, une spécialisation de Material
// Ce matériau gère une hiérarchie de Joint représentant un squelette animable


// superclasses et classes nécessaires
Requires("libs/Material/Material.js");


class SkeletonMaterial extends Material
{
    /**
     * Constructeur de la classe SkeletonMaterial. Ce matériau gère une hiérarchie
     * de Joint représentant un squelette animable
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    constructor(Ks, Ns)
    {
        super("SkeletonMaterial");

        // caractéristiques du matériau
        this.m_Ks = Ks;
        this.m_Ns = Ns;

        // liste des jointures, celles qui n'ont pas de parent sont des racines
        this.m_Joints = [];

        // compiler le shader
        this.compileShader();
    }


    /**
     * crée une nouvelle jointure pour ce matériau
     * @param joint : jointure à ajouter
     * @return jointure à ajouter
     */
    addJoint(joint)
    {
        // l'ajouter à la liste
        this.m_Joints.push(joint);

        // recompiler le shader car le nombre de jointures a changé
        this.compileShader();

        return joint;
    }


    /**
     * calcule les poids des sommets du maillage
     * @param mesh : maillage concerné
     */
    computeWeights(mesh)
    {
        if (this.m_Joints.length == 0) throw "Il n'y a aucune joint";
        // traiter tous les sommets
        for (let vertex of mesh.getVertexList()) {
            // initialiser les poids à 0
            for (let joint of this.m_Joints) {
                joint.setWeight(0.0);
            }
            // calculer le poids du sommet compte tenu de toutes les jointures (les parents sont calculés avant les enfants)
            let coords = vertex.getCoord();
            for (let joint of this.m_Joints) {
                joint.computeWeights(coords);
            }
            // préparer les attributs poids et identifiants du sommet
            this.computeWeightsVertex(vertex);
        }
    }


    /**
     * affecte les attributs ID_ATTR_IDBONES et ID_ATTR_WEIGHTS d'un sommet
     * @param vertex : sommet dont il faut affecter les poids et numéros de jointures
     */
    computeWeightsVertex(vertex)
    {
        // tableau des numéros de jointures et des poids du sommet
        let JointsIndex = [];
        let JointsWeight = [];
        // passer les jointures en revue, garder celles dont les poids ne sont pas nuls
        let sum = 0.0;
        let num = 0;
        for (let joint of this.m_Joints) {
            let weight = joint.getWeight();
            if (weight > 0.0) {
                // ajouter le poids et le numéro à la fin
                JointsIndex.push(num);
                JointsWeight.push(weight);
                sum += weight;
            }
            num++;
        }
        // modifier les poids si leur somme n'est pas 1.0
        if (sum == 0.0) {
            // forcer la dépendance totale à la première jointure
            JointsIndex.push(0.0);
            JointsWeight.push(1.0);
        } else if (sum != 1.0) {
            for (let i=0; i<JointsWeight.length; i++) {
                JointsWeight[i] /= sum;
            }
        }
        // compléter les listes à 4 éléments
        for (let i=JointsIndex.length; i<4; i++) {
            JointsIndex.push(0.0);
            JointsWeight.push(0.0);
        }
        // enregistrer ces informations dans le sommet
        vertex.setAttribute(MeshVertex.ID_ATTR_IDBONES, vec4.fromValues(JointsIndex[0], JointsIndex[1], JointsIndex[2], JointsIndex[3]));
        vertex.setAttribute(MeshVertex.ID_ATTR_WEIGHTS, vec4.fromValues(JointsWeight[0], JointsWeight[1], JointsWeight[2], JointsWeight[3]));
    }



    /**
     * recopie le poids d'une des jointures dans l'attribut ID_ATTR_COLOR
     * @param mesh : maillage contenant les sommets à colorer
     * @param joint : jointure concernée
     */
    debugWeights(mesh, joint)
    {
        for (let vertex of mesh.getVertexList()) {
            // recalculer le poids de ce sommet pour la jointure
            let coords = vertex.getCoord();
            for (let j of this.m_Joints) {
                j.computeWeights(coords);
            }
            // transformer le poids en couleur
            let weight = joint.getWeight();
            let color = vec4.create();
            vec4.lerp(color, vec4.fromValues(0,0,1,1), vec4.fromValues(1,0,0,1), weight);
            vertex.setColor(color);
        }
    }



    /**
     * calcule les matrices de transformation des jointures
     */
    computeGlobalMatrices()
    {
        // calculer les matrices absolues des jointures
        for (let joint of this.m_Joints) {
            joint.computeGlobalMatrix();
        }
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        // début du vertex shader
        let srcVertexShader = dedent
            `#version 300 es

            // paramètres uniform
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            // attributs de sommets
            in vec3 glVertex;
            in vec3 glNormal;
            in vec4 glColor;
            in vec4 JointsIndex;    // numéro des jointures de ce sommet
            in vec4 JointsWeight;   // poids des jointures pour ce sommet

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec3 frgNormal;
            out vec4 frgColor;

            // définition des os du squelette
            const int N = ${Math.max(1, this.m_Joints.length)};
            uniform mat4 JointsGlobalMatrix[N];

            void main()
            {
                // coordonnées et normale initiale
                vec4 vertex_orig = vec4(glVertex, 1.0);
                vec4 normal_orig = vec4(glNormal, 0.0);

                // calculer la transformation par sa première jointure
                int num = int(JointsIndex.x);
                float weight = JointsWeight.x;
                mat4 matrix = JointsGlobalMatrix[num];
                vec4 vertex = matrix * vertex_orig * weight;
                vec4 normal = matrix * normal_orig * weight;

                // ajouter la transformation par sa deuxième jointure
                num = int(JointsIndex.y);
                weight = JointsWeight.y;
                matrix = JointsGlobalMatrix[num];
                vertex += matrix * vertex_orig * weight;
                normal += matrix * normal_orig * weight;

                // ajouter la transformation par sa troisième jointure
                num = int(JointsIndex.z);
                weight = JointsWeight.z;
                matrix = JointsGlobalMatrix[num];
                vertex += matrix * vertex_orig * weight;
                normal += matrix * normal_orig * weight;

                // ajouter la transformation par sa quatrième jointure
                num = int(JointsIndex.w);
                weight = JointsWeight.w;
                matrix = JointsGlobalMatrix[num];
                vertex += matrix * vertex_orig * weight;
                normal += matrix * normal_orig * weight;

                // normaliser les coordonnées (perte de précision)
                vertex /= vertex.w;

                // transformation en repère caméra et écran
                frgPosition = mat4ModelView * vertex;
                gl_Position = mat4Projection * frgPosition;
                frgNormal = mat3Normal * normal.xyz;

                // couleur
                frgColor = glColor;
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;

            // coordonnées et normale du fragment
            in vec4 frgPosition;
            in vec3 frgNormal;
            in vec4 frgColor;

            // g-buffer
            out vec4 glFragData[4];

            // caractéristiques du matériau
            const vec3 Ks = ${vec3.str(this.m_Ks)};
            const float Ns = ${this.m_Ns.toFixed(2)};

            void main()
            {
                glFragData[0] = frgColor;
                glFragData[1] = vec4(Ks, Ns);
                glFragData[2] = vec4(frgPosition.xyz, 1.0);
                glFragData[3] = vec4(normalize(frgNormal), 0.0);
            }`;
        return srcFragmentShader;
    }


    /**
     * recompile le shader du matériau
     */
    compileShader()
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        // déterminer où sont les variables uniform spécifiques
        // NB: !! les tableaux d'attributes sont très mal gérés en WebLG !!
        this.m_MatsGlobalJointsLoc = [];
        for (let i=0; i<10; i++) {
            this.m_MatsGlobalJointsLoc.push(gl.getUniformLocation(this.m_ShaderId, "JointsGlobalMatrix["+i+"]"));
        }
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,  Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_COLOR,   Utils.VEC4, "glColor");
        vboset.addAttribute(MeshVertex.ID_ATTR_IDBONES, Utils.VEC4, "JointsIndex");
        vboset.addAttribute(MeshVertex.ID_ATTR_WEIGHTS, Utils.VEC4, "JointsWeight");
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
        // appeler la méthode de la superclasse
        super.enable(mat4Projection, mat4ModelView);

        // fournir les matrices des os
        for (let i=0; i<this.m_Joints.length; i++) {
            mat4.glUniformMatrix(this.m_MatsGlobalJointsLoc[i], this.m_Joints[i].getGlobalMatrix());
        }
    }
}
