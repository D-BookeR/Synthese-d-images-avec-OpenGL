#ifndef GL_MATRIX
#define GL_MATRIX

/* Copyright (c) 2013, Brandon Jones, Colin MacKenzie IV. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */


#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <math.h>

class vec2;
class vec3;
class vec4;
class mat2;
class mat2d;
class mat3;
class mat4;
class quat;

static const GLfloat GLMAT_EPSILON = 0.000001;
/**
 * @class 2x2 Matrix
 * @name mat2
 */
class mat2 {

private:

    GLfloat m_Cells[4];

    friend class vec2;
    friend class vec3;
    friend class vec4;
    friend class mat2d;
    friend class mat3;
    friend class mat4;
    friend class quat;

public:

    static const mat2 null;

    mat2()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
        m_Cells[2] = NAN;
        m_Cells[3] = NAN;
    }

    /**
     * Access to individual components of mat2
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const mat2& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1] &&
            a.m_Cells[2] == m_Cells[2] &&
            a.m_Cells[3] == m_Cells[3];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const mat2& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1] ||
            a.m_Cells[2] != m_Cells[2] ||
            a.m_Cells[3] != m_Cells[3];
    }
    /**
     * Creates a new identity mat2
     *
     * @returns {mat2} a new 2x2 matrix
     */
    static mat2 create();
    /**
     * Creates a new mat2 initialized with values from an existing matrix
     *
     * @param a matrix to clone
     * @returns {mat2} a new 2x2 matrix
     */
    static mat2 clone(const mat2 a);
    /**
     * Copy the values from one mat2 to another
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat2} out
     */
    static mat2 copy(mat2& out, const mat2 a);
    /**
     * Set a mat2 to the identity matrix
     *
     * @param out the receiving matrix
     * @returns {mat2} out
     */
    static mat2 identity(mat2& out);
    /**
     * Create a new mat2 with the given values
     *
     * @param m00 Component in column 0, row 0 position (index 0)
     * @param m01 Component in column 0, row 1 position (index 1)
     * @param m10 Component in column 1, row 0 position (index 2)
     * @param m11 Component in column 1, row 1 position (index 3)
     * @returns {mat2} out A new 2x2 matrix
     */
    static mat2 fromValues(const GLfloat m00, const GLfloat m01, const GLfloat m10, const GLfloat m11);
    /**
     * Set the components of a mat2 to the given values
     *
     * @param out the receiving matrix
     * @param m00 Component in column 0, row 0 position (index 0)
     * @param m01 Component in column 0, row 1 position (index 1)
     * @param m10 Component in column 1, row 0 position (index 2)
     * @param m11 Component in column 1, row 1 position (index 3)
     * @returns {mat2} out
     */
    static mat2 set(mat2& out, const GLfloat m00, const GLfloat m01, const GLfloat m10, const GLfloat m11);
    /**
     * Transpose the values of a mat2
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat2} out
     */
    static mat2 transpose(mat2& out, const mat2 a);
    /**
     * Inverts a mat2
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat2} out
     */
    static mat2 invert(mat2& out, const mat2 a);
    /**
     * Calculates the adjugate of a mat2
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat2} out
     */
    static mat2 adjoint(mat2& out, const mat2 a);
    /**
     * Calculates the determinant of a mat2
     *
     * @param a the source matrix
     * @returns {Number} determinant of a
     */
    static GLfloat determinant(const mat2 a);
    /**
     * Multiplies two mat2's
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat2} out
     */
    static mat2 multiply(mat2& out, const mat2 a, const mat2 b);
    /**
     * Rotates a mat2 by the given angle
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param rad the angle to rotate the matrix by
     * @returns {mat2} out
     */
    static mat2 rotate(mat2& out, const mat2 a, const GLfloat rad);
    /**
     * Scales the mat2 by the dimensions in the given vec2
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param v the vec2 to scale the matrix by
     * @returns {mat2} out
     **/
    static mat2 scale(mat2& out, const mat2 a, const vec2 v);
    /**
     * Creates a matrix from a given angle
     * This is equivalent to (but much faster than):
     *
     *     mat2.identity(dest);
     *     mat2.rotate(dest, dest, rad);
     *
     * @param out mat2 receiving operation result
     * @param rad the angle to rotate the matrix by
     * @returns {mat2} out
     */
    static mat2 fromRotation(mat2& out, const GLfloat rad);
    /**
     * Creates a matrix from a vector scaling
     * This is equivalent to (but much faster than):
     *
     *     mat2.identity(dest);
     *     mat2.scale(dest, dest, vec);
     *
     * @param out mat2 receiving operation result
     * @param v Scaling vector
     * @returns {mat2} out
     */
    static mat2 fromScaling(mat2& out, const vec2 v);
    /**
     * Returns Frobenius norm of a mat2
     *
     * @param a the matrix to calculate Frobenius norm of
     * @returns {Number} Frobenius norm
     */
    static GLfloat frob(const mat2 a);
    /**
     * Adds two mat2's
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat2} out
     */
    static mat2 add(mat2& out, const mat2 a, const mat2 b);
    /**
     * Subtracts matrix b from matrix a
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat2} out
     */
    static mat2 subtract(mat2& out, const mat2 a, const mat2 b);
    /**
     * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool exactEquals(const mat2 a, const mat2 b);
    /**
     * Returns whether or not the matrices have approximately the same elements in the same position.
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool equals(const mat2 a, const mat2 b);
    /**
     * Multiply each element of the matrix by a scalar.
     *
     * @param out the receiving matrix
     * @param a the matrix to scale
     * @param b amount to scale the matrix's elements by
     * @returns {mat2} out
     */
    static mat2 multiplyScalar(mat2& out, const mat2 a, const GLfloat b);
    /**
     * Adds two mat2's after multiplying each element of the second operand by a scalar value.
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param scale the amount to scale b's elements by before adding
     * @returns {mat2} out
     */
    static mat2 multiplyScalarAndAdd(mat2& out, const mat2 a, const mat2 b, const GLfloat scale);
    /**
     * Returns a string representation of a mat2
     *
     * @param a to represent as a string
     * @returns {String} string representation of the mat2
     */
    static std::string str(const mat2& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {mat2} out
     */
    static mat2 zero(mat2& out);
    /**
     * calls glUniformMatrix2fv for the matrix a
     *
     * @param loc the location of the vector
     * @param a the vector to send to OpenGL shader
     * @returns {void}
     */
    static void glUniformMatrix(const GLint loc, const mat2 a);
};
/**
 * @class 2x3 Matrix
 * @name mat2d
 *
 * @description
 * A mat2d contains six elements defined as:
 * <pre>
 * [a, c, tx,
 *  b, d, ty]
 * </pre>
 * This is a short form for the 3x3 matrix:
 * <pre>
 * [a, c, tx,
 *  b, d, ty,
 *  0, 0, 1]
 * </pre>
 * The last row is ignored so the array is shorter and operations are faster.
 */
class mat2d {

private:

    GLfloat m_Cells[6];

    friend class vec2;
    friend class vec3;
    friend class vec4;
    friend class mat2;
    friend class mat3;
    friend class mat4;
    friend class quat;

public:

    static const mat2d null;

