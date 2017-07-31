/**
 * Définition de la classe SoftSpotLight, un type de lampe plus sophistiqué
 * voir http://codeflow.org/entries/2013/feb/15/soft-shadow-mapping/
 */

// superclasses et classes nécessaires
Requires("libs/Material/SoftSpotLight.js");


class SoftSpotLightDebug extends SoftSpotLight
{
    /**
     * constructeur
     * @param maxangle : angle en degrés d'ouverture total de la lampe
     * @param minangle : angle dans lequel l'intensité est 100%, il y a un dégradé entre minangle et maxangle
     * @param radius : largeur de la source de lumière en unités à rapporter à far
     * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
     * @param near : distance la plus proche dans la shadowmap
     * @param far : distance la plus lointaine dans la shadowmap
     * @param pcss : algorithme employé pour le calcul du flou des ombres true:PCSS, false:PCF
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre gl.FRONT ou gl.BACK pour éliminer les faces avant ou de dos, gl.NONE pour ne rien configurer
     */
    constructor(maxangle, minangle, radius, shadowmapsize=null, near=null, far=null, pcss=true, offsetfill=false, cullface=gl.FRONT)
    {
        super(maxangle, minangle, radius, shadowmapsize, near, far, pcss, offsetfill, cullface);
        this.m_Name = "SoftSpotLightDebug";

        /// ajouter le shader et le VBO permettant de dessiner la lampe

        // construire le vertex shader
        let srcVertexShader = dedent
            `#version 300 es
            in vec3 glVertex;
            uniform mat4 matMV;
            uniform mat4 matProjection;
            void main()
            {
                gl_Position = matProjection * (matMV * vec4(glVertex, 1.0));
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
        this.m_DebugShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "SoftSpotLightDebug");

        // déterminer où sont les variables attribute et uniform
        this.m_DebugVertexLoc        = gl.getAttribLocation( this.m_DebugShaderId, "glVertex");
        this.m_DebugMatProjectionLoc = gl.getUniformLocation(this.m_DebugShaderId, "matProjection");
        this.m_DebugMatModelViewLoc  = gl.getUniformLocation(this.m_DebugShaderId, "matMV");

        // créer et remplir les buffers des coordonnées et des couleurs
        let vertices = [];
        // dessiner un cône dirigé vers -Z
        let rFar = Math.tan(this.m_MaxAngle * 0.5) * this.m_Far;
        let rNear = Math.tan(this.m_MinAngle * 0.5) * this.m_Near;
        let nbs = 12;
        for (let i=0; i<nbs; i++) {
            // angle, cosinus et sinus du point courant
            let a = Utils.radians(360.0 * (i+0.0) / nbs);
            let cosa = Math.cos(a);
            let sina = Math.sin(a);
            // angle, cosinus et sinus du point suivant
            let b = Utils.radians(360.0 * (i+1.0) / nbs);
            let cosb = Math.cos(b);
            let sinb = Math.sin(b);
            // cercle à la distance Near et correspondant à minangle
            vertices.push(rNear*cosa); vertices.push(rNear*sina); vertices.push(-this.m_Near);
            vertices.push(rNear*cosb); vertices.push(rNear*sinb); vertices.push(-this.m_Near);
            // rayons du spot vers la distance Far et correspondant à maxangle
            vertices.push(0.0);        vertices.push(0.0);        vertices.push(0.0);
            vertices.push(rFar*cosa);  vertices.push(rFar*sina);  vertices.push(-this.m_Far);
            // cercle à la distance Far et correspondant à maxangle
            vertices.push(rFar*cosa);  vertices.push(rFar*sina);  vertices.push(-this.m_Far);
            vertices.push(rFar*cosb);  vertices.push(rFar*sinb);  vertices.push(-this.m_Far);
        }
        this.m_DebugLineCount = vertices.length / 3;

        // créer les VBOs des coordonnées et des couleurs
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
     * @param mat4View : matrice de vue, ne sert pas car on prend directement les positions caméra
     */
    onDraw(mat4Projection, mat4View)
    {
        // activer le shader
        gl.useProgram(this.m_DebugShaderId);

        // décaler l'origine à la position caméra de la lampe
        let mat4ModelView = mat4.create();
        mat4.lookAt(mat4ModelView, this.m_PositionCamera, this.m_TargetCamera, vec3.fromValues(0,1,0));
        mat4.invert(mat4ModelView, mat4ModelView);

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
     */
    onDrawShadowMap()
    {
        // s'il n'y a pas de shadowmap, alors sortir sans rien faire
        if (this.m_ShadowMap == null) return;

        this.m_ShadowMap.onDraw();
    }
}
