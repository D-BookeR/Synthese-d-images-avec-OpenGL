// Définition de la classe DisplacementMaterial
// voir http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-closer-look-at-parallax-occlusion-mapping-r3262
// voir http://gamedev.stackexchange.com/questions/74354/per-pixel-displacement-mapping-glsl

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


/**
 * Cette fonction définit un matériau pour dessiner le maillage
 */
class DisplacementMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("DisplacementMaterial");

        // charger les textures
        this.m_DiffuseTexture = new Texture2D("data/textures/13302/diffuse.jpg", gl.LINEAR, gl.REPEAT);
        this.m_DiffuseTextureLoc = -1;
        this.m_NormalTexture = new Texture2D("data/textures/13302/normal.jpg", gl.LINEAR, gl.REPEAT);
        this.m_NormalTextureLoc = -1;
        this.m_ElevationTexture = new Texture2D("data/textures/13302/height.jpg", gl.LINEAR, gl.REPEAT);
        this.m_ElevationTextureLoc = -1;

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        return dedent
            `#version 300 es
            // matrices de changement de repère
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            // VBOs contenant le repère de Frenet complet
            in vec3 glVertex;
            in vec3 glNormal;
            in vec3 glTangent;
            in vec2 glTexCoord;

            // variables à interpoler pour le fragment shader
            out vec4 frgPosition;
            out vec3 frgNormal;
            out vec3 frgTangent;
            out vec2 frgTexCoord;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal = mat3Normal * glNormal;
                frgTangent = mat3Normal * glTangent;
                frgTexCoord = glTexCoord;
            }`;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        return dedent
            `#version 300 es
            precision mediump float;

            // variables interpolées venant du vertex shader
            in vec4 frgPosition;
            in vec3 frgNormal;
            in vec3 frgTangent;
            in vec2 frgTexCoord;

            // textures diffuse, normale et hauteur
            uniform sampler2D txDiffuse;
            uniform sampler2D txNormal;
            uniform sampler2D txHeight;

            // grandeur de l'effet de déplacement
            const float relative_height = 0.05;

            // nombre d'itérations de la recherche d'intersection
            const int steps_phase1 = 12;
            const int steps_phase2 = 8;

            /** calcul de l'intersection entre la vue et la surface
             * @param V : vec3 donnant l'axe de la vue au niveau de P
             * @param P : vec2 donnant les coordonnées de texture du point à la surface
             * @return vec2 donnant les coordonnées de texture de R correspondant au point effectivement vu
             */
            vec2 find_contact(vec3 V, vec2 P)
            {
                // incrément pour parcourir la texture
                vec3 increment = vec3(V.xy/V.z * (-relative_height), -1.0) / float(steps_phase1);

                // positions parcourues successivement, écrites dans le repère de Frenet
                vec3 position = vec3(P, 1.0);
                vec3 position_prec;
                vec3 position_high = position;
                vec3 position_low = position;

                // passe à 1.0 dès qu'il y a un contact avec la surface
                float ground_reached = 0.0;

                // recherche grossière du point de contact
                for (int i=0; i<steps_phase1; i++) {
                    position_prec = position;
                    position += increment;
                    // altitude du fond au niveau du point courant
                    float ground_level = texture(txHeight, position.xy).r;
                    // cette hauteur dépasse-t-elle la position courante ?
                    float first_contact = step(position.z + ground_reached*100.0, ground_level);
                    // mettre à jour les hauteurs haute et basse si c'est le premier contact
                    position_high = mix(position_high, position_prec, first_contact);
                    position_low  = mix(position_low, position,       first_contact);
                    ground_reached += first_contact;
                }

                // cas d'un contact non rencontré (ground_reached valait 0.0 au dernier tour)
                position_high = mix(position_prec, position_high, ground_reached);
                position_low  = mix(position,      position_low, ground_reached);
                //return (position_high.xy + position_low.xy)/2.0;
                //return position_high.xy;

                // recherche fine du point de contact par dichotomie
                for (int i=0; i<steps_phase2; i++) {
                    // point milieu entre position_high et position_low
                    position = (position_low + position_high) * 0.5;
                    // altitude du fond au niveau du point courant
                    float ground_level = texture(txHeight, position.xy).r;
                    // plus haut ou plus bas ?
                    float position_too_low = step(position.z, ground_level);
                    position_low  = mix(position_low, position,  position_too_low);
                    position_high = mix(position, position_high, position_too_low);
                }

                // dernier milieu
                position = (position_low + position_high) * 0.5;
                return position.xy;
            }

            out vec4 glFragColor;

            void main()
            {
                // matrices de passage entre les espaces tangent et global
                vec3 N = normalize(frgNormal);
                vec3 T = normalize(frgTangent);
                vec3 B = cross(N, T);
                mat3 TBN = mat3(T, B, N);
                mat3 TBNinv = transpose(TBN);

                // recherche du point de contact exact entre la vue et la surface
                vec3 Vue = normalize(TBNinv * frgPosition.xyz);
                vec2 newTexCoord = find_contact(Vue, frgTexCoord);

                // vecteur normal dans le repère de Frenet
                vec3 Nfrenet = texture(txNormal, newTexCoord).xyz * 2.0 - 1.0;

                // changement de repère du vecteur normal extrait de la normal map
                N = normalize(TBN * Nfrenet);

                // caractéristiques locales du matériau
                vec3 Kd = 0.2+texture(txDiffuse, newTexCoord).rgb;
                vec3 Ks = vec3(0.3, 0.3, 0.3);
                float Ns = 20.0;

                // direction et couleur de la lumière
                vec3 LightDirection = vec3(0.5, 0.7, 0.7);
                vec3 LightColor = vec3(1.5, 1.5, 1.5);

                // contribution diffuse de Lambert
                vec3 L = normalize(LightDirection);
                float dotNL = clamp(dot(N, L), 0.0, 1.0);

                // contribution spéculaire de Phong
                vec3 R = reflect(normalize(frgPosition.xyz), N);
                float dotRL = clamp(dot(R, L), 0.0, 1.0);

                // couleur finale
                glFragColor = vec4(LightColor*(Kd*dotNL + Ks*pow(dotRL, Ns)), 1.0);
            }`;
    }


    /**
     * compile ou recompile le shader
     */
    compileShader()
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        // déterminer où sont les variables uniform
        this.m_DiffuseTextureLoc   = gl.getUniformLocation(this.m_ShaderId, "txDiffuse");
        this.m_NormalTextureLoc   = gl.getUniformLocation(this.m_ShaderId, "txNormal");
        this.m_ElevationTextureLoc  = gl.getUniformLocation(this.m_ShaderId, "txHeight");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_TANGENT,  Utils.VEC3, "glTangent");
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
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

        // activer les textures chacune sur une unité différente
        this.m_DiffuseTexture.setTextureUnit(gl.TEXTURE0, this.m_DiffuseTextureLoc);
        this.m_NormalTexture.setTextureUnit(gl.TEXTURE1, this.m_NormalTextureLoc);
        this.m_ElevationTexture.setTextureUnit(gl.TEXTURE2, this.m_ElevationTextureLoc);
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_DiffuseTexture.setTextureUnit(gl.TEXTURE0);
        this.m_NormalTexture.setTextureUnit(gl.TEXTURE1);
        this.m_ElevationTexture.setTextureUnit(gl.TEXTURE2);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_ElevationTexture.destroy();
        this.m_NormalTexture.destroy();
        this.m_DiffuseTexture.destroy();
        super.destroy();
    }
}
