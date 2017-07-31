// Définition de la classe JointDebug
// elle permet d'afficher une jointure sous la forme d'un cylindre de triangles
// Le bord bleu indique la zone de poids nul, les pointes rouges donnent le poids de 1
// NB: il est préférable de bloquer les rotations des jointures

// superclasses et classes nécessaires
Requires("libs/Mesh/Joint.js");


class JointDebug extends Joint
{
    /**
     * Constructeur
     * @param nom de la jointure
     * @param parent jointure parente
     * @param os indique s'il faut dessiner l'os ou les poids
     */
    constructor(nom, parent=null, os=true)
    {
        // construction
        super(nom, parent);

        /// ajouter le shader et les VBOs permettant de dessiner la jointure

        // construire le vertex shader
        let srcVertexShader = dedent
            `#version 300 es
            in vec3 glVertex;
            in vec3 glColor;
            out vec3 frgColor;
            uniform mat4 matProjection;
            uniform mat4 matMV;
            void main()
            {
                gl_Position = matProjection * matMV * vec4(glVertex, 1.0);
                frgColor = glColor;
            }`;

        // construire le fragment shader
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            in vec3 frgColor;
            out vec4 glFragColor;
            void main()
            {
                glFragColor = vec4(frgColor, 1.0);
            }`;

        // compiler le shader de dessin
        this.m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "JointDebug");

        // déterminer où sont les variables attribute et uniform
        this.m_VertexLoc        = gl.getAttribLocation( this.m_ShaderId, "glVertex");
        this.m_ColorLoc         = gl.getAttribLocation( this.m_ShaderId, "glColor");
        this.m_MatProjectionLoc = gl.getUniformLocation(this.m_ShaderId, "matProjection");
        this.m_MatModelViewLoc  = gl.getUniformLocation(this.m_ShaderId, "matMV");

        // créer et remplir les buffers des coordonnées et des couleurs
        let vertices = [];
        let colors = [];

        this.m_IsBone = os;
        if (this.m_IsBone) {

            // Ce mode permet de voir les os du squelette, il affiche des sortes
            // de pointes de flèches colorées de la taille des os

            // dessiner 4 triangles formant une sorte de pointe de flèche
            vertices.push(0.0); vertices.push(0.0); vertices.push(0.0);
            colors.push(0.0); colors.push(0.0); colors.push(1.0);
            vertices.push(0.0); vertices.push(0.0); vertices.push(-1.0);
            colors.push(0.0); colors.push(1.0); colors.push(0.0);
            vertices.push(0.0); vertices.push(0.1); vertices.push(-0.1);
            colors.push(1.0); colors.push(0.0); colors.push(0.0);

            vertices.push(0.0); vertices.push(0.0); vertices.push(0.0);
            colors.push(0.0); colors.push(0.0); colors.push(1.0);
            vertices.push(0.0); vertices.push(0.0); vertices.push(-1.0);
            colors.push(0.0); colors.push(1.0); colors.push(0.0);
            vertices.push(0.0); vertices.push(-0.1);vertices.push(-0.1);
            colors.push(1.0); colors.push(0.0); colors.push(0.0);

            vertices.push(0.0); vertices.push(0.0); vertices.push(0.0);
            colors.push(0.0); colors.push(0.0); colors.push(1.0);
            vertices.push(0.0); vertices.push(0.0); vertices.push(-1.0);
            colors.push(0.0); colors.push(1.0); colors.push(0.0);
            vertices.push(0.1); vertices.push(0.0); vertices.push(-0.1);
            colors.push(1.0); colors.push(0.0); colors.push(0.0);

            vertices.push(0.0); vertices.push(0.0); vertices.push(0.0);
            colors.push(0.0); colors.push(0.0); colors.push(1.0);
            vertices.push(0.0); vertices.push(0.0); vertices.push(-1.0);
            colors.push(0.0); colors.push(1.0); colors.push(0.0);
            vertices.push(-0.1);vertices.push(0.0); vertices.push(-0.1);
            colors.push(1.0); colors.push(0.0); colors.push(0.0);

            this.m_TriangleCount = 4;

        } else {

            // Ce mode permet d'ajuster les zones m_Radius, m_Min et m_Max d'influence des jointures
            // Il affiche une sorte de cylindre dont la longueur dépend de Min et Max et le rayon
            // dépend de m_Radius. La couleur bleue indique des poids nuls, la couleur rouge un poids 1

            // dessiner un cylindre dirigé vers -Z
            let nbs = 12;
            for (let i=0; i<nbs; i++) {
                // angle, cosinus et sinus du point courant
                let a = Utils.radians(360 * (i+0.0) / nbs);
                let cosa = Math.cos(a);
                let sina = Math.sin(a);
                // angle, cosinus et sinus du point suivant
                let b = Utils.radians(360 * (i+0.5) / nbs);
                let cosb = Math.cos(b);
                let sinb = Math.sin(b);
                // angle, cosinus et sinus du point suivant
                let c = Utils.radians(360 * (i+1.0) / nbs);
                let cosc = Math.cos(c);
                let sinc = Math.sin(c);
                // triangles
                vertices.push(cosa); vertices.push(sina); vertices.push(0.0);
                colors.push(0.0);  colors.push(0.0);  colors.push(1.0);

                vertices.push(cosb); vertices.push(sinb); vertices.push(-1.0);
                colors.push(1.0);  colors.push(0.0);  colors.push(0.0);

                vertices.push(cosc); vertices.push(sinc); vertices.push(0.0);
                colors.push(0.0);  colors.push(0.0);  colors.push(1.0);
            }
            this.m_TriangleCount = nbs;
        }

        // créer les VBOs des coordonnées et des couleurs
        this.m_VertexBufferId = Utils.makeFloatVBO(vertices, gl.ARRAY_BUFFER, gl.STATIC_DRAW);
        this.m_ColorBufferId  = Utils.makeFloatVBO(colors, gl.ARRAY_BUFFER, gl.STATIC_DRAW);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        Utils.deleteVBO(this.m_VertexBufferId);
        Utils.deleteVBO(this.m_ColorBufferId);
        Utils.deleteShaderProgram(this.m_ShaderId);
        super.destroy();
    }


    /**
     * dessine une représentation de la jointure
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // afficher tous les triangles
        let cullFacePrec = gl.isEnabled(gl.CULL_FACE);
        gl.disable(gl.CULL_FACE);
        // les os doivent apparaître devant le maillage, mais pas les poids
        let depthTestPrec = gl.isEnabled(gl.DEPTH_TEST);
        if (this.m_IsBone) gl.disable(gl.DEPTH_TEST);

        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // décaler l'origine au pivot de la jointure
        let mat4ModelView = mat4.create();
        let target = vec3.create();
        vec3.add(target, this.m_Pivot, this.m_Direction);
        mat4.lookAt(mat4ModelView, this.m_Pivot, target, vec3.fromValues(0,1,0));
        mat4.invert(mat4ModelView, mat4ModelView);

        // modifier la taille de la jointure (impossible de faire ça dans le constructeur)
        if (this.m_IsBone) {
            mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(this.m_Length, this.m_Length, this.m_Length));
        } else {
            mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(this.m_Radius, this.m_Radius, this.m_Length*(this.m_Max-this.m_Min)));
            mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0, 0.0, this.m_Length*this.m_Min));
        }

        // transformations par la matrice globale et celle de vue
        mat4.multiply(mat4ModelView, this.m_MatGlobal, mat4ModelView);
        mat4.multiply(mat4ModelView, mat4View, mat4ModelView);

        // fournir les matrices P et MV
        mat4.glUniformMatrix(this.m_MatProjectionLoc,  mat4Projection);
        mat4.glUniformMatrix(this.m_MatModelViewLoc, mat4ModelView);

        // activer et lier le buffer contenant les coordonnées
        gl.enableVertexAttribArray(this.m_VertexLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_VertexBufferId);
        gl.vertexAttribPointer(this.m_VertexLoc, 3, gl.FLOAT, gl.FALSE, 0, 0);

        // activer et lier le buffer contenant les couleurs
        gl.enableVertexAttribArray(this.m_ColorLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_ColorBufferId);
        gl.vertexAttribPointer(this.m_ColorLoc, 3, gl.FLOAT, gl.FALSE, 0, 0);

        // dessiner le maillage
        gl.drawArrays(gl.TRIANGLES, 0, this.m_TriangleCount*3);

        // désactiver les buffers et le shader
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.disableVertexAttribArray(this.m_VertexLoc);
        gl.disableVertexAttribArray(this.m_ColorLoc);
        gl.useProgram(null);

        if (cullFacePrec) gl.enable(gl.CULL_FACE);
        if (depthTestPrec) gl.enable(gl.DEPTH_TEST);
    }
}