    mat2d()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
        m_Cells[2] = NAN;
        m_Cells[3] = NAN;
        m_Cells[4] = NAN;
        m_Cells[5] = NAN;
    }

    /**
     * Access to individual components of mat2d
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const mat2d& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1] &&
            a.m_Cells[2] == m_Cells[2] &&
            a.m_Cells[3] == m_Cells[3] &&
            a.m_Cells[4] == m_Cells[4] &&
            a.m_Cells[5] == m_Cells[5];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const mat2d& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1] ||
            a.m_Cells[2] != m_Cells[2] ||
            a.m_Cells[3] != m_Cells[3] ||
            a.m_Cells[4] != m_Cells[4] ||
            a.m_Cells[5] != m_Cells[5];
    }
    /**
     * Creates a new identity mat2d
     *
     * @returns {mat2d} a new 2x3 matrix
     */
    static mat2d create();
    /**
     * Creates a new mat2d initialized with values from an existing matrix
     *
     * @param a matrix to clone
     * @returns {mat2d} a new 2x3 matrix
     */
    static mat2d clone(const mat2d a);
    /**
     * Copy the values from one mat2d to another
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat2d} out
     */
    static mat2d copy(mat2d& out, const mat2d a);
    /**
     * Set a mat2d to the identity matrix
     *
     * @param out the receiving matrix
     * @returns {mat2d} out
     */
    static mat2d identity(mat2d& out);
    /**
     * Create a new mat2d with the given values
     *
     * @param a Component A (index 0)
     * @param b Component B (index 1)
     * @param c Component C (index 2)
     * @param d Component D (index 3)
     * @param tx Component TX (index 4)
     * @param ty Component TY (index 5)
     * @returns {mat2d} A new mat2d
     */
    static mat2d fromValues(const GLfloat a, const GLfloat b, const GLfloat c, const GLfloat d, const GLfloat tx, const GLfloat ty);
    /**
     * Set the components of a mat2d to the given values
     *
     * @param out the receiving matrix
     * @param a Component A (index 0)
     * @param b Component B (index 1)
     * @param c Component C (index 2)
     * @param d Component D (index 3)
     * @param tx Component TX (index 4)
     * @param ty Component TY (index 5)
     * @returns {mat2d} out
     */
    static mat2d set(mat2d& out, const GLfloat a, const GLfloat b, const GLfloat c, const GLfloat d, const GLfloat tx, const GLfloat ty);
    /**
     * Inverts a mat2d
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat2d} out
     */
    static mat2d invert(mat2d& out, const mat2d a);
    /**
     * Calculates the determinant of a mat2d
     *
     * @param a the source matrix
     * @returns {Number} determinant of a
     */
    static GLfloat determinant(const mat2d a);
    /**
     * Multiplies two mat2d's
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat2d} out
     */
    static mat2d multiply(mat2d& out, const mat2d a, const mat2d b);
    /**
     * Rotates a mat2d by the given angle
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param rad the angle to rotate the matrix by
     * @returns {mat2d} out
     */
    static mat2d rotate(mat2d& out, const mat2d a, const GLfloat rad);
    /**
     * Scales the mat2d by the dimensions in the given vec2
     *
     * @param out the receiving matrix
     * @param a the matrix to translate
     * @param v the vec2 to scale the matrix by
     * @returns {mat2d} out
     **/
    static mat2d scale(mat2d& out, const mat2d a, const vec2 v);
    /**
     * Translates the mat2d by the dimensions in the given vec2
     *
     * @param out the receiving matrix
     * @param a the matrix to translate
     * @param v the vec2 to translate the matrix by
     * @returns {mat2d} out
     **/
    static mat2d translate(mat2d& out, const mat2d a, const vec2 v);
    /**
     * Creates a matrix from a given angle
     * This is equivalent to (but much faster than):
     *
     *     mat2d.identity(dest);
     *     mat2d.rotate(dest, dest, rad);
     *
     * @param out mat2d receiving operation result
     * @param rad the angle to rotate the matrix by
     * @returns {mat2d} out
     */
    static mat2d fromRotation(mat2d& out, const GLfloat rad);
    /**
     * Creates a matrix from a vector scaling
     * This is equivalent to (but much faster than):
     *
     *     mat2d.identity(dest);
     *     mat2d.scale(dest, dest, vec);
     *
     * @param out mat2d receiving operation result
     * @param v Scaling vector
     * @returns {mat2d} out
     */
    static mat2d fromScaling(mat2d& out, const vec2 v);
    /**
     * Creates a matrix from a vector translation
     * This is equivalent to (but much faster than):
     *
     *     mat2d.identity(dest);
     *     mat2d.translate(dest, dest, vec);
     *
     * @param out mat2d receiving operation result
     * @param v Translation vector
     * @returns {mat2d} out
     */
    static mat2d fromTranslation(mat2d& out, const vec2 v);
    /**
     * Returns Frobenius norm of a mat2d
     *
     * @param a the matrix to calculate Frobenius norm of
     * @returns {Number} Frobenius norm
     */
    static GLfloat frob(const mat2d a);
    /**
     * Adds two mat2d's
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat2d} out
     */
    static mat2d add(mat2d& out, const mat2d a, const mat2d b);
    /**
     * Subtracts matrix b from matrix a
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat2d} out
     */
    static mat2d subtract(mat2d& out, const mat2d a, const mat2d b);
    /**
     * Multiply each element of the matrix by a scalar.
     *
     * @param out the receiving matrix
     * @param a the matrix to scale
     * @param b amount to scale the matrix's elements by
     * @returns {mat2d} out
     */
    static mat2d multiplyScalar(mat2d& out, const mat2d a, const GLfloat b);
    /**
     * Adds two mat2d's after multiplying each element of the second operand by a scalar value.
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param scale the amount to scale b's elements by before adding
     * @returns {mat2d} out
     */
    static mat2d multiplyScalarAndAdd(mat2d& out, const mat2d a, const mat2d b, const GLfloat scale);
    /**
     * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool exactEquals(const mat2d a, const mat2d b);
    /**
     * Returns whether or not the matrices have approximately the same elements in the same position.
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool equals(const mat2d a, const mat2d b);
    /**
     * Returns a string representation of a mat2d
     *
     * @param a to represent as a string
     * @returns {String} string representation of the mat2d
     */
    static std::string str(const mat2d& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {mat2d} out
     */
    static mat2d zero(mat2d& out);
};
/**
 * @class 3x3 Matrix
 * @name mat3
 */
class mat3 {

private:

    GLfloat m_Cells[9];

    friend class vec2;
    friend class vec3;
    friend class vec4;
    friend class mat2;
    friend class mat2d;
    friend class mat4;
    friend class quat;

public:

    static const mat3 null;

    mat3()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
        m_Cells[2] = NAN;
        m_Cells[3] = NAN;
        m_Cells[4] = NAN;
        m_Cells[5] = NAN;
        m_Cells[6] = NAN;
        m_Cells[7] = NAN;
        m_Cells[8] = NAN;
    }

    /**
     * Access to individual components of mat3
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const mat3& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1] &&
            a.m_Cells[2] == m_Cells[2] &&
            a.m_Cells[3] == m_Cells[3] &&
            a.m_Cells[4] == m_Cells[4] &&
            a.m_Cells[5] == m_Cells[5] &&
            a.m_Cells[6] == m_Cells[6] &&
            a.m_Cells[7] == m_Cells[7] &&
            a.m_Cells[8] == m_Cells[8];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const mat3& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1] ||
            a.m_Cells[2] != m_Cells[2] ||
            a.m_Cells[3] != m_Cells[3] ||
            a.m_Cells[4] != m_Cells[4] ||
            a.m_Cells[5] != m_Cells[5] ||
            a.m_Cells[6] != m_Cells[6] ||
            a.m_Cells[7] != m_Cells[7] ||
            a.m_Cells[8] != m_Cells[8];
    }
    /**
     * Creates a new identity mat3
     *
     * @returns {mat3} a new 3x3 matrix
     */
    static mat3 create();
    /**
     * Copies the upper-left 3x3 values into the given mat3.
     *
     * @param out the receiving 3x3 matrix
     * @param a   the source 4x4 matrix
     * @returns {mat3} out
     */
    static mat3 fromMat4(mat3& out, const mat4 a);
    /**
     * Creates a new mat3 initialized with values from an existing matrix
     *
     * @param a matrix to clone
     * @returns {mat3} a new 3x3 matrix
     */
    static mat3 clone(const mat3 a);
    /**
     * Copy the values from one mat3 to another
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat3} out
     */
    static mat3 copy(mat3& out, const mat3 a);
    /**
     * Create a new mat3 with the given values
     *
     * @param m00 Component in column 0, row 0 position (index 0)
     * @param m01 Component in column 0, row 1 position (index 1)
     * @param m02 Component in column 0, row 2 position (index 2)
     * @param m10 Component in column 1, row 0 position (index 3)
     * @param m11 Component in column 1, row 1 position (index 4)
     * @param m12 Component in column 1, row 2 position (index 5)
     * @param m20 Component in column 2, row 0 position (index 6)
     * @param m21 Component in column 2, row 1 position (index 7)
     * @param m22 Component in column 2, row 2 position (index 8)
     * @returns {mat3} A new mat3
     */
    static mat3 fromValues(const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m20, const GLfloat m21, const GLfloat m22);
    /**
     * Set the components of a mat3 to the given values
     *
     * @param out the receiving matrix
     * @param m00 Component in column 0, row 0 position (index 0)
     * @param m01 Component in column 0, row 1 position (index 1)
     * @param m02 Component in column 0, row 2 position (index 2)
     * @param m10 Component in column 1, row 0 position (index 3)
     * @param m11 Component in column 1, row 1 position (index 4)
     * @param m12 Component in column 1, row 2 position (index 5)
     * @param m20 Component in column 2, row 0 position (index 6)
     * @param m21 Component in column 2, row 1 position (index 7)
     * @param m22 Component in column 2, row 2 position (index 8)
     * @returns {mat3} out
     */
    static mat3 set(mat3& out, const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m20, const GLfloat m21, const GLfloat m22);
    /**
     * Set a mat3 to the identity matrix
     *
     * @param out the receiving matrix
     * @returns {mat3} out
     */
    static mat3 identity(mat3& out);
    /**
     * Transpose the values of a mat3
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat3} out
     */
    static mat3 transpose(mat3& out, const mat3 a);
    /**
     * Inverts a mat3
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat3} out
     */
    static mat3 invert(mat3& out, const mat3 a);
    /**
     * Calculates the adjugate of a mat3
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat3} out
     */
    static mat3 adjoint(mat3& out, const mat3 a);
    /**
     * Calculates the determinant of a mat3
     *
     * @param a the source matrix
     * @returns {Number} determinant of a
     */
    static GLfloat determinant(const mat3 a);
    /**
     * Multiplies two mat3's
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat3} out
     */
    static mat3 multiply(mat3& out, const mat3 a, const mat3 b);
    /**
     * Translate a mat3 by the given vector
     *
     * @param out the receiving matrix
     * @param a the matrix to translate
     * @param v vector to translate by
     * @returns {mat3} out
     */
    static mat3 translate(mat3& out, const mat3 a, const vec2 v);
    /**
     * Rotates a mat3 by the given angle
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param rad the angle to rotate the matrix by
     * @returns {mat3} out
     */
    static mat3 rotate(mat3& out, const mat3 a, const GLfloat rad);
    /**
     * Scales the mat3 by the dimensions in the given vec2
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param v the vec2 to scale the matrix by
     * @returns {mat3} out
     **/
    static mat3 scale(mat3& out, const mat3 a, const vec2 v);
    /**
     * Creates a matrix from a vector translation
     * This is equivalent to (but much faster than):
     *
     *     mat3.identity(dest);
     *     mat3.translate(dest, dest, vec);
     *
     * @param out mat3 receiving operation result
     * @param v Translation vector
     * @returns {mat3} out
     */
    static mat3 fromTranslation(mat3& out, const vec2 v);
    /**
     * Creates a matrix from a given angle
     * This is equivalent to (but much faster than):
     *
     *     mat3.identity(dest);
     *     mat3.rotate(dest, dest, rad);
     *
     * @param out mat3 receiving operation result
     * @param rad the angle to rotate the matrix by
     * @returns {mat3} out
     */
    static mat3 fromRotation(mat3& out, const GLfloat rad);
    /**
     * Creates a matrix from a vector scaling
     * This is equivalent to (but much faster than):
     *
     *     mat3.identity(dest);
     *     mat3.scale(dest, dest, vec);
     *
     * @param out mat3 receiving operation result
     * @param v Scaling vector
     * @returns {mat3} out
     */
    static mat3 fromScaling(mat3& out, const vec2 v);
    /**
     * Copies the values from a mat2d into a mat3
     *
     * @param out the receiving matrix
     * @param a the matrix to copy
     * @returns {mat3} out
     **/
    static mat3 fromMat2d(mat3& out, const mat2d a);
    /**
    * Calculates a 3x3 matrix from the given quaternion
    *
    * @param out mat3 receiving operation result
    * @param q Quaternion to create matrix from
    *
    * @returns {mat3} out
    */
    static mat3 fromQuat(mat3& out, const quat q);
    /**
    * Calculates a 3x3 normal matrix (transpose inverse) from the 4x4 matrix
    *
    * @param out mat3 receiving operation result
    * @param a Mat4 to derive the normal matrix from
    *
    * @returns {mat3} out
    */
    static mat3 normalFromMat4(mat3& out, const mat4 a);
    /**
     * Returns Frobenius norm of a mat3
     *
     * @param a the matrix to calculate Frobenius norm of
     * @returns {Number} Frobenius norm
     */
    static GLfloat frob(const mat3 a);
    /**
     * Adds two mat3's
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat3} out
     */
    static mat3 add(mat3& out, const mat3 a, const mat3 b);
    /**
     * Subtracts matrix b from matrix a
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat3} out
     */
    static mat3 subtract(mat3& out, const mat3 a, const mat3 b);
    /**
     * Multiply each element of the matrix by a scalar.
     *
     * @param out the receiving matrix
     * @param a the matrix to scale
     * @param b amount to scale the matrix's elements by
     * @returns {mat3} out
     */
    static mat3 multiplyScalar(mat3& out, const mat3 a, const GLfloat b);
    /**
     * Adds two mat3's after multiplying each element of the second operand by a scalar value.
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param scale the amount to scale b's elements by before adding
     * @returns {mat3} out
     */
    static mat3 multiplyScalarAndAdd(mat3& out, const mat3 a, const mat3 b, const GLfloat scale);
    /**
     * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool exactEquals(const mat3 a, const mat3 b);
    /**
     * Returns whether or not the matrices have approximately the same elements in the same position.
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool equals(const mat3 a, const mat3 b);
    /**
     * Returns a string representation of a mat3
     *
     * @param a to represent as a string
     * @returns {String} string representation of the mat3
     */
    static std::string str(const mat3& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {mat3} out
     */
    static mat3 zero(mat3& out);
    /**
     * calls glUniformMatrix3fv for the matrix a
     *
     * @param loc the location of the vector
     * @param a the vector to send to OpenGL shader
     * @returns {void}
     */
    static void glUniformMatrix(const GLint loc, const mat3 a);
};
/**
 * @class 4x4 Matrix
 * @name mat4
 */
