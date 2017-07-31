// Définition de la classe Primitives

class Primitives
{
    /**
     * Constructeur
     */
    constructor()
    {
        /** shader */

        let srcVertexShader = dedent
            `#version 300 es
            in vec2 glVertex;
            in vec3 glColor;
            out vec3 frgColor;
            void main()
            {
                gl_Position = vec4(glVertex*0.4, 0.0, 1.0);
                frgColor = glColor;
                gl_PointSize = 10.0;
            }`;

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
        this.m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "Primitives");

        // déterminer où sont les variables attribute
        this.m_VertexLoc = gl.getAttribLocation(this.m_ShaderId, "glVertex");
        this.m_ColorLoc = gl.getAttribLocation(this.m_ShaderId, "glColor");

        /** VBOs */

        // créer et remplir le buffer des coordonnées
        let vertices = [
              0.5,  0.0,
              2.0, -0.5,
              1.5,  2.0,
             -0.5,  1.5,
             -2.0, -0.5,
             -0.5, -1.5,
              1.0, -2.0,
              1.5, -1.5
        ];
        this.m_VertexBufferId = Utils.makeFloatVBO(vertices, gl.ARRAY_BUFFER, gl.STATIC_DRAW);

        // créer et remplir le buffer des couleurs
        let colors = [
            0.72, 0.08, 0.08,
            0.72, 0.51, 0.08,
            0.51, 0.72, 0.08,
            0.08, 0.72, 0.08,
            0.08, 0.72, 0.51,
            0.08, 0.51, 0.72,
            0.08, 0.08, 0.72,
            0.51, 0.08, 0.72
        ];
        this.m_ColorBufferId = Utils.makeFloatVBO(colors, gl.ARRAY_BUFFER, gl.STATIC_DRAW);
    }

    onDraw()
    {
        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // activer et lier le buffer contenant les coordonnées
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_VertexBufferId);
        gl.enableVertexAttribArray(this.m_VertexLoc);
        gl.vertexAttribPointer(this.m_VertexLoc, Utils.VEC2, gl.FLOAT, gl.FALSE, 0, 0);

        // activer et lier le buffer contenant les couleurs
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_ColorBufferId);
        gl.enableVertexAttribArray(this.m_ColorLoc);
        gl.vertexAttribPointer(this.m_ColorLoc, Utils.VEC3, gl.FLOAT, gl.FALSE, 0, 0);

        // largeur des lignes (la largeur des points est dans le MeshVertex Shader)
        gl.lineWidth(5.0);

        // dessiner différentes primitives à tour de rôle
        switch (Math.floor(Utils.Time/2) % 7) {
        case 0:
            gl.drawArrays(gl.POINTS,            0, 8);
            break;
        case 1:
            gl.drawArrays(gl.LINES,             0, 8);
            break;
        case 2:
            gl.drawArrays(gl.LINE_STRIP,        0, 8);
            break;
        case 3:
            gl.drawArrays(gl.LINE_LOOP,         0, 8);
            break;
        case 4:
            gl.drawArrays(gl.TRIANGLES,         0, 6);
            break;
        case 5:
            gl.drawArrays(gl.TRIANGLE_STRIP,    0, 8);
            break;
        case 6:
            gl.drawArrays(gl.TRIANGLE_FAN,      0, 8);
            break;
        }

        // désactiver les buffers
        gl.disableVertexAttribArray(this.m_VertexLoc);
        gl.disableVertexAttribArray(this.m_ColorLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        // désactiver le shader
        gl.useProgram(null);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        Utils.deleteVBO(this.m_ColorBufferId);
        Utils.deleteVBO(this.m_VertexBufferId);
        Utils.deleteShaderProgram(this.m_ShaderId);
    }
}
