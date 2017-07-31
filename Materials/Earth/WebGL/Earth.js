// Définition de la classe Earth

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");


class Earth
{
    /**
     * Constructeur
     * @param nblon : nombre de méridiens
     * @param nblat : nombre de parallèles
     */
    constructor(nblon, nblat)
    {
        /// Textures
        this.m_TxDiffuseDay = new Texture2D("data/textures/earth/earth_map.jpg");
        this.m_TxDiffuseNight = new Texture2D("data/textures/earth/earth_lights.jpg");
        this.m_TxSpecular  = new Texture2D("data/textures/earth/earth_spec.jpg");

        /// Shader
        let Phong_vs = dedent
            `#version 300 es
            uniform mat4 matMV;
            uniform mat4 matProjection;
            uniform mat3 matNormal;
            in vec3 glVertex;
            in vec3 glNormal;
            in vec2 glTexCoord;
            out vec3 frgNormal;
            out vec4 frgPosition;
            out vec2 frgTexCoord;
            void main()
            {
                frgPosition = matMV * vec4(glVertex, 1.0);
                gl_Position = matProjection * frgPosition;
                frgNormal =  matNormal * glNormal;
                frgTexCoord = glTexCoord;
            }`;

        let Phong_fs = dedent
            `#version 300 es
            precision mediump float;
            uniform vec4 LightPosition;
            uniform sampler2D diffuseJour;
            uniform sampler2D diffuseNuit;
            uniform sampler2D speculaire;
            in vec3 frgNormal;
            in vec4 frgPosition;
            in vec2 frgTexCoord;
            out vec4 glFragColor;
            void main()
            {
                vec3 N =  normalize(frgNormal);
                vec3 L =  normalize(vec3(LightPosition-frgPosition));
                vec3 R = reflect(normalize(frgPosition.xyz), N);
                float NL = dot(N,L);
                float k = smoothstep(-0.2, 0.2, NL);
                NL = clamp(NL,0.0,1.0);
                vec4 Jour = texture(diffuseJour, frgTexCoord);
                vec4 Nuit = texture(diffuseNuit, frgTexCoord);
                vec3 Kd = mix(Nuit.rgb, Jour.rgb*NL, k);
                float RL = clamp(dot(R,L), 0.0, 1.0);
                vec3 Ks = texture(speculaire, frgTexCoord).rgb;
                vec3 color = Kd + Ks*pow(RL, 256.0);
                glFragColor = vec4(color,1);
            }`;


        // compiler le shader de dessin
        this.m_ShaderId = Utils.makeShaderProgram(Phong_vs, Phong_fs, "shader terre");

        // déterminer où sont les variables attribute
        this.m_VertexLoc   = gl.getAttribLocation(this.m_ShaderId, "glVertex");
        this.m_NormalLoc   = gl.getAttribLocation(this.m_ShaderId, "glNormal");
        this.m_TexCoordLoc = gl.getAttribLocation(this.m_ShaderId, "glTexCoord");

        // déterminer où sont les variables uniform
        this.m_MatMVLoc         = gl.getUniformLocation(this.m_ShaderId, "matMV");
        this.m_MatPLoc          = gl.getUniformLocation(this.m_ShaderId, "matProjection");
        this.m_MatNLoc          = gl.getUniformLocation(this.m_ShaderId, "matNormal");
        this.m_LightPositionLoc = gl.getUniformLocation(this.m_ShaderId, "LightPosition");
        this.m_TxDiffuseDayLoc = gl.getUniformLocation(this.m_ShaderId, "diffuseJour");
        this.m_TxDiffuseNightLoc = gl.getUniformLocation(this.m_ShaderId, "diffuseNuit");
        this.m_TxSpecularLoc  = gl.getUniformLocation(this.m_ShaderId, "speculaire");

        // créer et remplir le buffer des vertices et des normales
        let vertices = [];
        let normals = [];
        let texcoords = [];
        let dlat = 180.0/nblat;
        let dlon = 360.0/nblon;
        for (let ilat=0; ilat<=nblat; ilat++) {
            let coslat = Math.cos(Utils.radians(ilat*dlat-90));
            let sinlat = Math.sin(Utils.radians(ilat*dlat-90));
            for (let ilon=0; ilon<=nblon; ilon++) {
                let coslon = Math.cos(Utils.radians(ilon*dlon));
                let sinlon = Math.sin(Utils.radians(ilon*dlon));
                // ajouter les coordonnées du point (lon,lat)
                vertices.push(coslat*coslon);
                vertices.push(sinlat);
                vertices.push(coslat*sinlon);
                // ajouter la normale au point (lon,lat)
                normals.push(coslat*coslon);
                normals.push(sinlat);
                normals.push(coslat*sinlon);
                // ajouter les coordonnées de texture
                texcoords.push(1.0-(ilon+0.0)/nblon);
                texcoords.push((ilat+0.0)/nblat);
            }
        }
        this.m_VertexBufferId   = Utils.makeFloatVBO(vertices,  gl.ARRAY_BUFFER, gl.STATIC_DRAW);
        this.m_NormalBufferId   = Utils.makeFloatVBO(normals,  gl.ARRAY_BUFFER, gl.STATIC_DRAW);
        this.m_TexcoordBufferId = Utils.makeFloatVBO(texcoords, gl.ARRAY_BUFFER, gl.STATIC_DRAW);

        // créer et remplir le buffer des indices
        let indexlist = [];
        for (let ilat=0; ilat<nblat; ilat++) {
            for (let ilon=0; ilon<nblon; ilon++) {
                // ajouter deux triangles
                indexlist.push((ilat+0)*(nblon+1) + ilon+1);
                indexlist.push((ilat+0)*(nblon+1) + ilon+0);
                indexlist.push((ilat+1)*(nblon+1) + ilon+0);

                indexlist.push((ilat+0)*(nblon+1) + ilon+1);
                indexlist.push((ilat+1)*(nblon+1) + ilon+0);
                indexlist.push((ilat+1)*(nblon+1) + ilon+1);
            }
        }
        this.m_IndexBufferId = Utils.makeShortVBO(indexlist,gl.ELEMENT_ARRAY_BUFFER, gl.STATIC_DRAW);
        this.m_IndexBufferSize = indexlist.length;
    }