class mat4 {

private:

    GLfloat m_Cells[16];

    friend class vec2;
    friend class vec3;
    friend class vec4;
    friend class mat2;
    friend class mat2d;
    friend class mat3;
    friend class quat;

public:

    static const mat4 null;

    mat4()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
        m_Cells[2] = NAN;
        m_Cells[3] = NAN;
        m_Cells[4] = NAN;
        m_Cells[5] = NAN;
        m_Cells[6] = NAN;
        m_Cells[7] = NAN;
        m_Cells[8] = NAN;
        m_Cells[9] = NAN;
        m_Cells[10] = NAN;
        m_Cells[11] = NAN;
        m_Cells[12] = NAN;
        m_Cells[13] = NAN;
        m_Cells[14] = NAN;
        m_Cells[15] = NAN;
    }

    /**
     * Access to individual components of mat4
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const mat4& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1] &&
            a.m_Cells[2] == m_Cells[2] &&
            a.m_Cells[3] == m_Cells[3] &&
            a.m_Cells[4] == m_Cells[4] &&
            a.m_Cells[5] == m_Cells[5] &&
            a.m_Cells[6] == m_Cells[6] &&
            a.m_Cells[7] == m_Cells[7] &&
            a.m_Cells[8] == m_Cells[8] &&
            a.m_Cells[9] == m_Cells[9] &&
            a.m_Cells[10] == m_Cells[10] &&
            a.m_Cells[11] == m_Cells[11] &&
            a.m_Cells[12] == m_Cells[12] &&
            a.m_Cells[13] == m_Cells[13] &&
            a.m_Cells[14] == m_Cells[14] &&
            a.m_Cells[15] == m_Cells[15];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const mat4& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1] ||
            a.m_Cells[2] != m_Cells[2] ||
            a.m_Cells[3] != m_Cells[3] ||
            a.m_Cells[4] != m_Cells[4] ||
            a.m_Cells[5] != m_Cells[5] ||
            a.m_Cells[6] != m_Cells[6] ||
            a.m_Cells[7] != m_Cells[7] ||
            a.m_Cells[8] != m_Cells[8] ||
            a.m_Cells[9] != m_Cells[9] ||
            a.m_Cells[10] != m_Cells[10] ||
            a.m_Cells[11] != m_Cells[11] ||
            a.m_Cells[12] != m_Cells[12] ||
            a.m_Cells[13] != m_Cells[13] ||
            a.m_Cells[14] != m_Cells[14] ||
            a.m_Cells[15] != m_Cells[15];
    }
    /**
     * Creates a new identity mat4
     *
     * @returns {mat4} a new 4x4 matrix
     */
    static mat4 create();
    /**
     * Creates a new mat4 initialized with values from an existing matrix
     *
     * @param a matrix to clone
     * @returns {mat4} a new 4x4 matrix
     */
    static mat4 clone(const mat4 a);
    /**
     * Copy the values from one mat4 to another
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat4} out
     */
    static mat4 copy(mat4& out, const mat4 a);
    /**
     * Create a new mat4 with the given values
     *
     * @param m00 Component in column 0, row 0 position (index 0)
     * @param m01 Component in column 0, row 1 position (index 1)
     * @param m02 Component in column 0, row 2 position (index 2)
     * @param m03 Component in column 0, row 3 position (index 3)
     * @param m10 Component in column 1, row 0 position (index 4)
     * @param m11 Component in column 1, row 1 position (index 5)
     * @param m12 Component in column 1, row 2 position (index 6)
     * @param m13 Component in column 1, row 3 position (index 7)
     * @param m20 Component in column 2, row 0 position (index 8)
     * @param m21 Component in column 2, row 1 position (index 9)
     * @param m22 Component in column 2, row 2 position (index 10)
     * @param m23 Component in column 2, row 3 position (index 11)
     * @param m30 Component in column 3, row 0 position (index 12)
     * @param m31 Component in column 3, row 1 position (index 13)
     * @param m32 Component in column 3, row 2 position (index 14)
     * @param m33 Component in column 3, row 3 position (index 15)
     * @returns {mat4} A new mat4
     */
    static mat4 fromValues(const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m03, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m13, const GLfloat m20, const GLfloat m21, const GLfloat m22, const GLfloat m23, const GLfloat m30, const GLfloat m31, const GLfloat m32, const GLfloat m33);
    /**
     * Set the components of a mat4 to the given values
     *
     * @param out the receiving matrix
     * @param m00 Component in column 0, row 0 position (index 0)
     * @param m01 Component in column 0, row 1 position (index 1)
     * @param m02 Component in column 0, row 2 position (index 2)
     * @param m03 Component in column 0, row 3 position (index 3)
     * @param m10 Component in column 1, row 0 position (index 4)
     * @param m11 Component in column 1, row 1 position (index 5)
     * @param m12 Component in column 1, row 2 position (index 6)
     * @param m13 Component in column 1, row 3 position (index 7)
     * @param m20 Component in column 2, row 0 position (index 8)
     * @param m21 Component in column 2, row 1 position (index 9)
     * @param m22 Component in column 2, row 2 position (index 10)
     * @param m23 Component in column 2, row 3 position (index 11)
     * @param m30 Component in column 3, row 0 position (index 12)
     * @param m31 Component in column 3, row 1 position (index 13)
     * @param m32 Component in column 3, row 2 position (index 14)
     * @param m33 Component in column 3, row 3 position (index 15)
     * @returns {mat4} out
     */
    static mat4 set(mat4& out, const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m03, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m13, const GLfloat m20, const GLfloat m21, const GLfloat m22, const GLfloat m23, const GLfloat m30, const GLfloat m31, const GLfloat m32, const GLfloat m33);
    /**
     * Set a mat4 to the identity matrix
     *
     * @param out the receiving matrix
     * @returns {mat4} out
     */
    static mat4 identity(mat4& out);
    /**
     * Transpose the values of a mat4 not using SIMD
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat4} out
     */
    static mat4 transpose(mat4& out, const mat4 a);
    /**
     * Inverts a mat4 not using SIMD
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat4} out
     */
    static mat4 invert(mat4& out, const mat4 a);
    /**
     * Calculates the adjugate of a mat4 not using SIMD
     *
     * @param out the receiving matrix
     * @param a the source matrix
     * @returns {mat4} out
     */
    static mat4 adjoint(mat4& out, const mat4 a);
    /**
     * Calculates the determinant of a mat4
     *
     * @param a the source matrix
     * @returns {Number} determinant of a
     */
    static GLfloat determinant(const mat4 a);
    /**
     * Multiplies two mat4's explicitly not using SIMD
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat4} out
     */
    static mat4 multiply(mat4& out, const mat4 a, const mat4 b);
    /**
     * Translate a mat4 by the given vector not using SIMD
     *
     * @param out the receiving matrix
     * @param a the matrix to translate
     * @param v vector to translate by
     * @returns {mat4} out
     */
    static mat4 translate(mat4& out, const mat4 a, const vec3 v);
    /**
     * Scales the mat4 by the dimensions in the given vec3 not using vectorization
     *
     * @param out the receiving matrix
     * @param a the matrix to scale
     * @param v the vec3 to scale the matrix by
     * @returns {mat4} out
     **/
    static mat4 scale(mat4& out, const mat4 a, const vec3 v);
    /**
     * Rotates a mat4 by the given angle around the given axis
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param rad the angle to rotate the matrix by
     * @param axis the axis to rotate around
     * @returns {mat4} out
     */
    static mat4 rotate(mat4& out, const mat4 a, const GLfloat rad, const vec3 axis);
    /**
     * Rotates a matrix by the given angle around the X axis not using SIMD
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param rad the angle to rotate the matrix by
     * @returns {mat4} out
     */
    static mat4 rotateX(mat4& out, const mat4 a, const GLfloat rad);
    /**
     * Rotates a matrix by the given angle around the Y axis not using SIMD
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param rad the angle to rotate the matrix by
     * @returns {mat4} out
     */
    static mat4 rotateY(mat4& out, const mat4 a, const GLfloat rad);
    /**
     * Rotates a matrix by the given angle around the Z axis not using SIMD
     *
     * @param out the receiving matrix
     * @param a the matrix to rotate
     * @param rad the angle to rotate the matrix by
     * @returns {mat4} out
     */
    static mat4 rotateZ(mat4& out, const mat4 a, const GLfloat rad);
    /**
     * Creates a matrix from a vector translation
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.translate(dest, dest, vec);
     *
     * @param out mat4 receiving operation result
     * @param v Translation vector
     * @returns {mat4} out
     */
    static mat4 fromTranslation(mat4& out, const vec3 v);
    /**
     * Creates a matrix from a vector scaling
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.scale(dest, dest, vec);
     *
     * @param out mat4 receiving operation result
     * @param v Scaling vector
     * @returns {mat4} out
     */
    static mat4 fromScaling(mat4& out, const vec3 v);
    /**
     * Creates a matrix from a given angle around a given axis
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.rotate(dest, dest, rad, axis);
     *
     * @param out mat4 receiving operation result
     * @param rad the angle to rotate the matrix by
     * @param axis the axis to rotate around
     * @returns {mat4} out
     */
    static mat4 fromRotation(mat4& out, const GLfloat rad, const vec3 axis);
    /**
     * Creates a matrix from the given angle around the X axis
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.rotateX(dest, dest, rad);
     *
     * @param out mat4 receiving operation result
     * @param rad the angle to rotate the matrix by
     * @returns {mat4} out
     */
    static mat4 fromXRotation(mat4& out, const GLfloat rad);
    /**
     * Creates a matrix from the given angle around the Y axis
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.rotateY(dest, dest, rad);
     *
     * @param out mat4 receiving operation result
     * @param rad the angle to rotate the matrix by
     * @returns {mat4} out
     */
    static mat4 fromYRotation(mat4& out, const GLfloat rad);
    /**
     * Creates a matrix from the given angle around the Z axis
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.rotateZ(dest, dest, rad);
     *
     * @param out mat4 receiving operation result
     * @param rad the angle to rotate the matrix by
     * @returns {mat4} out
     */
    static mat4 fromZRotation(mat4& out, const GLfloat rad);
    /**
     * Creates a matrix from a quaternion rotation and vector translation
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.translate(dest, vec);
     *     var quatMat = mat4.create();
     *     quat.toMat4(quat, quatMat);
     *     mat4.multiply(dest, quatMat);
     *
     * @param out mat4 receiving operation result
     * @param q Rotation quaternion
     * @param v Translation vector
     * @returns {mat4} out
     */
    static mat4 fromRotationTranslation(mat4& out, const quat q, const vec3 v);
    /**
     * Returns the translation vector component of a transformation
     *  matrix. If a matrix is built with fromRotationTranslation,
     *  the returned vector will be the same as the translation vector
     *  originally supplied.
     * @param  {vec3} out Vector to receive translation component
     * @param  {mat4} mat Matrix to be decomposed (input)
     * @returns {vec3} out
     */
    static vec3 getTranslation(vec3& out, const mat4 mat);
    /**
     * Returns a quaternion representing the rotational component
     *  of a transformation matrix. If a matrix is built with
     *  fromRotationTranslation, the returned quaternion will be the
     *  same as the quaternion originally supplied.
     * @param out Quaternion to receive the rotation component
     * @param mat Matrix to be decomposed (input)
     * @returns {quat} out
     */
    static quat getRotation(quat& out, const mat4 mat);
    /**
     * Creates a matrix from a quaternion rotation, vector translation and vector scale
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.translate(dest, vec);
     *     var quatMat = mat4.create();
     *     quat.toMat4(quat, quatMat);
     *     mat4.multiply(dest, quatMat);
     *     mat4.scale(dest, scale)
     *
     * @param out mat4 receiving operation result
     * @param q Rotation quaternion
     * @param v Translation vector
     * @param s Scaling vector
     * @returns {mat4} out
     */
    static mat4 fromRotationTranslationScale(mat4& out, const quat q, const vec3 v, const vec3 s);
    /**
     * Creates a matrix from a quaternion rotation, vector translation and vector scale, rotating and scaling around the given origin
     * This is equivalent to (but much faster than):
     *
     *     mat4.identity(dest);
     *     mat4.translate(dest, vec);
     *     mat4.translate(dest, origin);
     *     var quatMat = mat4.create();
     *     quat.toMat4(quat, quatMat);
     *     mat4.multiply(dest, quatMat);
     *     mat4.scale(dest, scale)
     *     mat4.translate(dest, negativeOrigin);
     *
     * @param out mat4 receiving operation result
     * @param q Rotation quaternion
     * @param v Translation vector
     * @param s Scaling vector
     * @param o The origin vector around which to scale and rotate
     * @returns {mat4} out
     */
    static mat4 fromRotationTranslationScaleOrigin(mat4& out, const quat q, const vec3 v, const vec3 s, const vec3 o);
    /**
     * Calculates a 4x4 matrix from the given quaternion
     *
     * @param out mat4 receiving operation result
     * @param q Quaternion to create matrix from
     *
     * @returns {mat4} out
     */
    static mat4 fromQuat(mat4& out, const quat q);
    /**
     * Generates a frustum matrix with the given bounds
     *
     * @param out mat4 frustum matrix will be written into
     * @param left Left bound of the frustum
     * @param right Right bound of the frustum
     * @param bottom Bottom bound of the frustum
     * @param top Top bound of the frustum
     * @param near Near bound of the frustum
     * @param far Far bound of the frustum
     * @returns {mat4} out
     */
    static mat4 frustum(mat4& out, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat near, const GLfloat far);
    /**
     * Generates a perspective projection matrix with the given bounds
     *
     * @param out mat4 frustum matrix will be written into
     * @param fovy Vertical field of view in radians
     * @param aspect Aspect ratio. typically viewport width/height
     * @param near Near bound of the frustum
     * @param far Far bound of the frustum
     * @returns {mat4} out
     */
    static mat4 perspective(mat4& out, const GLfloat fovy, const GLfloat aspect, const GLfloat near, const GLfloat far);
    /**
     * Generates a orthogonal projection matrix with the given bounds
     *
     * @param out mat4 frustum matrix will be written into
     * @param left Left bound of the frustum
     * @param right Right bound of the frustum
     * @param bottom Bottom bound of the frustum
     * @param top Top bound of the frustum
     * @param near Near bound of the frustum
     * @param far Far bound of the frustum
     * @returns {mat4} out
     */
    static mat4 ortho(mat4& out, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat near, const GLfloat far);
    /**
     * Generates a look-at matrix with the given eye position, focal point, and up axis
     *
     * @param out mat4 frustum matrix will be written into
     * @param eye Position of the viewer
     * @param center Point the viewer is looking at
     * @param up vec3 pointing up
     * @returns {mat4} out
     */
    static mat4 lookAt(mat4& out, const vec3 eye, const vec3 center, const vec3 up);
    /**
     * Returns Frobenius norm of a mat4
     *
     * @param a the matrix to calculate Frobenius norm of
     * @returns {Number} Frobenius norm
     */
    static GLfloat frob(const mat4 a);
    /**
     * Adds two mat4's
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat4} out
     */
    static mat4 add(mat4& out, const mat4 a, const mat4 b);
    /**
     * Subtracts matrix b from matrix a
     *
     * @param out the receiving matrix
     * @param a the first operand
     * @param b the second operand
     * @returns {mat4} out
     */
    static mat4 subtract(mat4& out, const mat4 a, const mat4 b);
    /**
     * Multiply each element of the matrix by a scalar.
     *
     * @param out the receiving matrix
     * @param a the matrix to scale
     * @param b amount to scale the matrix's elements by
     * @returns {mat4} out
     */
    static mat4 multiplyScalar(mat4& out, const mat4 a, const GLfloat b);
    /**
     * Adds two mat4's after multiplying each element of the second operand by a scalar value.
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param scale the amount to scale b's elements by before adding
     * @returns {mat4} out
     */
    static mat4 multiplyScalarAndAdd(mat4& out, const mat4 a, const mat4 b, const GLfloat scale);
    /**
     * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool exactEquals(const mat4 a, const mat4 b);
    /**
     * Returns whether or not the matrices have approximately the same elements in the same position.
     *
     * @param a The first matrix.
     * @param b The second matrix.
     * @returns {Boolean} True if the matrices are equal, false otherwise.
     */
    static bool equals(const mat4 a, const mat4 b);
    /**
     * Returns a string representation of a mat4
     *
     * @param a to represent as a string
     * @returns {String} string representation of the mat4
     */
    static std::string str(const mat4& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {mat4} out
     */
    static mat4 zero(mat4& out);
    /**
     * calls glUniformMatrix4fv for the matrix a
     *
     * @param loc the location of the vector
     * @param a the vector to send to OpenGL shader
     * @returns {void}
     */
    static void glUniformMatrix(const GLint loc, const mat4 a);
};
/**
 * @class 2 Dimensional Vector
 * @name vec2
 */
