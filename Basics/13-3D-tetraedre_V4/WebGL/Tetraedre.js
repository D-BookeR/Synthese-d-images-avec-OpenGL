// Définition de la classe Tetraedre


class Tetraedre
{
    /** constructeur */
    constructor()
    {
        /** shader des triangles */

        let srcVertexShaderTriangles = dedent
            `#version 300 es
            uniform mat4 matrix;
            in vec3 glVertex;
            in vec3 glColor;
            out vec3 frgColor;
            void main()
            {
                gl_Position = matrix * vec4(glVertex, 1.0);
                frgColor = glColor;
            }`;

        let srcFragmentShaderTriangles = dedent
            `#version 300 es
            precision mediump float;
            in vec3 frgColor;
            out vec4 glFragColor;
            void main()
            {
                glFragColor = vec4(frgColor, 1.0);
            }`;


        // compiler le shader de dessin des triangles
        this.m_TrianglesShaderId = Utils.makeShaderProgram(srcVertexShaderTriangles, srcFragmentShaderTriangles, "Tetraedre Triangles");

        // déterminer où sont les variables attribute et uniform
        this.m_TrianglesMatrixLoc = gl.getUniformLocation(this.m_TrianglesShaderId, "matrix");
        this.m_Triangles_glVertexLoc = gl.getAttribLocation(this.m_TrianglesShaderId, "glVertex");
        this.m_Triangles_glColorLoc = gl.getAttribLocation(this.m_TrianglesShaderId, "glColor");


        /** shader des lignes */

        let srcVertexShaderLines = dedent
            `#version 300 es
            uniform mat4 matrix;
            in vec3 glVertex;
            void main()
            {
                gl_Position = matrix * vec4(glVertex, 1.0);
            }`;

        let srcFragmentShaderLines = dedent
            `#version 300 es
            precision mediump float;
            out vec4 glFragColor;
            void main()
            {
              glFragColor = vec4(1.0, 1.0, 1.0, 1.0);
            }`;


        // compiler le shader de dessin des lignes
        this.m_LinesShaderId = Utils.makeShaderProgram(srcVertexShaderLines, srcFragmentShaderLines, "shader Tetraedre Lignes");

        // déterminer où sont les variables attribute et uniform
        this.m_LinesMatrixLoc = gl.getUniformLocation(this.m_LinesShaderId, "matrix");
        this.m_LinesVertexLoc = gl.getAttribLocation(this.m_LinesShaderId, "glVertex");

        /** VBOs */

        // créer et remplir le buffer des coordonnées
        const b = 0.5;
        let vertices = [
            -b, +b, +b,     // P0 bleu
            +b, -b, +b,     // P1 orange
            +b, +b, -b,     // P2 vert
            -b, -b, -b,     // P3 violet
        ];
        this.m_VertexBufferId = Utils.makeFloatVBO(vertices, gl.ARRAY_BUFFER, gl.STATIC_DRAW);

        // créer et remplir le buffer des couleurs
        let colors = [
            0.0, 0.5, 1.0,  // P0 bleu
            1.0, 0.5, 0.0,  // P1 orange
            0.0, 1.0, 0.0,  // P2 vert
            0.7, 0.0, 0.7,  // P3 violet
        ];
        this.m_ColorBufferId = Utils.makeFloatVBO(colors, gl.ARRAY_BUFFER, gl.STATIC_DRAW);

        // créer et remplir le buffer des indices pour les triangles
        let triangles_indexlist = [
            0, 1, 2,
            0, 1, 3,
            1, 2, 3,
            0, 2, 3,
        ];
        this.TRIANGLE_COUNT = triangles_indexlist.length / 3;
        this.m_TrianglesIndexBufferId = Utils.makeShortVBO(triangles_indexlist, gl.ELEMENT_ARRAY_BUFFER, gl.STATIC_DRAW);

        // créer et remplir le buffer des indices pour les lignes
        let lines_indexlist = [
            0, 1,
            0, 2,
            0, 3,
            1, 2,
            1, 3,
            2, 3,
        ];
        this.LINE_COUNT = lines_indexlist.length / 2;
        this.m_LinesIndexBufferId = Utils.makeShortVBO(lines_indexlist, gl.ELEMENT_ARRAY_BUFFER, gl.STATIC_DRAW);
    }


    /**
     * dessiner l'objet
     * @param matrix : matrice à appliquer sur l'objet
     */
    onDraw(matrix)
    {
        // activer le shader des triangles
        gl.useProgram(this.m_TrianglesShaderId);

        // fournir la matrice au shader
        mat4.glUniformMatrix(this.m_TrianglesMatrixLoc, matrix);

        // activer et lier le buffer contenant les coordonnées
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_VertexBufferId);
        gl.enableVertexAttribArray(this.m_Triangles_glVertexLoc);
        gl.vertexAttribPointer(this.m_Triangles_glVertexLoc, Utils.VEC3, gl.FLOAT, gl.FALSE, 0, 0);

        // activer et lier le buffer contenant les couleurs
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_ColorBufferId);
        gl.enableVertexAttribArray(this.m_Triangles_glColorLoc);
        gl.vertexAttribPointer(this.m_Triangles_glColorLoc, Utils.VEC3, gl.FLOAT, gl.FALSE, 0, 0);

        // activer et lier le buffer contenant les indices des triangles
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.m_TrianglesIndexBufferId);

        // dessiner les triangles
        gl.drawElements(gl.TRIANGLES, this.TRIANGLE_COUNT * 3, gl.UNSIGNED_SHORT, 0);

        // désactiver les buffers qui ne sont plus utilisés
        gl.disableVertexAttribArray(this.m_Triangles_glColorLoc);


        // activer le shader des lignes
        gl.useProgram(this.m_LinesShaderId);

        // fournir la matrice au shader
        mat4.glUniformMatrix(this.m_LinesMatrixLoc, matrix);

        // réactiver et lier le buffer contenant les coordonnées
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_VertexBufferId);
        gl.enableVertexAttribArray(this.m_LinesVertexLoc);
        gl.vertexAttribPointer(this.m_LinesVertexLoc, Utils.VEC3, gl.FLOAT, gl.FALSE, 0, 0);

        // activer et lier le buffer contenant les indices des lignes
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.m_LinesIndexBufferId);

        // dessiner les lignes
        gl.lineWidth(3.0);
        gl.drawElements(gl.LINES, this.LINE_COUNT * 2, gl.UNSIGNED_SHORT, 0);

        // désactiver les buffers
        gl.disableVertexAttribArray(this.m_LinesVertexLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);

        // désactiver le shader
        gl.useProgram(null);
    }


    /** destructeur */
    destroy()
    {
        // supprimer les shaders et les VBOs
        Utils.deleteShaderProgram(this.m_TrianglesShaderId);
        Utils.deleteShaderProgram(this.m_LinesShaderId);
        Utils.deleteVBO(this.m_VertexBufferId);
        Utils.deleteVBO(this.m_ColorBufferId);
        Utils.deleteVBO(this.m_TrianglesIndexBufferId);
        Utils.deleteVBO(this.m_LinesIndexBufferId);
    }
}