    /**
     * définit la position de la lampe
     * @param position : vec4 donnant les coordonnées de la lampe
     */
    setLightPosition(position)
    {
        gl.useProgram(this.m_ShaderId);
        gl.uniform4fv(this.m_LightPositionLoc, position);
    }


    /**
     * dessiner l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // fournir les textures
        this.m_TxDiffuseDay.setTextureUnit(gl.TEXTURE0, this.m_TxDiffuseDayLoc);
        this.m_TxDiffuseNight.setTextureUnit(gl.TEXTURE1, this.m_TxDiffuseNightLoc);
        this.m_TxSpecular.setTextureUnit(gl.TEXTURE2, this.m_TxSpecularLoc);

        // fournir les variables uniform au shader
        mat4.glUniformMatrix(this.m_MatMVLoc, mat4ModelView);
        mat4.glUniformMatrix(this.m_MatPLoc, mat4Projection);

        // calculer la matrice normale (coûteux)
        let normalMatrix = mat3.create();
        mat3.fromMat4(normalMatrix, mat4ModelView);
        mat3.invert(normalMatrix, normalMatrix);
        mat3.transpose(normalMatrix, normalMatrix);
        mat3.glUniformMatrix(this.m_MatNLoc, normalMatrix);

        // activer et lier le buffer contenant les coordonnées
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_VertexBufferId);
        gl.enableVertexAttribArray(this.m_VertexLoc);
        gl.vertexAttribPointer(this.m_VertexLoc, Utils.VEC3, gl.FLOAT, gl.FALSE, 0, 0);

        // activer et lier le buffer contenant les normales
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_NormalBufferId);
        gl.enableVertexAttribArray(this.m_NormalLoc);
        gl.vertexAttribPointer(this.m_NormalLoc, Utils.VEC3, gl.FLOAT, gl.FALSE, 0, 0);

        // activer et lier le buffer contenant les coords de texture
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_TexcoordBufferId);
        gl.enableVertexAttribArray(this.m_TexCoordLoc);
        gl.vertexAttribPointer(this.m_TexCoordLoc, Utils.VEC2, gl.FLOAT, gl.FALSE, 0, 0);

        // activer le buffer contenant les indices
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.m_IndexBufferId);

        // dessiner l'objet
        gl.drawElements(gl.TRIANGLES, this.m_IndexBufferSize, gl.UNSIGNED_SHORT, null);

        // désactiver les buffers
        gl.disableVertexAttribArray(this.m_VertexLoc);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);

        // désactiver le shader
        gl.useProgram(null);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        Utils.deleteVBO(this.m_IndexBufferId);
        Utils.deleteVBO(this.m_TexcoordBufferId);
        Utils.deleteVBO(this.m_NormalBufferId);
        Utils.deleteVBO(this.m_VertexBufferId);
        Utils.deleteShaderProgram(this.m_ShaderId);
        this.m_TxSpecular.destroy();
        this.m_TxDiffuseNight.destroy();
        this.m_TxDiffuseDay.destroy();
    }
}