class vec2 {

private:

    GLfloat m_Cells[2];

    friend class vec3;
    friend class vec4;
    friend class mat2;
    friend class mat2d;
    friend class mat3;
    friend class mat4;
    friend class quat;

public:

    static const vec2 null;

    vec2()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
    }

    /**
     * Access to individual components of vec2
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const vec2& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const vec2& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1];
    }
    /**
     * Creates a new, empty vec2
     *
     * @returns {vec2} a new 2D vector
     */
    static vec2 create();
    /**
     * Creates a new vec2 initialized with values from an existing vector
     *
     * @param a vector to clone
     * @returns {vec2} a new 2D vector
     */
    static vec2 clone(const vec2 a);
    /**
     * Creates a new vec2 initialized with the given values
     *
     * @param x X component
     * @param y Y component
     * @returns {vec2} a new 2D vector
     */
    static vec2 fromValues(const GLfloat x, const GLfloat y);
    /**
     * Copy the values from one vec2 to another
     *
     * @param out the receiving vector
     * @param a the source vector
     * @returns {vec2} out
     */
    static vec2 copy(vec2& out, const vec2 a);
    /**
     * Set the components of a vec2 to the given values
     *
     * @param out the receiving vector
     * @param x X component
     * @param y Y component
     * @returns {vec2} out
     */
    static vec2 set(vec2& out, const GLfloat x, const GLfloat y);
    /**
     * Adds two vec2's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec2} out
     */
    static vec2 add(vec2& out, const vec2 a, const vec2 b);
    /**
     * Subtracts vector b from vector a
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec2} out
     */
    static vec2 subtract(vec2& out, const vec2 a, const vec2 b);
    /**
     * Multiplies two vec2's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec2} out
     */
    static vec2 multiply(vec2& out, const vec2 a, const vec2 b);
    /**
     * Divides two vec2's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec2} out
     */
    static vec2 divide(vec2& out, const vec2 a, const vec2 b);
    /**
     * Math.ceil the components of a vec2
     *
     * @param out the receiving vector
     * @param a vector to ceil
     * @returns {vec2} out
     */
    static vec2 ceil(vec2& out, const vec2 a);
    /**
     * Math.floor the components of a vec2
     *
     * @param out the receiving vector
     * @param a vector to floor
     * @returns {vec2} out
     */
    static vec2 floor(vec2& out, const vec2 a);
    /**
     * Returns the minimum of two vec2's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec2} out
     */
    static vec2 min(vec2& out, const vec2 a, const vec2 b);
    /**
     * Returns the maximum of two vec2's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec2} out
     */
    static vec2 max(vec2& out, const vec2 a, const vec2 b);
    /**
     * Math.round the components of a vec2
     *
     * @param out the receiving vector
     * @param a vector to round
     * @returns {vec2} out
     */
    static vec2 round(vec2& out, const vec2 a);
    /**
     * Scales a vec2 by a scalar Number
     *
     * @param out the receiving vector
     * @param a the vector to scale
     * @param b amount to scale the vector by
     * @returns {vec2} out
     */
    static vec2 scale(vec2& out, const vec2 a, const GLfloat b);
    /**
     * Adds two vec2's after scaling the second operand by a scalar value
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param scale the amount to scale b by before adding
     * @returns {vec2} out
     */
    static vec2 scaleAndAdd(vec2& out, const vec2 a, const vec2 b, const GLfloat scale);
    /**
     * Calculates the euclidian distance between two vec2's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} distance between a and b
     */
    static GLfloat distance(const vec2 a, const vec2 b);
    /**
     * Calculates the squared euclidian distance between two vec2's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} squared distance between a and b
     */
    static GLfloat squaredDistance(const vec2 a, const vec2 b);
    /**
     * Calculates the length of a vec2
     *
     * @param a vector to calculate length of
     * @returns {Number} length of a
     */
    static GLfloat length(const vec2 a);
    /**
     * Calculates the squared length of a vec2
     *
     * @param a vector to calculate squared length of
     * @returns {Number} squared length of a
     */
    static GLfloat squaredLength(const vec2 a);
    /**
     * Negates the components of a vec2
     *
     * @param out the receiving vector
     * @param a vector to negate
     * @returns {vec2} out
     */
    static vec2 negate(vec2& out, const vec2 a);
    /**
     * Returns the inverse of the components of a vec2
     *
     * @param out the receiving vector
     * @param a vector to invert
     * @returns {vec2} out
     */
    static vec2 inverse(vec2& out, const vec2 a);
    /**
     * Normalize a vec2
     *
     * @param out the receiving vector
     * @param a vector to normalize
     * @returns {vec2} out
     */
    static vec2 normalize(vec2& out, const vec2 a);
    /**
     * Calculates the dot product of two vec2's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} dot product of a and b
     */
    static GLfloat dot(const vec2 a, const vec2 b);
    /**
     * Computes the cross product of two vec2's
     * Note that the cross product must by definition produce a 3D vector
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 cross(vec3& out, const vec2 a, const vec2 b);
    /**
     * Performs a linear interpolation between two vec2's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param t interpolation amount between the two inputs
     * @returns {vec2} out
     */
    static vec2 lerp(vec2& out, const vec2 a, const vec2 b, const GLfloat t);
    /**
     * Generates a random vector with the given scale
     *
     * @param out the receiving vector
     * @param [scale] Length of the resulting vector. If ommitted, a unit vector will be returned
     * @returns {vec2} out
     */
    static vec2 random(vec2& out, const GLfloat scale);
    /**
     * Transforms the vec2 with a mat2
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param m matrix to transform with
     * @returns {vec2} out
     */
    static vec2 transformMat2(vec2& out, const vec2 a, const mat2 m);
    /**
     * Transforms the vec2 with a mat2d
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param m matrix to transform with
     * @returns {vec2} out
     */
    static vec2 transformMat2d(vec2& out, const vec2 a, const mat2d m);
    /**
     * Transforms the vec2 with a mat3
     * 3rd vector component is implicitly '1'
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param m matrix to transform with
     * @returns {vec2} out
     */
    static vec2 transformMat3(vec2& out, const vec2 a, const mat3 m);
    /**
     * Transforms the vec2 with a mat4
     * 3rd vector component is implicitly '0'
     * 4th vector component is implicitly '1'
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param m matrix to transform with
     * @returns {vec2} out
     */
    static vec2 transformMat4(vec2& out, const vec2 a, const mat4 m);
    /**
     * Returns whether or not the vectors exactly have the same elements in the same position (when compared with ===)
     *
     * @param a The first vector.
     * @param b The second vector.
     * @returns {Boolean} True if the vectors are equal, false otherwise.
     */
    static bool exactEquals(const vec2 a, const vec2 b);
    /**
     * Returns whether or not the vectors have approximately the same elements in the same position.
     *
     * @param a The first vector.
     * @param b The second vector.
     * @returns {Boolean} True if the vectors are equal, false otherwise.
     */
    static bool equals(const vec2 a, const vec2 b);
    /**
     * Returns a string representation of a vec2
     *
     * @param a to represent as a string
     * @returns {String} string representation of the vec2
     */
    static std::string str(const vec2& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {vec2} out
     */
    static vec2 zero(vec2& out);
    /**
     * Creates a new vec2 initialized from the given vec2
     *
     * @param a
     * @returns {vec2} a new vector
     */
    static vec2 fromVec(const vec2 a);
    /**
     * Creates a new vec2 initialized from the given vec3
     *
     * @param a
     * @returns {vec2} a new vector
     */
    static vec2 fromVec(const vec3 a);
    /**
     * Creates a new vec2 initialized from the given vec4
     *
     * @param a
     * @returns {vec2} a new vector
     */
    static vec2 fromVec(const vec4 a);
    /**
     * Cette méthode calcule le vec2 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
     * @param out the receiving vector
     * @param p0 : coordonnées à k=0
     * @param t0 : tangente à k=0
     * @param p1 : coordonnées à k=1
     * @param t1 : tangente à k=1
     * @param k coefficient entre 0 et 1
     * @returns {vec2} out
     */
    static vec2 hermite(vec2& out, const vec2 p0, const vec2 t0, const vec2 p1, const vec2 t1, const GLfloat k);
    /**
     * Performs a bezier interpolation with two control points
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param c the third operand
     * @param d the fourth operand
     * @param t interpolation amount between the two inputs
     * @returns {vec2} out
     */
    static vec2 bezier(vec2& out, const vec2 a, const vec2 b, const vec2 c, const vec2 d, const GLfloat t);
    /**
     * calls glUniform2fv for the vector a
     *
     * @param loc the location of the vector
     * @param a the vector to send to OpenGL shader
     * @returns {void}
     */
    static void glUniform(const GLint loc, const vec2 a);
    /**
     * calls glUniform2fv for the vector array a
     *
     * @param loc the location of the vector
     * @param a the array of vectors to send to OpenGL shader
     * @returns {void}
     */
    static void glUniform(const GLint loc, const std::vector<vec2>& a);
};
/**
 * @class 3 Dimensional Vector
 * @name vec3
 */
