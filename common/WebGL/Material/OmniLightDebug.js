/**
 * Définition de la classe OmniLight, une base pour construire des éclairages diffus et localisés
 */

// superclasses et classes nécessaires
Requires("libs/Material/OmniLight.js");


class OmniLightDebug extends OmniLight
{
    /**
     * constructeur
     * C'est une lampe toute simple, positionnelle ou directionnelle, sans ombre
     */
    constructor()
    {
        super();
        this.m_Name = "OmniLightDebug";

        /// ajouter le shader et le VBO permettant de dessiner la lampe : une sorte d'étoile

        // construire le vertex shader
        let srcVertexShader = dedent
            `#version 300 es
            in vec3 glVertex;
            uniform mat4 matMV;
            uniform mat4 matProjection;
            void main()
            {
                gl_Position = matProjection * matMV * vec4(glVertex, 1.0);
            }`;

        // construire le fragment shader
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            out vec4 glFragColor;
            void main()
            {
                glFragColor = vec4(1.0, 1.0, 0.0, 1.0);
            }`;

        // compiler le shader de dessin
        this.m_DebugShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "OmniLightDebug");

        // déterminer où sont les variables attribute et uniform
        this.m_DebugVertexLoc        = gl.getAttribLocation( this.m_DebugShaderId, "glVertex");
        this.m_DebugMatProjectionLoc = gl.getUniformLocation(this.m_DebugShaderId, "matProjection");
        this.m_DebugMatModelViewLoc  = gl.getUniformLocation(this.m_DebugShaderId, "matMV");

        // créer et remplir le buffer des coordonnées
        let vertices = [];
        let s = 0.5;    // taille des branches

        // dessiner une sorte d'étoile sur tous les axes (lampe omnidirectionnelle)
        vertices.push(-s); vertices.push( 0.0); vertices.push( 0.0);
        vertices.push(+s); vertices.push( 0.0); vertices.push( 0.0);

        vertices.push( 0.0); vertices.push(-s); vertices.push( 0.0);
        vertices.push( 0.0); vertices.push(+s); vertices.push( 0.0);

        vertices.push( 0.0); vertices.push( 0.0); vertices.push(-s);
        vertices.push( 0.0); vertices.push( 0.0); vertices.push(+s);

        let k = 0.577 * s; // c'est à dire 1/racine carrée de 3, pour faire une longueur unitaire
        vertices.push(-k); vertices.push(-k); vertices.push(-k);
        vertices.push(+k); vertices.push(+k); vertices.push(+k);

        vertices.push(-k); vertices.push(+k); vertices.push(-k);
        vertices.push(+k); vertices.push(-k); vertices.push(+k);

        vertices.push(-k); vertices.push(-k); vertices.push(+k);
        vertices.push(+k); vertices.push(+k); vertices.push(-k);

        vertices.push(-k); vertices.push(+k); vertices.push(+k);
        vertices.push(+k); vertices.push(-k); vertices.push(-k);
        this.m_DebugLineCount = vertices.length / 3;

        // créer le VBO des coordonnées
        this.m_DebugVertexBufferId = Utils.makeFloatVBO(vertices, gl.ARRAY_BUFFER, gl.STATIC_DRAW);
    }


    /**
     * destructeur
     */
    destroy()
    {
        Utils.deleteVBO(this.m_DebugVertexBufferId);
        Utils.deleteShaderProgram(this.m_DebugShaderId);
        super.destroy();
    }


    /**
     * dessine une représentation de la lampe
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue (nb: pas utilisée)
     * @param scale : taille de l'étoile représentant la lampe
     */
    onDraw(mat4Projection, mat4View, scale=1.0)
    {
        // activer le shader
        gl.useProgram(this.m_DebugShaderId);

        // décaler l'origine à la position caméra de la lampe
        let mat4ModelView = mat4.create();
        mat4.translate(mat4ModelView, mat4ModelView, this.m_PositionCamera);
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(scale, scale, scale));

        // fournir les matrices MV et P
        mat4.glUniformMatrix(this.m_DebugMatModelViewLoc, mat4ModelView);
        mat4.glUniformMatrix(this.m_DebugMatProjectionLoc,  mat4Projection);

        // activer et lier le buffer contenant les coordonnées
        gl.enableVertexAttribArray(this.m_DebugVertexLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_DebugVertexBufferId);
        gl.vertexAttribPointer(this.m_DebugVertexLoc, 3, gl.FLOAT, gl.FALSE, 0, 0);

        // dessiner le maillage
        gl.drawArrays(gl.LINES, 0, this.m_DebugLineCount);

        // désactiver les buffers et le shader
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.disableVertexAttribArray(this.m_DebugVertexLoc);
        gl.useProgram(null);
    }


    /**
     * dessine la shadowmap de cette lampe sur l'écran (remplit tout l'écran)
     * NB: cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     */
    onDrawShadowMap()
    {
    }
}