class vec3 {

private:

    GLfloat m_Cells[3];

    friend class vec2;
    friend class vec4;
    friend class mat2;
    friend class mat2d;
    friend class mat3;
    friend class mat4;
    friend class quat;

public:

    static const vec3 null;

    vec3()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
        m_Cells[2] = NAN;
    }

    /**
     * Access to individual components of vec3
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const vec3& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1] &&
            a.m_Cells[2] == m_Cells[2];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const vec3& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1] ||
            a.m_Cells[2] != m_Cells[2];
    }
    /**
     * Creates a new, empty vec3
     *
     * @returns {vec3} a new 3D vector
     */
    static vec3 create();
    /**
     * Creates a new vec3 initialized with values from an existing vector
     *
     * @param a vector to clone
     * @returns {vec3} a new 3D vector
     */
    static vec3 clone(const vec3 a);
    /**
     * Creates a new vec3 initialized with the given values
     *
     * @param x X component
     * @param y Y component
     * @param z Z component
     * @returns {vec3} a new 3D vector
     */
    static vec3 fromValues(const GLfloat x, const GLfloat y, const GLfloat z);
    /**
     * Copy the values from one vec3 to another
     *
     * @param out the receiving vector
     * @param a the source vector
     * @returns {vec3} out
     */
    static vec3 copy(vec3& out, const vec3 a);
    /**
     * Set the components of a vec3 to the given values
     *
     * @param out the receiving vector
     * @param x X component
     * @param y Y component
     * @param z Z component
     * @returns {vec3} out
     */
    static vec3 set(vec3& out, const GLfloat x, const GLfloat y, const GLfloat z);
    /**
     * Adds two vec3's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 add(vec3& out, const vec3 a, const vec3 b);
    /**
     * Subtracts vector b from vector a
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 subtract(vec3& out, const vec3 a, const vec3 b);
    /**
     * Multiplies two vec3's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 multiply(vec3& out, const vec3 a, const vec3 b);
    /**
     * Divides two vec3's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 divide(vec3& out, const vec3 a, const vec3 b);
    /**
     * Math.ceil the components of a vec3
     *
     * @param out the receiving vector
     * @param a vector to ceil
     * @returns {vec3} out
     */
    static vec3 ceil(vec3& out, const vec3 a);
    /**
     * Math.floor the components of a vec3
     *
     * @param out the receiving vector
     * @param a vector to floor
     * @returns {vec3} out
     */
    static vec3 floor(vec3& out, const vec3 a);
    /**
     * Returns the minimum of two vec3's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 min(vec3& out, const vec3 a, const vec3 b);
    /**
     * Returns the maximum of two vec3's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 max(vec3& out, const vec3 a, const vec3 b);
    /**
     * Math.round the components of a vec3
     *
     * @param out the receiving vector
     * @param a vector to round
     * @returns {vec3} out
     */
    static vec3 round(vec3& out, const vec3 a);
    /**
     * Scales a vec3 by a scalar Number
     *
     * @param out the receiving vector
     * @param a the vector to scale
     * @param b amount to scale the vector by
     * @returns {vec3} out
     */
    static vec3 scale(vec3& out, const vec3 a, const GLfloat b);
    /**
     * Adds two vec3's after scaling the second operand by a scalar value
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param scale the amount to scale b by before adding
     * @returns {vec3} out
     */
    static vec3 scaleAndAdd(vec3& out, const vec3 a, const vec3 b, const GLfloat scale);
    /**
     * Calculates the euclidian distance between two vec3's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} distance between a and b
     */
    static GLfloat distance(const vec3 a, const vec3 b);
    /**
     * Calculates the squared euclidian distance between two vec3's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} squared distance between a and b
     */
    static GLfloat squaredDistance(const vec3 a, const vec3 b);
    /**
     * Calculates the length of a vec3
     *
     * @param a vector to calculate length of
     * @returns {Number} length of a
     */
    static GLfloat length(const vec3 a);
    /**
     * Calculates the squared length of a vec3
     *
     * @param a vector to calculate squared length of
     * @returns {Number} squared length of a
     */
    static GLfloat squaredLength(const vec3 a);
    /**
     * Negates the components of a vec3
     *
     * @param out the receiving vector
     * @param a vector to negate
     * @returns {vec3} out
     */
    static vec3 negate(vec3& out, const vec3 a);
    /**
     * Returns the inverse of the components of a vec3
     *
     * @param out the receiving vector
     * @param a vector to invert
     * @returns {vec3} out
     */
    static vec3 inverse(vec3& out, const vec3 a);
    /**
     * Normalize a vec3
     *
     * @param out the receiving vector
     * @param a vector to normalize
     * @returns {vec3} out
     */
    static vec3 normalize(vec3& out, const vec3 a);
    /**
     * Calculates the dot product of two vec3's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} dot product of a and b
     */
    static GLfloat dot(const vec3 a, const vec3 b);
    /**
     * Computes the cross product of two vec3's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec3} out
     */
    static vec3 cross(vec3& out, const vec3 a, const vec3 b);
    /**
     * Performs a linear interpolation between two vec3's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param t interpolation amount between the two inputs
     * @returns {vec3} out
     */
    static vec3 lerp(vec3& out, const vec3 a, const vec3 b, const GLfloat t);
    /**
     * Generates a random vector with the given scale
     *
     * @param out the receiving vector
     * @param [scale] Length of the resulting vector. If ommitted, a unit vector will be returned
     * @returns {vec3} out
     */
    static vec3 random(vec3& out, const GLfloat scale);
    /**
     * Transforms the vec3 with a mat4.
     * 4th vector component is implicitly '1'
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param m matrix to transform with
     * @returns {vec3} out
     */
    static vec3 transformMat4(vec3& out, const vec3 a, const mat4 m);
    /**
     * Transforms the vec3 with a mat3.
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param m the 3x3 matrix to transform with
     * @returns {vec3} out
     */
    static vec3 transformMat3(vec3& out, const vec3 a, const mat4 m);
    /**
     * Transforms the vec3 with a quat
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param q quaternion to transform with
     * @returns {vec3} out
     */
    static vec3 transformQuat(vec3& out, const vec3 a, const quat q);
    /**
     * Rotate a 3D vector around the x-axis
     * @param out The receiving vec3
     * @param a The vec3 point to rotate
     * @param b The origin of the rotation
     * @param c The angle of rotation
     * @returns {vec3} out
     */
    static vec3 rotateX(vec3& out, const vec3 a, const vec3 b, const GLfloat c);
    /**
     * Rotate a 3D vector around the y-axis
     * @param out The receiving vec3
     * @param a The vec3 point to rotate
     * @param b The origin of the rotation
     * @param c The angle of rotation
     * @returns {vec3} out
     */
    static vec3 rotateY(vec3& out, const vec3 a, const vec3 b, const GLfloat c);
    /**
     * Rotate a 3D vector around the z-axis
     * @param out The receiving vec3
     * @param a The vec3 point to rotate
     * @param b The origin of the rotation
     * @param c The angle of rotation
     * @returns {vec3} out
     */
    static vec3 rotateZ(vec3& out, const vec3 a, const vec3 b, const GLfloat c);
    /**
     * Get the angle between two 3D vectors
     * @param a The first operand
     * @param b The second operand
     * @returns {Number} The angle in radians
     */
    static GLfloat angle(const vec3 a, const vec3 b);
    /**
     * Returns whether or not the vectors have exactly the same elements in the same position (when compared with ===)
     *
     * @param a The first vector.
     * @param b The second vector.
     * @returns {Boolean} True if the vectors are equal, false otherwise.
     */
    static bool exactEquals(const vec3 a, const vec3 b);
    /**
     * Returns whether or not the vectors have approximately the same elements in the same position.
     *
     * @param a The first vector.
     * @param b The second vector.
     * @returns {Boolean} True if the vectors are equal, false otherwise.
     */
    static bool equals(const vec3 a, const vec3 b);
    /**
     * Returns a string representation of a vec3
     *
     * @param a to represent as a string
     * @returns {String} string representation of the vec3
     */
    static std::string str(const vec3& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {vec3} out
     */
    static vec3 zero(vec3& out);
    /**
     * Creates a new vec3 initialized from the given vec2
     *
     * @param a
     * @returns {vec3} a new vector
     */
    static vec3 fromVec(const vec2 a);
    /**
     * Creates a new vec3 initialized from the given vec3
     *
     * @param a
     * @returns {vec3} a new vector
     */
    static vec3 fromVec(const vec3 a);
    /**
     * Creates a new vec3 initialized from the given vec4
     *
     * @param a
     * @returns {vec3} a new vector
     */
    static vec3 fromVec(const vec4 a);
    /**
     * Cette méthode calcule le vec3 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
     * @param out the receiving vector
     * @param p0 : coordonnées à k=0
     * @param t0 : tangente à k=0
     * @param p1 : coordonnées à k=1
     * @param t1 : tangente à k=1
     * @param k coefficient entre 0 et 1
     * @returns {vec3} out
     */
    static vec3 hermite(vec3& out, const vec3 p0, const vec3 t0, const vec3 p1, const vec3 t1, const GLfloat k);
    /**
     * Performs a bezier interpolation with two control points
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param c the third operand
     * @param d the fourth operand
     * @param t interpolation amount between the two inputs
     * @returns {vec3} out
     */
    static vec3 bezier(vec3& out, const vec3 a, const vec3 b, const vec3 c, const vec3 d, const GLfloat t);
    /**
     * calls glUniform3fv for the vector a
     *
     * @param loc the location of the vector
     * @param a the vector to send to OpenGL shader
     * @returns {void}
     */
    static void glUniform(const GLint loc, const vec3 a);
    /**
     * calls glUniform3fv for the vector array a
     *
     * @param loc the location of the vector
     * @param a the array of vectors to send to OpenGL shader
     * @returns {void}
     */
    static void glUniform(const GLint loc, const std::vector<vec3>& a);
};
/**
 * @class 4 Dimensional Vector
 * @name vec4
 */
class vec4 {

private:

    GLfloat m_Cells[4];

    friend class vec2;
    friend class vec3;
    friend class mat2;
    friend class mat2d;
    friend class mat3;
    friend class mat4;
    friend class quat;

public:

    static const vec4 null;

    vec4()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
        m_Cells[2] = NAN;
        m_Cells[3] = NAN;
    }

    /**
     * Access to individual components of vec4
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const vec4& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1] &&
            a.m_Cells[2] == m_Cells[2] &&
            a.m_Cells[3] == m_Cells[3];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const vec4& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1] ||
            a.m_Cells[2] != m_Cells[2] ||
            a.m_Cells[3] != m_Cells[3];
    }
    /**
     * Creates a new, empty vec4
     *
     * @returns {vec4} a new 4D vector
     */
    static vec4 create();
    /**
     * Creates a new vec4 initialized with values from an existing vector
     *
     * @param a vector to clone
     * @returns {vec4} a new 4D vector
     */
    static vec4 clone(const vec4 a);
    /**
     * Creates a new vec4 initialized with the given values
     *
     * @param x X component
     * @param y Y component
     * @param z Z component
     * @param w W component
     * @returns {vec4} a new 4D vector
     */
    static vec4 fromValues(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);
    /**
     * Copy the values from one vec4 to another
     *
     * @param out the receiving vector
     * @param a the source vector
     * @returns {vec4} out
     */
    static vec4 copy(vec4& out, const vec4 a);
    /**
     * Set the components of a vec4 to the given values
     *
     * @param out the receiving vector
     * @param x X component
     * @param y Y component
     * @param z Z component
     * @param w W component
     * @returns {vec4} out
     */
    static vec4 set(vec4& out, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);
    /**
     * Adds two vec4's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec4} out
     */
    static vec4 add(vec4& out, const vec4 a, const vec4 b);
    /**
     * Subtracts vector b from vector a
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec4} out
     */
    static vec4 subtract(vec4& out, const vec4 a, const vec4 b);
    /**
     * Multiplies two vec4's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec4} out
     */
    static vec4 multiply(vec4& out, const vec4 a, const vec4 b);
    /**
     * Divides two vec4's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec4} out
     */
    static vec4 divide(vec4& out, const vec4 a, const vec4 b);
    /**
     * Math.ceil the components of a vec4
     *
     * @param out the receiving vector
     * @param a vector to ceil
     * @returns {vec4} out
     */
    static vec4 ceil(vec4& out, const vec4 a);
    /**
     * Math.floor the components of a vec4
     *
     * @param out the receiving vector
     * @param a vector to floor
     * @returns {vec4} out
     */
    static vec4 floor(vec4& out, const vec4 a);
    /**
     * Returns the minimum of two vec4's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec4} out
     */
    static vec4 min(vec4& out, const vec4 a, const vec4 b);
    /**
     * Returns the maximum of two vec4's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @returns {vec4} out
     */
    static vec4 max(vec4& out, const vec4 a, const vec4 b);
    /**
     * Math.round the components of a vec4
     *
     * @param out the receiving vector
     * @param a vector to round
     * @returns {vec4} out
     */
    static vec4 round(vec4& out, const vec4 a);
    /**
     * Scales a vec4 by a scalar Number
     *
     * @param out the receiving vector
     * @param a the vector to scale
     * @param b amount to scale the vector by
     * @returns {vec4} out
     */
    static vec4 scale(vec4& out, const vec4 a, const GLfloat b);
    /**
     * Adds two vec4's after scaling the second operand by a scalar value
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param scale the amount to scale b by before adding
     * @returns {vec4} out
     */
    static vec4 scaleAndAdd(vec4& out, const vec4 a, const vec4 b, const GLfloat scale);
    /**
     * Calculates the euclidian distance between two vec4's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} distance between a and b
     */
    static GLfloat distance(const vec4 a, const vec4 b);
    /**
     * Calculates the squared euclidian distance between two vec4's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} squared distance between a and b
     */
    static GLfloat squaredDistance(const vec4 a, const vec4 b);
    /**
     * Calculates the length of a vec4
     *
     * @param a vector to calculate length of
     * @returns {Number} length of a
     */
    static GLfloat length(const vec4 a);
    /**
     * Calculates the squared length of a vec4
     *
     * @param a vector to calculate squared length of
     * @returns {Number} squared length of a
     */
    static GLfloat squaredLength(const vec4 a);
    /**
     * Negates the components of a vec4
     *
     * @param out the receiving vector
     * @param a vector to negate
     * @returns {vec4} out
     */
    static vec4 negate(vec4& out, const vec4 a);
    /**
     * Returns the inverse of the components of a vec4
     *
     * @param out the receiving vector
     * @param a vector to invert
     * @returns {vec4} out
     */
    static vec4 inverse(vec4& out, const vec4 a);
    /**
     * Normalize a vec4
     *
     * @param out the receiving vector
     * @param a vector to normalize
     * @returns {vec4} out
     */
    static vec4 normalize(vec4& out, const vec4 a);
    /**
     * Calculates the dot product of two vec4's
     *
     * @param a the first operand
     * @param b the second operand
     * @returns {Number} dot product of a and b
     */
    static GLfloat dot(const vec4 a, const vec4 b);
    /**
     * Performs a linear interpolation between two vec4's
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param t interpolation amount between the two inputs
     * @returns {vec4} out
     */
    static vec4 lerp(vec4& out, const vec4 a, const vec4 b, const GLfloat t);
    /**
     * Generates a random vector with the given scale
     *
     * @param out the receiving vector
     * @param [scale] Length of the resulting vector. If ommitted, a unit vector will be returned
     * @returns {vec4} out
     */
    static vec4 random(vec4& out, const GLfloat scale);
    /**
     * Transforms the vec4 with a mat4.
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param m matrix to transform with
     * @returns {vec4} out
     */
    static vec4 transformMat4(vec4& out, const vec4 a, const mat4 m);
    /**
     * Transforms the vec4 with a quat
     *
     * @param out the receiving vector
     * @param a the vector to transform
     * @param q quaternion to transform with
     * @returns {vec4} out
     */
    static vec4 transformQuat(vec4& out, const vec4 a, const quat q);
    /**
     * Returns whether or not the vectors have exactly the same elements in the same position (when compared with ===)
     *
     * @param a The first vector.
     * @param b The second vector.
     * @returns {Boolean} True if the vectors are equal, false otherwise.
     */
    static bool exactEquals(const vec4 a, const vec4 b);
    /**
     * Returns whether or not the vectors have approximately the same elements in the same position.
     *
     * @param a The first vector.
     * @param b The second vector.
     * @returns {Boolean} True if the vectors are equal, false otherwise.
     */
    static bool equals(const vec4 a, const vec4 b);
    /**
     * Returns a string representation of a vec4
     *
     * @param a to represent as a string
     * @returns {String} string representation of the vec4
     */
    static std::string str(const vec4& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {vec4} out
     */
    static vec4 zero(vec4& out);
    /**
     * Creates a new vec4 initialized from the given vec2
     *
     * @param a
     * @returns {vec4} a new vector
     */
    static vec4 fromVec(const vec2 a);
    /**
     * Creates a new vec4 initialized from the given vec3
     *
     * @param a
     * @returns {vec4} a new vector
     */
    static vec4 fromVec(const vec3 a);
    /**
     * Creates a new vec4 initialized from the given vec4
     *
     * @param a
     * @returns {vec4} a new vector
     */
    static vec4 fromVec(const vec4 a);
    /**
     * Cette méthode calcule le vec4 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
     * @param out the receiving vector
     * @param p0 : coordonnées à k=0
     * @param t0 : tangente à k=0
     * @param p1 : coordonnées à k=1
     * @param t1 : tangente à k=1
     * @param k coefficient entre 0 et 1
     * @returns {vec4} out
     */
    static vec4 hermite(vec4& out, const vec4 p0, const vec4 t0, const vec4 p1, const vec4 t1, const GLfloat k);
    /**
     * Performs a bezier interpolation with two control points
     *
     * @param out the receiving vector
     * @param a the first operand
     * @param b the second operand
     * @param c the third operand
     * @param d the fourth operand
     * @param t interpolation amount between the two inputs
     * @returns {vec4} out
     */
    static vec4 bezier(vec4& out, const vec4 a, const vec4 b, const vec4 c, const vec4 d, const GLfloat t);
    /**
     * calls glUniform4fv for the vector a
     *
     * @param loc the location of the vector
     * @param a the vector to send to OpenGL shader
     * @returns {void}
     */
    static void glUniform(const GLint loc, const vec4 a);
    /**
     * calls glUniform4fv for the vector array a
     *
     * @param loc the location of the vector
     * @param a the array of vectors to send to OpenGL shader
     * @returns {void}
     */
    static void glUniform(const GLint loc, const std::vector<vec4>& a);
};
/**
 * @class Quaternion
 * @name quat
 */
class quat {

private:

    GLfloat m_Cells[4];

    friend class vec2;
    friend class vec3;
    friend class vec4;
    friend class mat2;
    friend class mat2d;
    friend class mat3;
    friend class mat4;

public:

    static const quat null;

    quat()
    {
        m_Cells[0] = NAN;
        m_Cells[1] = NAN;
        m_Cells[2] = NAN;
        m_Cells[3] = NAN;
    }

    /**
     * Access to individual components of quat
     *
     * @param i index of component
     * @return a reference on component i
     */
    GLfloat& operator[](int i) {
        return m_Cells[i];
    }
    /**
     * equality
     *
     * @param a to compare to
     * @return a == this
     */
    bool operator==(const quat& a) const {
        return
            a.m_Cells[0] == m_Cells[0] &&
            a.m_Cells[1] == m_Cells[1] &&
            a.m_Cells[2] == m_Cells[2] &&
            a.m_Cells[3] == m_Cells[3];
    }
    /**
     * difference
     * @param a to compare to
     * @return a != this
     */
    bool operator!=(const quat& a) const {
        return
            a.m_Cells[0] != m_Cells[0] ||
            a.m_Cells[1] != m_Cells[1] ||
            a.m_Cells[2] != m_Cells[2] ||
            a.m_Cells[3] != m_Cells[3];
    }
    /**
     * Creates a new identity quat
     *
     * @returns {quat} a new quaternion
     */
    static quat create();
    /**
     * Set a quat to the identity quaternion
     *
     * @param out the receiving quaternion
     * @returns {quat} out
     */
    static quat identity(quat& out);
    /**
     * Sets a quat from the given angle and rotation axis,
     * then returns it.
     *
     * @param out the receiving quaternion
     * @param axis the axis around which to rotate
     * @param rad the angle in radians
     * @returns {quat} out
     **/
    static quat setAxisAngle(quat& out, const vec3 axis, const GLfloat rad);
    /**
     * Gets the rotation axis and angle for a given
     *  quaternion. If a quaternion is created with
     *  setAxisAngle, this method will return the same
     *  values as providied in the original parameter list
     *  OR functionally equivalent values.
     * Example: The quaternion formed by axis [0, 0, 1] and
     *  angle -90 is the same as the quaternion formed by
     *  [0, 0, 1] and 270. This method favors the latter.
     * @param  {vec3} out_axis  Vector receiving the axis of rotation
     * @param  {quat} q     Quaternion to be decomposed
     * @returns {Number}     Angle, in radians, of the rotation
     */
    static GLfloat getAxisAngle(vec3& out_axis, const quat q);
    /**
     * Multiplies two quat's
     *
     * @param out the receiving quaternion
     * @param a the first operand
     * @param b the second operand
     * @returns {quat} out
     */
    static quat multiply(quat& out, const quat a, const quat b);
    /**
     * Rotates a quaternion by the given angle about the X axis
     *
     * @param out quat receiving operation result
     * @param a quat to rotate
     * @param rad angle (in radians) to rotate
     * @returns {quat} out
     */
    static quat rotateX(quat& out, const quat a, const GLfloat rad);
    /**
     * Rotates a quaternion by the given angle about the Y axis
     *
     * @param out quat receiving operation result
     * @param a quat to rotate
     * @param rad angle (in radians) to rotate
     * @returns {quat} out
     */
    static quat rotateY(quat& out, const quat a, const GLfloat rad);
    /**
     * Rotates a quaternion by the given angle about the Z axis
     *
     * @param out quat receiving operation result
     * @param a quat to rotate
     * @param rad angle (in radians) to rotate
     * @returns {quat} out
     */
    static quat rotateZ(quat& out, const quat a, const GLfloat rad);
    /**
     * Calculates the W component of a quat from the X, Y, and Z components.
     * Assumes that quaternion is 1 unit in length.
     * Any existing W component will be ignored.
     *
     * @param out the receiving quaternion
     * @param a quat to calculate W component of
     * @returns {quat} out
     */
    static quat calculateW(quat& out, const quat a);
    /**
     * Performs a spherical linear interpolation between two quat
     *
     * @param out the receiving quaternion
     * @param a the first operand
     * @param b the second operand
     * @param t interpolation amount between the two inputs
     * @returns {quat} out
     */
    static quat slerp(quat& out, const quat a, const quat b, const GLfloat t);
    /**
     * Calculates the inverse of a quat
     *
     * @param out the receiving quaternion
     * @param a quat to calculate inverse of
     * @returns {quat} out
     */
    static quat invert(quat& out, const quat a);
    /**
     * Calculates the conjugate of a quat
     * If the quaternion is normalized, this function is faster than quat.inverse and produces the same result.
     *
     * @param out the receiving quaternion
     * @param a quat to calculate conjugate of
     * @returns {quat} out
     */
    static quat conjugate(quat& out, const quat a);
    /**
     * Creates a quaternion from the given 3x3 rotation matrix.
     *
     * NOTE: The resultant quaternion is not normalized, so you should be sure
     * to renormalize the quaternion yourself where necessary.
     *
     * @param out the receiving quaternion
     * @param m rotation matrix
     * @returns {quat} out
     * @function
     */
    static quat fromMat3(quat& out, const mat3 m);
    /**
     * Returns a string representation of a quat
     *
     * @param a to represent as a string
     * @returns {String} string representation of the quat
     */
    static std::string str(const quat& a);
    /**
     * set all components to zero
     *
     * @param out the receiving vector
     * @returns {quat} out
     */
    static quat zero(quat& out);
    /**
     * Sets a quaternion to represent the shortest rotation from one
     * vector to another.
     *
     * Both vectors are assumed to be unit length.
     *
     * @param out the receiving quaternion.
     * @param a the initial vector
     * @param b the destination vector
     * @returns {quat} out
     */
    static quat rotationTo(quat& out, const vec3 a, const vec3 b);
    /**
     * Sets the specified quaternion with values corresponding to the given
     * axes. Each axis is a vec3 and is expected to be unit length and
     * perpendicular to all other specified axes.
     *
     * @param out the receiving quaternion.
     * @param view  the vector representing the viewing direction
     * @param right the vector representing the local "right" direction
     * @param up    the vector representing the local "up" direction
     * @returns {quat} out
     */
    static quat setAxes(quat& out, const vec3 view, const vec3 right, const vec3 up);
    /**
     * Performs a spherical linear interpolation with two control points
     *
     * @param out the receiving quaternion
     * @param a the first operand
     * @param b the second operand
     * @param c the third operand
     * @param d the fourth operand
     * @param t interpolation amount
     * @returns {quat} out
     */
    static quat sqlerp(quat& out, const quat a, const quat b, const quat c, const quat d, const GLfloat t);
    /**
     * Normalize a quat
     *
     * @param out the receiving quaternion
     * @param a quaternion to normalize
     * @returns {quat} out
     */
    static quat normalize(quat& out, const quat a);
};
#endif

