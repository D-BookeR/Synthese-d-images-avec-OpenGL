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
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <math.h>

#include <gl-matrix.h>

const mat2 mat2::null;

/**
 * Creates a new identity mat2
 *
 * @returns {mat2} a new 2x2 matrix
 */
mat2 mat2::create()
{
    mat2 out;
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 1;
    return out;
};

/**
 * Creates a new mat2 initialized with values from an existing matrix
 *
 * @param a matrix to clone
 * @returns {mat2} a new 2x2 matrix
 */
mat2 mat2::clone(const mat2 a)
{
    mat2 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    return out;
};

/**
 * Copy the values from one mat2 to another
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat2} out
 */
mat2 mat2::copy(mat2& out, const mat2 a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    return out;
};

/**
 * Set a mat2 to the identity matrix
 *
 * @param out the receiving matrix
 * @returns {mat2} out
 */
mat2 mat2::identity(mat2& out)
{
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 1;
    return out;
};

/**
 * Create a new mat2 with the given values
 *
 * @param m00 Component in column 0, row 0 position (index 0)
 * @param m01 Component in column 0, row 1 position (index 1)
 * @param m10 Component in column 1, row 0 position (index 2)
 * @param m11 Component in column 1, row 1 position (index 3)
 * @returns {mat2} out A new 2x2 matrix
 */
mat2 mat2::fromValues(const GLfloat m00, const GLfloat m01, const GLfloat m10, const GLfloat m11)
{
    mat2 out;
    out.m_Cells[0] = m00;
    out.m_Cells[1] = m01;
    out.m_Cells[2] = m10;
    out.m_Cells[3] = m11;
    return out;
};

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
mat2 mat2::set(mat2& out, const GLfloat m00, const GLfloat m01, const GLfloat m10, const GLfloat m11)
{
    out.m_Cells[0] = m00;
    out.m_Cells[1] = m01;
    out.m_Cells[2] = m10;
    out.m_Cells[3] = m11;
    return out;
};


/**
 * Transpose the values of a mat2
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat2} out
 */
mat2 mat2::transpose(mat2& out, const mat2 a)
{
    // If we are transposing ourselves we can skip a few steps but have to cache some values
    if (out == a) {
        GLfloat a1 = a.m_Cells[1];
        out.m_Cells[1] = a.m_Cells[2];
        out.m_Cells[2] = a1;
    } else {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[2];
        out.m_Cells[2] = a.m_Cells[1];
        out.m_Cells[3] = a.m_Cells[3];
    }

    return out;
};

/**
 * Inverts a mat2
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat2} out
 */
mat2 mat2::invert(mat2& out, const mat2 a)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],

    // Calculate the determinant
    det = a0 * a3 - a2 * a1;

    if (!det) {
        return null;
    }
    det = 1.0 / det;

    out.m_Cells[0] =  a3 * det;
    out.m_Cells[1] = -a1 * det;
    out.m_Cells[2] = -a2 * det;
    out.m_Cells[3] =  a0 * det;

    return out;
};

/**
 * Calculates the adjugate of a mat2
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat2} out
 */
mat2 mat2::adjoint(mat2& out, const mat2 a)
{
    // Caching this value is nessecary if out == a
    GLfloat a0 = a.m_Cells[0];
    out.m_Cells[0] =  a.m_Cells[3];
    out.m_Cells[1] = -a.m_Cells[1];
    out.m_Cells[2] = -a.m_Cells[2];
    out.m_Cells[3] =  a0;

    return out;
};

/**
 * Calculates the determinant of a mat2
 *
 * @param a the source matrix
 * @returns {Number} determinant of a
 */
GLfloat mat2::determinant(const mat2 a)
{
    return a.m_Cells[0] * a.m_Cells[3] - a.m_Cells[2] * a.m_Cells[1];
};

/**
 * Multiplies two mat2's
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat2} out
 */
mat2 mat2::multiply(mat2& out, const mat2 a, const mat2 b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3];
    GLfloat b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3];
    out.m_Cells[0] = a0 * b0 + a2 * b1;
    out.m_Cells[1] = a1 * b0 + a3 * b1;
    out.m_Cells[2] = a0 * b2 + a2 * b3;
    out.m_Cells[3] = a1 * b2 + a3 * b3;
    return out;
};

/**
 * Rotates a mat2 by the given angle
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param rad the angle to rotate the matrix by
 * @returns {mat2} out
 */
mat2 mat2::rotate(mat2& out, const mat2 a, const GLfloat rad)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],
    s = sin(rad),
    c = cos(rad);
    out.m_Cells[0] = a0 *  c + a2 * s;
    out.m_Cells[1] = a1 *  c + a3 * s;
    out.m_Cells[2] = a0 * -s + a2 * c;
    out.m_Cells[3] = a1 * -s + a3 * c;
    return out;
};

/**
 * Scales the mat2 by the dimensions in the given vec2
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param v the vec2 to scale the matrix by
 * @returns {mat2} out
 **/
mat2 mat2::scale(mat2& out, const mat2 a, const vec2 v)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],
    v0 = v.m_Cells[0], v1 = v.m_Cells[1];
    out.m_Cells[0] = a0 * v0;
    out.m_Cells[1] = a1 * v0;
    out.m_Cells[2] = a2 * v1;
    out.m_Cells[3] = a3 * v1;
    return out;
};

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
mat2 mat2::fromRotation(mat2& out, const GLfloat rad)
{
    GLfloat s = sin(rad),
    c = cos(rad);
    out.m_Cells[0] = c;
    out.m_Cells[1] = s;
    out.m_Cells[2] = -s;
    out.m_Cells[3] = c;
    return out;
}

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
mat2 mat2::fromScaling(mat2& out, const vec2 v)
{
    out.m_Cells[0] = v.m_Cells[0];
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = v.m_Cells[1];
    return out;
}

/**
 * Returns Frobenius norm of a mat2
 *
 * @param a the matrix to calculate Frobenius norm of
 * @returns {Number} Frobenius norm
 */
GLfloat mat2::frob(const mat2 a)
{
    return(sqrt(pow(a.m_Cells[0], 2) + pow(a.m_Cells[1], 2) + pow(a.m_Cells[2], 2) + pow(a.m_Cells[3], 2)));
};

/**
 * Adds two mat2's
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat2} out
 */
mat2 mat2::add(mat2& out, const mat2 a, const mat2 b)
{
    out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] + b.m_Cells[3];
    return out;
};

/**
 * Subtracts matrix b from matrix a
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat2} out
 */
mat2 mat2::subtract(mat2& out, const mat2 a, const mat2 b)
{
    out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] - b.m_Cells[3];
    return out;
};

/**
 * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat2::exactEquals(const mat2 a, const mat2 b)
{
    return a.m_Cells[0] == b.m_Cells[0] && a.m_Cells[1] == b.m_Cells[1] && a.m_Cells[2] == b.m_Cells[2] && a.m_Cells[3] == b.m_Cells[3];
};

/**
 * Returns whether or not the matrices have approximately the same elements in the same position.
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat2::equals(const mat2 a, const mat2 b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3];
    GLfloat b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3];
    return (fabs(a0 - b0) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a0), fabs(b0))) &&
    fabs(a1 - b1) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a1), fabs(b1))) &&
    fabs(a2 - b2) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a2), fabs(b2))) &&
    fabs(a3 - b3) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a3), fabs(b3))));
};

/**
 * Multiply each element of the matrix by a scalar.
 *
 * @param out the receiving matrix
 * @param a the matrix to scale
 * @param b amount to scale the matrix's elements by
 * @returns {mat2} out
 */
mat2 mat2::multiplyScalar(mat2& out, const mat2 a, const GLfloat b)
{
    out.m_Cells[0] = a.m_Cells[0] * b;
    out.m_Cells[1] = a.m_Cells[1] * b;
    out.m_Cells[2] = a.m_Cells[2] * b;
    out.m_Cells[3] = a.m_Cells[3] * b;
    return out;
};

/**
 * Adds two mat2's after multiplying each element of the second operand by a scalar value.
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param scale the amount to scale b's elements by before adding
 * @returns {mat2} out
 */
mat2 mat2::multiplyScalarAndAdd(mat2& out, const mat2 a, const mat2 b, const GLfloat scale)
{
    out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
    out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
    out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
    out.m_Cells[3] = a.m_Cells[3] + (b.m_Cells[3] * scale);
    return out;
};

/**
 * Returns a string representation of a mat2
 *
 * @param a to represent as a string
 * @returns {String} string representation of the mat2
 */
std::string mat2::str(const mat2& a)
{
    std::stringstream ss;
    ss << "mat2(" << a.m_Cells[0];
    for (int i=1; i<4; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {mat2} out
 */
mat2 mat2::zero(mat2& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    out.m_Cells[2] = 0.0;
    out.m_Cells[3] = 0.0;
    return out;
};
/**
 * calls glUniformMatrix2fv for the matrix a
 *
 * @param loc the location of the vector
 * @param a the vector to send to OpenGL shader
 * @returns {void}
 */
void mat2::glUniformMatrix(const GLint loc, const mat2 a)
{
    if (loc >= 0) glUniformMatrix2fv(loc, 1, GL_FALSE, (const GLfloat*)&a);
};

const mat2d mat2d::null;

/**
 * Creates a new identity mat2d
 *
 * @returns {mat2d} a new 2x3 matrix
 */
mat2d mat2d::create()
{
    mat2d out;
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 1;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 0;
    return out;
};

/**
 * Creates a new mat2d initialized with values from an existing matrix
 *
 * @param a matrix to clone
 * @returns {mat2d} a new 2x3 matrix
 */
mat2d mat2d::clone(const mat2d a)
{
    mat2d out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5];
    return out;
};

/**
 * Copy the values from one mat2d to another
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat2d} out
 */
mat2d mat2d::copy(mat2d& out, const mat2d a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5];
    return out;
};

/**
 * Set a mat2d to the identity matrix
 *
 * @param out the receiving matrix
 * @returns {mat2d} out
 */
mat2d mat2d::identity(mat2d& out)
{
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 1;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 0;
    return out;
};

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
mat2d mat2d::fromValues(const GLfloat a, const GLfloat b, const GLfloat c, const GLfloat d, const GLfloat tx, const GLfloat ty)
{
    mat2d out;
    out.m_Cells[0] = a;
    out.m_Cells[1] = b;
    out.m_Cells[2] = c;
    out.m_Cells[3] = d;
    out.m_Cells[4] = tx;
    out.m_Cells[5] = ty;
    return out;
};

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
mat2d mat2d::set(mat2d& out, const GLfloat a, const GLfloat b, const GLfloat c, const GLfloat d, const GLfloat tx, const GLfloat ty)
{
    out.m_Cells[0] = a;
    out.m_Cells[1] = b;
    out.m_Cells[2] = c;
    out.m_Cells[3] = d;
    out.m_Cells[4] = tx;
    out.m_Cells[5] = ty;
    return out;
};

/**
 * Inverts a mat2d
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat2d} out
 */
mat2d mat2d::invert(mat2d& out, const mat2d a)
{
    GLfloat aa = a.m_Cells[0], ab = a.m_Cells[1], ac = a.m_Cells[2], ad = a.m_Cells[3],
    atx = a.m_Cells[4], aty = a.m_Cells[5];

    GLfloat det = aa * ad - ab * ac;
    if(!det){
        return null;
    }
    det = 1.0 / det;

    out.m_Cells[0] = ad * det;
    out.m_Cells[1] = -ab * det;
    out.m_Cells[2] = -ac * det;
    out.m_Cells[3] = aa * det;
    out.m_Cells[4] = (ac * aty - ad * atx) * det;
    out.m_Cells[5] = (ab * atx - aa * aty) * det;
    return out;
};

/**
 * Calculates the determinant of a mat2d
 *
 * @param a the source matrix
 * @returns {Number} determinant of a
 */
GLfloat mat2d::determinant(const mat2d a)
{
    return a.m_Cells[0] * a.m_Cells[3] - a.m_Cells[1] * a.m_Cells[2];
};

/**
 * Multiplies two mat2d's
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat2d} out
 */
mat2d mat2d::multiply(mat2d& out, const mat2d a, const mat2d b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
    b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3], b4 = b.m_Cells[4], b5 = b.m_Cells[5];
    out.m_Cells[0] = a0 * b0 + a2 * b1;
    out.m_Cells[1] = a1 * b0 + a3 * b1;
    out.m_Cells[2] = a0 * b2 + a2 * b3;
    out.m_Cells[3] = a1 * b2 + a3 * b3;
    out.m_Cells[4] = a0 * b4 + a2 * b5 + a4;
    out.m_Cells[5] = a1 * b4 + a3 * b5 + a5;
    return out;
};

/**
 * Rotates a mat2d by the given angle
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param rad the angle to rotate the matrix by
 * @returns {mat2d} out
 */
mat2d mat2d::rotate(mat2d& out, const mat2d a, const GLfloat rad)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
    s = sin(rad),
    c = cos(rad);
    out.m_Cells[0] = a0 *  c + a2 * s;
    out.m_Cells[1] = a1 *  c + a3 * s;
    out.m_Cells[2] = a0 * -s + a2 * c;
    out.m_Cells[3] = a1 * -s + a3 * c;
    out.m_Cells[4] = a4;
    out.m_Cells[5] = a5;
    return out;
};

/**
 * Scales the mat2d by the dimensions in the given vec2
 *
 * @param out the receiving matrix
 * @param a the matrix to translate
 * @param v the vec2 to scale the matrix by
 * @returns {mat2d} out
 **/
mat2d mat2d::scale(mat2d& out, const mat2d a, const vec2 v)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
    v0 = v.m_Cells[0], v1 = v.m_Cells[1];
    out.m_Cells[0] = a0 * v0;
    out.m_Cells[1] = a1 * v0;
    out.m_Cells[2] = a2 * v1;
    out.m_Cells[3] = a3 * v1;
    out.m_Cells[4] = a4;
    out.m_Cells[5] = a5;
    return out;
};

/**
 * Translates the mat2d by the dimensions in the given vec2
 *
 * @param out the receiving matrix
 * @param a the matrix to translate
 * @param v the vec2 to translate the matrix by
 * @returns {mat2d} out
 **/
mat2d mat2d::translate(mat2d& out, const mat2d a, const vec2 v)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
    v0 = v.m_Cells[0], v1 = v.m_Cells[1];
    out.m_Cells[0] = a0;
    out.m_Cells[1] = a1;
    out.m_Cells[2] = a2;
    out.m_Cells[3] = a3;
    out.m_Cells[4] = a0 * v0 + a2 * v1 + a4;
    out.m_Cells[5] = a1 * v0 + a3 * v1 + a5;
    return out;
};

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
mat2d mat2d::fromRotation(mat2d& out, const GLfloat rad)
{
    GLfloat s = sin(rad), c = cos(rad);
    out.m_Cells[0] = c;
    out.m_Cells[1] = s;
    out.m_Cells[2] = -s;
    out.m_Cells[3] = c;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 0;
    return out;
}

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
mat2d mat2d::fromScaling(mat2d& out, const vec2 v)
{
    out.m_Cells[0] = v.m_Cells[0];
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = v.m_Cells[1];
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 0;
    return out;
}

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
mat2d mat2d::fromTranslation(mat2d& out, const vec2 v)
{
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 1;
    out.m_Cells[4] = v.m_Cells[0];
    out.m_Cells[5] = v.m_Cells[1];
    return out;
}

/**
 * Returns Frobenius norm of a mat2d
 *
 * @param a the matrix to calculate Frobenius norm of
 * @returns {Number} Frobenius norm
 */
GLfloat mat2d::frob(const mat2d a)
{
    return(sqrt(pow(a.m_Cells[0], 2) + pow(a.m_Cells[1], 2) + pow(a.m_Cells[2], 2) + pow(a.m_Cells[3], 2) + pow(a.m_Cells[4], 2) + pow(a.m_Cells[5], 2) + 1));
};

/**
 * Adds two mat2d's
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat2d} out
 */
mat2d mat2d::add(mat2d& out, const mat2d a, const mat2d b)
{
    out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] + b.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4] + b.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5] + b.m_Cells[5];
    return out;
};

/**
 * Subtracts matrix b from matrix a
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat2d} out
 */
mat2d mat2d::subtract(mat2d& out, const mat2d a, const mat2d b)
{
    out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] - b.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4] - b.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5] - b.m_Cells[5];
    return out;
};

/**
 * Multiply each element of the matrix by a scalar.
 *
 * @param out the receiving matrix
 * @param a the matrix to scale
 * @param b amount to scale the matrix's elements by
 * @returns {mat2d} out
 */
mat2d mat2d::multiplyScalar(mat2d& out, const mat2d a, const GLfloat b)
{
    out.m_Cells[0] = a.m_Cells[0] * b;
    out.m_Cells[1] = a.m_Cells[1] * b;
    out.m_Cells[2] = a.m_Cells[2] * b;
    out.m_Cells[3] = a.m_Cells[3] * b;
    out.m_Cells[4] = a.m_Cells[4] * b;
    out.m_Cells[5] = a.m_Cells[5] * b;
    return out;
};

/**
 * Adds two mat2d's after multiplying each element of the second operand by a scalar value.
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param scale the amount to scale b's elements by before adding
 * @returns {mat2d} out
 */
mat2d mat2d::multiplyScalarAndAdd(mat2d& out, const mat2d a, const mat2d b, const GLfloat scale)
{
    out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
    out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
    out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
    out.m_Cells[3] = a.m_Cells[3] + (b.m_Cells[3] * scale);
    out.m_Cells[4] = a.m_Cells[4] + (b.m_Cells[4] * scale);
    out.m_Cells[5] = a.m_Cells[5] + (b.m_Cells[5] * scale);
    return out;
};

/**
 * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat2d::exactEquals(const mat2d a, const mat2d b)
{
    return a.m_Cells[0] == b.m_Cells[0] && a.m_Cells[1] == b.m_Cells[1] && a.m_Cells[2] == b.m_Cells[2] && a.m_Cells[3] == b.m_Cells[3] && a.m_Cells[4] == b.m_Cells[4] && a.m_Cells[5] == b.m_Cells[5];
};

/**
 * Returns whether or not the matrices have approximately the same elements in the same position.
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat2d::equals(const mat2d a, const mat2d b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5];
    GLfloat b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3], b4 = b.m_Cells[4], b5 = b.m_Cells[5];
    return (fabs(a0 - b0) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a0), fabs(b0))) &&
    fabs(a1 - b1) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a1), fabs(b1))) &&
    fabs(a2 - b2) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a2), fabs(b2))) &&
    fabs(a3 - b3) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a3), fabs(b3))) &&
    fabs(a4 - b4) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a4), fabs(b4))) &&
    fabs(a5 - b5) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a5), fabs(b5))));
};

/**
 * Returns a string representation of a mat2d
 *
 * @param a to represent as a string
 * @returns {String} string representation of the mat2d
 */
std::string mat2d::str(const mat2d& a)
{
    std::stringstream ss;
    ss << "mat2d(" << a.m_Cells[0];
    for (int i=1; i<6; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {mat2d} out
 */
mat2d mat2d::zero(mat2d& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    out.m_Cells[2] = 0.0;
    out.m_Cells[3] = 0.0;
    out.m_Cells[4] = 0.0;
    out.m_Cells[5] = 0.0;
    return out;
};

const mat3 mat3::null;

/**
 * Creates a new identity mat3
 *
 * @returns {mat3} a new 3x3 matrix
 */
mat3 mat3::create()
{
    mat3 out;
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 1;
    out.m_Cells[5] = 0;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 1;
    return out;
};

/**
 * Copies the upper-left 3x3 values into the given mat3.
 *
 * @param out the receiving 3x3 matrix
 * @param a   the source 4x4 matrix
 * @returns {mat3} out
 */
mat3 mat3::fromMat4(mat3& out, const mat4 a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[4];
    out.m_Cells[4] = a.m_Cells[5];
    out.m_Cells[5] = a.m_Cells[6];
    out.m_Cells[6] = a.m_Cells[8];
    out.m_Cells[7] = a.m_Cells[9];
    out.m_Cells[8] = a.m_Cells[10];
    return out;
};

/**
 * Creates a new mat3 initialized with values from an existing matrix
 *
 * @param a matrix to clone
 * @returns {mat3} a new 3x3 matrix
 */
mat3 mat3::clone(const mat3 a)
{
    mat3 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8];
    return out;
};

/**
 * Copy the values from one mat3 to another
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat3} out
 */
mat3 mat3::copy(mat3& out, const mat3 a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8];
    return out;
};

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
mat3 mat3::fromValues(const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m20, const GLfloat m21, const GLfloat m22)
{
    mat3 out;
    out.m_Cells[0] = m00;
    out.m_Cells[1] = m01;
    out.m_Cells[2] = m02;
    out.m_Cells[3] = m10;
    out.m_Cells[4] = m11;
    out.m_Cells[5] = m12;
    out.m_Cells[6] = m20;
    out.m_Cells[7] = m21;
    out.m_Cells[8] = m22;
    return out;
};

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
mat3 mat3::set(mat3& out, const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m20, const GLfloat m21, const GLfloat m22)
{
    out.m_Cells[0] = m00;
    out.m_Cells[1] = m01;
    out.m_Cells[2] = m02;
    out.m_Cells[3] = m10;
    out.m_Cells[4] = m11;
    out.m_Cells[5] = m12;
    out.m_Cells[6] = m20;
    out.m_Cells[7] = m21;
    out.m_Cells[8] = m22;
    return out;
};

/**
 * Set a mat3 to the identity matrix
 *
 * @param out the receiving matrix
 * @returns {mat3} out
 */
mat3 mat3::identity(mat3& out)
{
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 1;
    out.m_Cells[5] = 0;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 1;
    return out;
};

/**
 * Transpose the values of a mat3
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat3} out
 */
mat3 mat3::transpose(mat3& out, const mat3 a)
{
    // If we are transposing ourselves we can skip a few steps but have to cache some values
    if (out == a) {
        GLfloat a01 = a.m_Cells[1], a02 = a.m_Cells[2], a12 = a.m_Cells[5];
        out.m_Cells[1] = a.m_Cells[3];
        out.m_Cells[2] = a.m_Cells[6];
        out.m_Cells[3] = a01;
        out.m_Cells[5] = a.m_Cells[7];
        out.m_Cells[6] = a02;
        out.m_Cells[7] = a12;
    } else {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[3];
        out.m_Cells[2] = a.m_Cells[6];
        out.m_Cells[3] = a.m_Cells[1];
        out.m_Cells[4] = a.m_Cells[4];
        out.m_Cells[5] = a.m_Cells[7];
        out.m_Cells[6] = a.m_Cells[2];
        out.m_Cells[7] = a.m_Cells[5];
        out.m_Cells[8] = a.m_Cells[8];
    }

    return out;
};

/**
 * Inverts a mat3
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat3} out
 */
mat3 mat3::invert(mat3& out, const mat3 a)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
    a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
    a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8],

    b01 = a22 * a11 - a12 * a21,
    b11 = -a22 * a10 + a12 * a20,
    b21 = a21 * a10 - a11 * a20,

    // Calculate the determinant
    det = a00 * b01 + a01 * b11 + a02 * b21;

    if (!det) {
        return null;
    }
    det = 1.0 / det;

    out.m_Cells[0] = b01 * det;
    out.m_Cells[1] = (-a22 * a01 + a02 * a21) * det;
    out.m_Cells[2] = (a12 * a01 - a02 * a11) * det;
    out.m_Cells[3] = b11 * det;
    out.m_Cells[4] = (a22 * a00 - a02 * a20) * det;
    out.m_Cells[5] = (-a12 * a00 + a02 * a10) * det;
    out.m_Cells[6] = b21 * det;
    out.m_Cells[7] = (-a21 * a00 + a01 * a20) * det;
    out.m_Cells[8] = (a11 * a00 - a01 * a10) * det;
    return out;
};

/**
 * Calculates the adjugate of a mat3
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat3} out
 */
mat3 mat3::adjoint(mat3& out, const mat3 a)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
    a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
    a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8];

    out.m_Cells[0] = (a11 * a22 - a12 * a21);
    out.m_Cells[1] = (a02 * a21 - a01 * a22);
    out.m_Cells[2] = (a01 * a12 - a02 * a11);
    out.m_Cells[3] = (a12 * a20 - a10 * a22);
    out.m_Cells[4] = (a00 * a22 - a02 * a20);
    out.m_Cells[5] = (a02 * a10 - a00 * a12);
    out.m_Cells[6] = (a10 * a21 - a11 * a20);
    out.m_Cells[7] = (a01 * a20 - a00 * a21);
    out.m_Cells[8] = (a00 * a11 - a01 * a10);
    return out;
};

/**
 * Calculates the determinant of a mat3
 *
 * @param a the source matrix
 * @returns {Number} determinant of a
 */
GLfloat mat3::determinant(const mat3 a)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
    a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
    a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8];

    return a00 * (a22 * a11 - a12 * a21) + a01 * (-a22 * a10 + a12 * a20) + a02 * (a21 * a10 - a11 * a20);
};

/**
 * Multiplies two mat3's
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat3} out
 */
mat3 mat3::multiply(mat3& out, const mat3 a, const mat3 b)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
    a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
    a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8],

    b00 = b.m_Cells[0], b01 = b.m_Cells[1], b02 = b.m_Cells[2],
    b10 = b.m_Cells[3], b11 = b.m_Cells[4], b12 = b.m_Cells[5],
    b20 = b.m_Cells[6], b21 = b.m_Cells[7], b22 = b.m_Cells[8];

    out.m_Cells[0] = b00 * a00 + b01 * a10 + b02 * a20;
    out.m_Cells[1] = b00 * a01 + b01 * a11 + b02 * a21;
    out.m_Cells[2] = b00 * a02 + b01 * a12 + b02 * a22;

    out.m_Cells[3] = b10 * a00 + b11 * a10 + b12 * a20;
    out.m_Cells[4] = b10 * a01 + b11 * a11 + b12 * a21;
    out.m_Cells[5] = b10 * a02 + b11 * a12 + b12 * a22;

    out.m_Cells[6] = b20 * a00 + b21 * a10 + b22 * a20;
    out.m_Cells[7] = b20 * a01 + b21 * a11 + b22 * a21;
    out.m_Cells[8] = b20 * a02 + b21 * a12 + b22 * a22;
    return out;
};

/**
 * Translate a mat3 by the given vector
 *
 * @param out the receiving matrix
 * @param a the matrix to translate
 * @param v vector to translate by
 * @returns {mat3} out
 */
mat3 mat3::translate(mat3& out, const mat3 a, const vec2 v)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
    a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
    a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8],
    x = v.m_Cells[0], y = v.m_Cells[1];

    out.m_Cells[0] = a00;
    out.m_Cells[1] = a01;
    out.m_Cells[2] = a02;

    out.m_Cells[3] = a10;
    out.m_Cells[4] = a11;
    out.m_Cells[5] = a12;

    out.m_Cells[6] = x * a00 + y * a10 + a20;
    out.m_Cells[7] = x * a01 + y * a11 + a21;
    out.m_Cells[8] = x * a02 + y * a12 + a22;
    return out;
};

/**
 * Rotates a mat3 by the given angle
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param rad the angle to rotate the matrix by
 * @returns {mat3} out
 */
mat3 mat3::rotate(mat3& out, const mat3 a, const GLfloat rad)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
    a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
    a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8],

    s = sin(rad),
    c = cos(rad);

    out.m_Cells[0] = c * a00 + s * a10;
    out.m_Cells[1] = c * a01 + s * a11;
    out.m_Cells[2] = c * a02 + s * a12;

    out.m_Cells[3] = c * a10 - s * a00;
    out.m_Cells[4] = c * a11 - s * a01;
    out.m_Cells[5] = c * a12 - s * a02;

    out.m_Cells[6] = a20;
    out.m_Cells[7] = a21;
    out.m_Cells[8] = a22;
    return out;
};

/**
 * Scales the mat3 by the dimensions in the given vec2
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param v the vec2 to scale the matrix by
 * @returns {mat3} out
 **/
mat3 mat3::scale(mat3& out, const mat3 a, const vec2 v)
{
    GLfloat x = v.m_Cells[0], y = v.m_Cells[1];

    out.m_Cells[0] = x * a.m_Cells[0];
    out.m_Cells[1] = x * a.m_Cells[1];
    out.m_Cells[2] = x * a.m_Cells[2];

    out.m_Cells[3] = y * a.m_Cells[3];
    out.m_Cells[4] = y * a.m_Cells[4];
    out.m_Cells[5] = y * a.m_Cells[5];

    out.m_Cells[6] = a.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8];
    return out;
};

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
mat3 mat3::fromTranslation(mat3& out, const vec2 v)
{
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 1;
    out.m_Cells[5] = 0;
    out.m_Cells[6] = v.m_Cells[0];
    out.m_Cells[7] = v.m_Cells[1];
    out.m_Cells[8] = 1;
    return out;
}

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
mat3 mat3::fromRotation(mat3& out, const GLfloat rad)
{
    GLfloat s = sin(rad), c = cos(rad);

    out.m_Cells[0] = c;
    out.m_Cells[1] = s;
    out.m_Cells[2] = 0;

    out.m_Cells[3] = -s;
    out.m_Cells[4] = c;
    out.m_Cells[5] = 0;

    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 1;
    return out;
}

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
mat3 mat3::fromScaling(mat3& out, const vec2 v)
{
    out.m_Cells[0] = v.m_Cells[0];
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;

    out.m_Cells[3] = 0;
    out.m_Cells[4] = v.m_Cells[1];
    out.m_Cells[5] = 0;

    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 1;
    return out;
}

/**
 * Copies the values from a mat2d into a mat3
 *
 * @param out the receiving matrix
 * @param a the matrix to copy
 * @returns {mat3} out
 **/
mat3 mat3::fromMat2d(mat3& out, const mat2d a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = 0;

    out.m_Cells[3] = a.m_Cells[2];
    out.m_Cells[4] = a.m_Cells[3];
    out.m_Cells[5] = 0;

    out.m_Cells[6] = a.m_Cells[4];
    out.m_Cells[7] = a.m_Cells[5];
    out.m_Cells[8] = 1;
    return out;
};

/**
* Calculates a 3x3 matrix from the given quaternion
*
* @param out mat3 receiving operation result
* @param q Quaternion to create matrix from
*
* @returns {mat3} out
*/
mat3 mat3::fromQuat(mat3& out, const quat q)
{
    GLfloat x = q.m_Cells[0], y = q.m_Cells[1], z = q.m_Cells[2], w = q.m_Cells[3],
    x2 = x + x,
    y2 = y + y,
    z2 = z + z,

    xx = x * x2,
    yx = y * x2,
    yy = y * y2,
    zx = z * x2,
    zy = z * y2,
    zz = z * z2,
    wx = w * x2,
    wy = w * y2,
    wz = w * z2;

    out.m_Cells[0] = 1 - yy - zz;
    out.m_Cells[3] = yx - wz;
    out.m_Cells[6] = zx + wy;

    out.m_Cells[1] = yx + wz;
    out.m_Cells[4] = 1 - xx - zz;
    out.m_Cells[7] = zy - wx;

    out.m_Cells[2] = zx - wy;
    out.m_Cells[5] = zy + wx;
    out.m_Cells[8] = 1 - xx - yy;

    return out;
};

/**
* Calculates a 3x3 normal matrix (transpose inverse) from the 4x4 matrix
*
* @param out mat3 receiving operation result
* @param a Mat4 to derive the normal matrix from
*
* @returns {mat3} out
*/
mat3 mat3::normalFromMat4(mat3& out, const mat4 a)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
    a10 = a.m_Cells[4], a11 = a.m_Cells[5], a12 = a.m_Cells[6], a13 = a.m_Cells[7],
    a20 = a.m_Cells[8], a21 = a.m_Cells[9], a22 = a.m_Cells[10], a23 = a.m_Cells[11],
    a30 = a.m_Cells[12], a31 = a.m_Cells[13], a32 = a.m_Cells[14], a33 = a.m_Cells[15],

    b00 = a00 * a11 - a01 * a10,
    b01 = a00 * a12 - a02 * a10,
    b02 = a00 * a13 - a03 * a10,
    b03 = a01 * a12 - a02 * a11,
    b04 = a01 * a13 - a03 * a11,
    b05 = a02 * a13 - a03 * a12,
    b06 = a20 * a31 - a21 * a30,
    b07 = a20 * a32 - a22 * a30,
    b08 = a20 * a33 - a23 * a30,
    b09 = a21 * a32 - a22 * a31,
    b10 = a21 * a33 - a23 * a31,
    b11 = a22 * a33 - a23 * a32,

    // Calculate the determinant
    det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

    if (!det) {
        return null;
    }
    det = 1.0 / det;

    out.m_Cells[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
    out.m_Cells[1] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
    out.m_Cells[2] = (a10 * b10 - a11 * b08 + a13 * b06) * det;

    out.m_Cells[3] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
    out.m_Cells[4] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
    out.m_Cells[5] = (a01 * b08 - a00 * b10 - a03 * b06) * det;

    out.m_Cells[6] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
    out.m_Cells[7] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
    out.m_Cells[8] = (a30 * b04 - a31 * b02 + a33 * b00) * det;

    return out;
};

/**
 * Returns Frobenius norm of a mat3
 *
 * @param a the matrix to calculate Frobenius norm of
 * @returns {Number} Frobenius norm
 */
GLfloat mat3::frob(const mat3 a)
{
    return(sqrt(pow(a.m_Cells[0], 2) + pow(a.m_Cells[1], 2) + pow(a.m_Cells[2], 2) + pow(a.m_Cells[3], 2) + pow(a.m_Cells[4], 2) + pow(a.m_Cells[5], 2) + pow(a.m_Cells[6], 2) + pow(a.m_Cells[7], 2) + pow(a.m_Cells[8], 2)));
};

/**
 * Adds two mat3's
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat3} out
 */
mat3 mat3::add(mat3& out, const mat3 a, const mat3 b)
{
    out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] + b.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4] + b.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5] + b.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6] + b.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7] + b.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8] + b.m_Cells[8];
    return out;
};

/**
 * Subtracts matrix b from matrix a
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat3} out
 */
mat3 mat3::subtract(mat3& out, const mat3 a, const mat3 b)
{
    out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] - b.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4] - b.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5] - b.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6] - b.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7] - b.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8] - b.m_Cells[8];
    return out;
};

/**
 * Multiply each element of the matrix by a scalar.
 *
 * @param out the receiving matrix
 * @param a the matrix to scale
 * @param b amount to scale the matrix's elements by
 * @returns {mat3} out
 */
mat3 mat3::multiplyScalar(mat3& out, const mat3 a, const GLfloat b)
{
    out.m_Cells[0] = a.m_Cells[0] * b;
    out.m_Cells[1] = a.m_Cells[1] * b;
    out.m_Cells[2] = a.m_Cells[2] * b;
    out.m_Cells[3] = a.m_Cells[3] * b;
    out.m_Cells[4] = a.m_Cells[4] * b;
    out.m_Cells[5] = a.m_Cells[5] * b;
    out.m_Cells[6] = a.m_Cells[6] * b;
    out.m_Cells[7] = a.m_Cells[7] * b;
    out.m_Cells[8] = a.m_Cells[8] * b;
    return out;
};

/**
 * Adds two mat3's after multiplying each element of the second operand by a scalar value.
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param scale the amount to scale b's elements by before adding
 * @returns {mat3} out
 */
mat3 mat3::multiplyScalarAndAdd(mat3& out, const mat3 a, const mat3 b, const GLfloat scale)
{
    out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
    out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
    out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
    out.m_Cells[3] = a.m_Cells[3] + (b.m_Cells[3] * scale);
    out.m_Cells[4] = a.m_Cells[4] + (b.m_Cells[4] * scale);
    out.m_Cells[5] = a.m_Cells[5] + (b.m_Cells[5] * scale);
    out.m_Cells[6] = a.m_Cells[6] + (b.m_Cells[6] * scale);
    out.m_Cells[7] = a.m_Cells[7] + (b.m_Cells[7] * scale);
    out.m_Cells[8] = a.m_Cells[8] + (b.m_Cells[8] * scale);
    return out;
};

/**
 * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat3::exactEquals(const mat3 a, const mat3 b)
{
    return a.m_Cells[0] == b.m_Cells[0] && a.m_Cells[1] == b.m_Cells[1] && a.m_Cells[2] == b.m_Cells[2] &&
    a.m_Cells[3] == b.m_Cells[3] && a.m_Cells[4] == b.m_Cells[4] && a.m_Cells[5] == b.m_Cells[5] &&
    a.m_Cells[6] == b.m_Cells[6] && a.m_Cells[7] == b.m_Cells[7] && a.m_Cells[8] == b.m_Cells[8];
};

/**
 * Returns whether or not the matrices have approximately the same elements in the same position.
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat3::equals(const mat3 a, const mat3 b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5], a6 = a.m_Cells[6], a7 = a.m_Cells[7], a8 = a.m_Cells[8];
    GLfloat b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3], b4 = b.m_Cells[4], b5 = b.m_Cells[5], b6 = b.m_Cells[6], b7 = b.m_Cells[7], b8 = b.m_Cells[8];
    return (fabs(a0 - b0) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a0), fabs(b0))) &&
    fabs(a1 - b1) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a1), fabs(b1))) &&
    fabs(a2 - b2) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a2), fabs(b2))) &&
    fabs(a3 - b3) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a3), fabs(b3))) &&
    fabs(a4 - b4) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a4), fabs(b4))) &&
    fabs(a5 - b5) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a5), fabs(b5))) &&
    fabs(a6 - b6) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a6), fabs(b6))) &&
    fabs(a7 - b7) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a7), fabs(b7))) &&
    fabs(a8 - b8) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a8), fabs(b8))));
};


/**
 * Returns a string representation of a mat3
 *
 * @param a to represent as a string
 * @returns {String} string representation of the mat3
 */
std::string mat3::str(const mat3& a)
{
    std::stringstream ss;
    ss << "mat3(" << a.m_Cells[0];
    for (int i=1; i<9; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {mat3} out
 */
mat3 mat3::zero(mat3& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    out.m_Cells[2] = 0.0;
    out.m_Cells[3] = 0.0;
    out.m_Cells[4] = 0.0;
    out.m_Cells[5] = 0.0;
    out.m_Cells[6] = 0.0;
    out.m_Cells[7] = 0.0;
    out.m_Cells[8] = 0.0;
    return out;
};
/**
 * calls glUniformMatrix3fv for the matrix a
 *
 * @param loc the location of the vector
 * @param a the vector to send to OpenGL shader
 * @returns {void}
 */
void mat3::glUniformMatrix(const GLint loc, const mat3 a)
{
    if (loc >= 0) glUniformMatrix3fv(loc, 1, GL_FALSE, (const GLfloat*)&a);
};

const mat4 mat4::null;

/**
 * Creates a new identity mat4
 *
 * @returns {mat4} a new 4x4 matrix
 */
mat4 mat4::create()
{
    mat4 out;
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 1;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = 1;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;
    return out;
};

/**
 * Creates a new mat4 initialized with values from an existing matrix
 *
 * @param a matrix to clone
 * @returns {mat4} a new 4x4 matrix
 */
mat4 mat4::clone(const mat4 a)
{
    mat4 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8];
    out.m_Cells[9] = a.m_Cells[9];
    out.m_Cells[10] = a.m_Cells[10];
    out.m_Cells[11] = a.m_Cells[11];
    out.m_Cells[12] = a.m_Cells[12];
    out.m_Cells[13] = a.m_Cells[13];
    out.m_Cells[14] = a.m_Cells[14];
    out.m_Cells[15] = a.m_Cells[15];
    return out;
};

/**
 * Copy the values from one mat4 to another
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat4} out
 */
mat4 mat4::copy(mat4& out, const mat4 a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8];
    out.m_Cells[9] = a.m_Cells[9];
    out.m_Cells[10] = a.m_Cells[10];
    out.m_Cells[11] = a.m_Cells[11];
    out.m_Cells[12] = a.m_Cells[12];
    out.m_Cells[13] = a.m_Cells[13];
    out.m_Cells[14] = a.m_Cells[14];
    out.m_Cells[15] = a.m_Cells[15];
    return out;
};

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
mat4 mat4::fromValues(const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m03, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m13, const GLfloat m20, const GLfloat m21, const GLfloat m22, const GLfloat m23, const GLfloat m30, const GLfloat m31, const GLfloat m32, const GLfloat m33)
{
    mat4 out;
    out.m_Cells[0] = m00;
    out.m_Cells[1] = m01;
    out.m_Cells[2] = m02;
    out.m_Cells[3] = m03;
    out.m_Cells[4] = m10;
    out.m_Cells[5] = m11;
    out.m_Cells[6] = m12;
    out.m_Cells[7] = m13;
    out.m_Cells[8] = m20;
    out.m_Cells[9] = m21;
    out.m_Cells[10] = m22;
    out.m_Cells[11] = m23;
    out.m_Cells[12] = m30;
    out.m_Cells[13] = m31;
    out.m_Cells[14] = m32;
    out.m_Cells[15] = m33;
    return out;
};

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
mat4 mat4::set(mat4& out, const GLfloat m00, const GLfloat m01, const GLfloat m02, const GLfloat m03, const GLfloat m10, const GLfloat m11, const GLfloat m12, const GLfloat m13, const GLfloat m20, const GLfloat m21, const GLfloat m22, const GLfloat m23, const GLfloat m30, const GLfloat m31, const GLfloat m32, const GLfloat m33)
{
    out.m_Cells[0] = m00;
    out.m_Cells[1] = m01;
    out.m_Cells[2] = m02;
    out.m_Cells[3] = m03;
    out.m_Cells[4] = m10;
    out.m_Cells[5] = m11;
    out.m_Cells[6] = m12;
    out.m_Cells[7] = m13;
    out.m_Cells[8] = m20;
    out.m_Cells[9] = m21;
    out.m_Cells[10] = m22;
    out.m_Cells[11] = m23;
    out.m_Cells[12] = m30;
    out.m_Cells[13] = m31;
    out.m_Cells[14] = m32;
    out.m_Cells[15] = m33;
    return out;
};


/**
 * Set a mat4 to the identity matrix
 *
 * @param out the receiving matrix
 * @returns {mat4} out
 */
mat4 mat4::identity(mat4& out)
{
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 1;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = 1;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;
    return out;
};

/**
 * Transpose the values of a mat4 not using SIMD
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat4} out
 */
mat4 mat4::transpose(mat4& out, const mat4 a)
{
    // If we are transposing ourselves we can skip a few steps but have to cache some values
    if (out == a) {
        GLfloat a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
        a12 = a.m_Cells[6], a13 = a.m_Cells[7],
        a23 = a.m_Cells[11];

        out.m_Cells[1] = a.m_Cells[4];
        out.m_Cells[2] = a.m_Cells[8];
        out.m_Cells[3] = a.m_Cells[12];
        out.m_Cells[4] = a01;
        out.m_Cells[6] = a.m_Cells[9];
        out.m_Cells[7] = a.m_Cells[13];
        out.m_Cells[8] = a02;
        out.m_Cells[9] = a12;
        out.m_Cells[11] = a.m_Cells[14];
        out.m_Cells[12] = a03;
        out.m_Cells[13] = a13;
        out.m_Cells[14] = a23;
    } else {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[4];
        out.m_Cells[2] = a.m_Cells[8];
        out.m_Cells[3] = a.m_Cells[12];
        out.m_Cells[4] = a.m_Cells[1];
        out.m_Cells[5] = a.m_Cells[5];
        out.m_Cells[6] = a.m_Cells[9];
        out.m_Cells[7] = a.m_Cells[13];
        out.m_Cells[8] = a.m_Cells[2];
        out.m_Cells[9] = a.m_Cells[6];
        out.m_Cells[10] = a.m_Cells[10];
        out.m_Cells[11] = a.m_Cells[14];
        out.m_Cells[12] = a.m_Cells[3];
        out.m_Cells[13] = a.m_Cells[7];
        out.m_Cells[14] = a.m_Cells[11];
        out.m_Cells[15] = a.m_Cells[15];
    }

    return out;
};

/**
 * Inverts a mat4 not using SIMD
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat4} out
 */
mat4 mat4::invert(mat4& out, const mat4 a)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
    a10 = a.m_Cells[4], a11 = a.m_Cells[5], a12 = a.m_Cells[6], a13 = a.m_Cells[7],
    a20 = a.m_Cells[8], a21 = a.m_Cells[9], a22 = a.m_Cells[10], a23 = a.m_Cells[11],
    a30 = a.m_Cells[12], a31 = a.m_Cells[13], a32 = a.m_Cells[14], a33 = a.m_Cells[15],

    b00 = a00 * a11 - a01 * a10,
    b01 = a00 * a12 - a02 * a10,
    b02 = a00 * a13 - a03 * a10,
    b03 = a01 * a12 - a02 * a11,
    b04 = a01 * a13 - a03 * a11,
    b05 = a02 * a13 - a03 * a12,
    b06 = a20 * a31 - a21 * a30,
    b07 = a20 * a32 - a22 * a30,
    b08 = a20 * a33 - a23 * a30,
    b09 = a21 * a32 - a22 * a31,
    b10 = a21 * a33 - a23 * a31,
    b11 = a22 * a33 - a23 * a32,

    // Calculate the determinant
    det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

    if (!det) {
        return null;
    }
    det = 1.0 / det;

    out.m_Cells[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
    out.m_Cells[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
    out.m_Cells[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
    out.m_Cells[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
    out.m_Cells[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
    out.m_Cells[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
    out.m_Cells[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
    out.m_Cells[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
    out.m_Cells[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
    out.m_Cells[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
    out.m_Cells[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
    out.m_Cells[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
    out.m_Cells[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
    out.m_Cells[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
    out.m_Cells[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
    out.m_Cells[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;

    return out;
};

/**
 * Calculates the adjugate of a mat4 not using SIMD
 *
 * @param out the receiving matrix
 * @param a the source matrix
 * @returns {mat4} out
 */
mat4 mat4::adjoint(mat4& out, const mat4 a)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
    a10 = a.m_Cells[4], a11 = a.m_Cells[5], a12 = a.m_Cells[6], a13 = a.m_Cells[7],
    a20 = a.m_Cells[8], a21 = a.m_Cells[9], a22 = a.m_Cells[10], a23 = a.m_Cells[11],
    a30 = a.m_Cells[12], a31 = a.m_Cells[13], a32 = a.m_Cells[14], a33 = a.m_Cells[15];

    out.m_Cells[0]  =  (a11 * (a22 * a33 - a23 * a32) - a21 * (a12 * a33 - a13 * a32) + a31 * (a12 * a23 - a13 * a22));
    out.m_Cells[1]  = -(a01 * (a22 * a33 - a23 * a32) - a21 * (a02 * a33 - a03 * a32) + a31 * (a02 * a23 - a03 * a22));
    out.m_Cells[2]  =  (a01 * (a12 * a33 - a13 * a32) - a11 * (a02 * a33 - a03 * a32) + a31 * (a02 * a13 - a03 * a12));
    out.m_Cells[3]  = -(a01 * (a12 * a23 - a13 * a22) - a11 * (a02 * a23 - a03 * a22) + a21 * (a02 * a13 - a03 * a12));
    out.m_Cells[4]  = -(a10 * (a22 * a33 - a23 * a32) - a20 * (a12 * a33 - a13 * a32) + a30 * (a12 * a23 - a13 * a22));
    out.m_Cells[5]  =  (a00 * (a22 * a33 - a23 * a32) - a20 * (a02 * a33 - a03 * a32) + a30 * (a02 * a23 - a03 * a22));
    out.m_Cells[6]  = -(a00 * (a12 * a33 - a13 * a32) - a10 * (a02 * a33 - a03 * a32) + a30 * (a02 * a13 - a03 * a12));
    out.m_Cells[7]  =  (a00 * (a12 * a23 - a13 * a22) - a10 * (a02 * a23 - a03 * a22) + a20 * (a02 * a13 - a03 * a12));
    out.m_Cells[8]  =  (a10 * (a21 * a33 - a23 * a31) - a20 * (a11 * a33 - a13 * a31) + a30 * (a11 * a23 - a13 * a21));
    out.m_Cells[9]  = -(a00 * (a21 * a33 - a23 * a31) - a20 * (a01 * a33 - a03 * a31) + a30 * (a01 * a23 - a03 * a21));
    out.m_Cells[10] =  (a00 * (a11 * a33 - a13 * a31) - a10 * (a01 * a33 - a03 * a31) + a30 * (a01 * a13 - a03 * a11));
    out.m_Cells[11] = -(a00 * (a11 * a23 - a13 * a21) - a10 * (a01 * a23 - a03 * a21) + a20 * (a01 * a13 - a03 * a11));
    out.m_Cells[12] = -(a10 * (a21 * a32 - a22 * a31) - a20 * (a11 * a32 - a12 * a31) + a30 * (a11 * a22 - a12 * a21));
    out.m_Cells[13] =  (a00 * (a21 * a32 - a22 * a31) - a20 * (a01 * a32 - a02 * a31) + a30 * (a01 * a22 - a02 * a21));
    out.m_Cells[14] = -(a00 * (a11 * a32 - a12 * a31) - a10 * (a01 * a32 - a02 * a31) + a30 * (a01 * a12 - a02 * a11));
    out.m_Cells[15] =  (a00 * (a11 * a22 - a12 * a21) - a10 * (a01 * a22 - a02 * a21) + a20 * (a01 * a12 - a02 * a11));
    return out;
};

/**
 * Calculates the determinant of a mat4
 *
 * @param a the source matrix
 * @returns {Number} determinant of a
 */
GLfloat mat4::determinant(const mat4 a)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
    a10 = a.m_Cells[4], a11 = a.m_Cells[5], a12 = a.m_Cells[6], a13 = a.m_Cells[7],
    a20 = a.m_Cells[8], a21 = a.m_Cells[9], a22 = a.m_Cells[10], a23 = a.m_Cells[11],
    a30 = a.m_Cells[12], a31 = a.m_Cells[13], a32 = a.m_Cells[14], a33 = a.m_Cells[15],

    b00 = a00 * a11 - a01 * a10,
    b01 = a00 * a12 - a02 * a10,
    b02 = a00 * a13 - a03 * a10,
    b03 = a01 * a12 - a02 * a11,
    b04 = a01 * a13 - a03 * a11,
    b05 = a02 * a13 - a03 * a12,
    b06 = a20 * a31 - a21 * a30,
    b07 = a20 * a32 - a22 * a30,
    b08 = a20 * a33 - a23 * a30,
    b09 = a21 * a32 - a22 * a31,
    b10 = a21 * a33 - a23 * a31,
    b11 = a22 * a33 - a23 * a32;

    // Calculate the determinant
    return b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
};

/**
 * Multiplies two mat4's explicitly not using SIMD
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat4} out
 */
mat4 mat4::multiply(mat4& out, const mat4 a, const mat4 b)
{
    GLfloat a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
    a10 = a.m_Cells[4], a11 = a.m_Cells[5], a12 = a.m_Cells[6], a13 = a.m_Cells[7],
    a20 = a.m_Cells[8], a21 = a.m_Cells[9], a22 = a.m_Cells[10], a23 = a.m_Cells[11],
    a30 = a.m_Cells[12], a31 = a.m_Cells[13], a32 = a.m_Cells[14], a33 = a.m_Cells[15];

    // Cache only the current line of the second matrix
    GLfloat b0  = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3];
    out.m_Cells[0] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out.m_Cells[1] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out.m_Cells[2] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out.m_Cells[3] = b0*a03 + b1*a13 + b2*a23 + b3*a33;

    b0 = b.m_Cells[4]; b1 = b.m_Cells[5]; b2 = b.m_Cells[6]; b3 = b.m_Cells[7];
    out.m_Cells[4] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out.m_Cells[5] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out.m_Cells[6] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out.m_Cells[7] = b0*a03 + b1*a13 + b2*a23 + b3*a33;

    b0 = b.m_Cells[8]; b1 = b.m_Cells[9]; b2 = b.m_Cells[10]; b3 = b.m_Cells[11];
    out.m_Cells[8] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out.m_Cells[9] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out.m_Cells[10] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out.m_Cells[11] = b0*a03 + b1*a13 + b2*a23 + b3*a33;

    b0 = b.m_Cells[12]; b1 = b.m_Cells[13]; b2 = b.m_Cells[14]; b3 = b.m_Cells[15];
    out.m_Cells[12] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out.m_Cells[13] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out.m_Cells[14] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out.m_Cells[15] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
    return out;
};

/**
 * Translate a mat4 by the given vector not using SIMD
 *
 * @param out the receiving matrix
 * @param a the matrix to translate
 * @param v vector to translate by
 * @returns {mat4} out
 */
mat4 mat4::translate(mat4& out, const mat4 a, const vec3 v)
{
    GLfloat x = v.m_Cells[0], y = v.m_Cells[1], z = v.m_Cells[2],
    a00, a01, a02, a03,
    a10, a11, a12, a13,
    a20, a21, a22, a23;

    if (a == out) {
        out.m_Cells[12] = a.m_Cells[0] * x + a.m_Cells[4] * y + a.m_Cells[8] * z + a.m_Cells[12];
        out.m_Cells[13] = a.m_Cells[1] * x + a.m_Cells[5] * y + a.m_Cells[9] * z + a.m_Cells[13];
        out.m_Cells[14] = a.m_Cells[2] * x + a.m_Cells[6] * y + a.m_Cells[10] * z + a.m_Cells[14];
        out.m_Cells[15] = a.m_Cells[3] * x + a.m_Cells[7] * y + a.m_Cells[11] * z + a.m_Cells[15];
    } else {
        a00 = a.m_Cells[0]; a01 = a.m_Cells[1]; a02 = a.m_Cells[2]; a03 = a.m_Cells[3];
        a10 = a.m_Cells[4]; a11 = a.m_Cells[5]; a12 = a.m_Cells[6]; a13 = a.m_Cells[7];
        a20 = a.m_Cells[8]; a21 = a.m_Cells[9]; a22 = a.m_Cells[10]; a23 = a.m_Cells[11];

        out.m_Cells[0] = a00; out.m_Cells[1] = a01; out.m_Cells[2] = a02; out.m_Cells[3] = a03;
        out.m_Cells[4] = a10; out.m_Cells[5] = a11; out.m_Cells[6] = a12; out.m_Cells[7] = a13;
        out.m_Cells[8] = a20; out.m_Cells[9] = a21; out.m_Cells[10] = a22; out.m_Cells[11] = a23;

        out.m_Cells[12] = a00 * x + a10 * y + a20 * z + a.m_Cells[12];
        out.m_Cells[13] = a01 * x + a11 * y + a21 * z + a.m_Cells[13];
        out.m_Cells[14] = a02 * x + a12 * y + a22 * z + a.m_Cells[14];
        out.m_Cells[15] = a03 * x + a13 * y + a23 * z + a.m_Cells[15];
    }

    return out;
};

/**
 * Scales the mat4 by the dimensions in the given vec3 not using vectorization
 *
 * @param out the receiving matrix
 * @param a the matrix to scale
 * @param v the vec3 to scale the matrix by
 * @returns {mat4} out
 **/
mat4 mat4::scale(mat4& out, const mat4 a, const vec3 v)
{
    GLfloat x = v.m_Cells[0], y = v.m_Cells[1], z = v.m_Cells[2];

    out.m_Cells[0] = a.m_Cells[0] * x;
    out.m_Cells[1] = a.m_Cells[1] * x;
    out.m_Cells[2] = a.m_Cells[2] * x;
    out.m_Cells[3] = a.m_Cells[3] * x;
    out.m_Cells[4] = a.m_Cells[4] * y;
    out.m_Cells[5] = a.m_Cells[5] * y;
    out.m_Cells[6] = a.m_Cells[6] * y;
    out.m_Cells[7] = a.m_Cells[7] * y;
    out.m_Cells[8] = a.m_Cells[8] * z;
    out.m_Cells[9] = a.m_Cells[9] * z;
    out.m_Cells[10] = a.m_Cells[10] * z;
    out.m_Cells[11] = a.m_Cells[11] * z;
    out.m_Cells[12] = a.m_Cells[12];
    out.m_Cells[13] = a.m_Cells[13];
    out.m_Cells[14] = a.m_Cells[14];
    out.m_Cells[15] = a.m_Cells[15];
    return out;
};

/**
 * Rotates a mat4 by the given angle around the given axis
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param rad the angle to rotate the matrix by
 * @param axis the axis to rotate around
 * @returns {mat4} out
 */
mat4 mat4::rotate(mat4& out, const mat4 a, const GLfloat rad, const vec3 axis)
{
    GLfloat x = axis.m_Cells[0], y = axis.m_Cells[1], z = axis.m_Cells[2],
    len = sqrt(x * x + y * y + z * z),
    s, c, t,
    a00, a01, a02, a03,
    a10, a11, a12, a13,
    a20, a21, a22, a23,
    b00, b01, b02,
    b10, b11, b12,
    b20, b21, b22;

if (fabs(len) < GLMAT_EPSILON) { return null; }

    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;

    s = sin(rad);
    c = cos(rad);
    t = 1 - c;

    a00 = a.m_Cells[0]; a01 = a.m_Cells[1]; a02 = a.m_Cells[2]; a03 = a.m_Cells[3];
    a10 = a.m_Cells[4]; a11 = a.m_Cells[5]; a12 = a.m_Cells[6]; a13 = a.m_Cells[7];
    a20 = a.m_Cells[8]; a21 = a.m_Cells[9]; a22 = a.m_Cells[10]; a23 = a.m_Cells[11];

    // Construct the elements of the rotation matrix
    b00 = x * x * t + c; b01 = y * x * t + z * s; b02 = z * x * t - y * s;
    b10 = x * y * t - z * s; b11 = y * y * t + c; b12 = z * y * t + x * s;
    b20 = x * z * t + y * s; b21 = y * z * t - x * s; b22 = z * z * t + c;

    // Perform rotation-specific matrix multiplication
    out.m_Cells[0] = a00 * b00 + a10 * b01 + a20 * b02;
    out.m_Cells[1] = a01 * b00 + a11 * b01 + a21 * b02;
    out.m_Cells[2] = a02 * b00 + a12 * b01 + a22 * b02;
    out.m_Cells[3] = a03 * b00 + a13 * b01 + a23 * b02;
    out.m_Cells[4] = a00 * b10 + a10 * b11 + a20 * b12;
    out.m_Cells[5] = a01 * b10 + a11 * b11 + a21 * b12;
    out.m_Cells[6] = a02 * b10 + a12 * b11 + a22 * b12;
    out.m_Cells[7] = a03 * b10 + a13 * b11 + a23 * b12;
    out.m_Cells[8] = a00 * b20 + a10 * b21 + a20 * b22;
    out.m_Cells[9] = a01 * b20 + a11 * b21 + a21 * b22;
    out.m_Cells[10] = a02 * b20 + a12 * b21 + a22 * b22;
    out.m_Cells[11] = a03 * b20 + a13 * b21 + a23 * b22;

    if (a != out) { // If the source and destination differ, copy the unchanged last row
        out.m_Cells[12] = a.m_Cells[12];
        out.m_Cells[13] = a.m_Cells[13];
        out.m_Cells[14] = a.m_Cells[14];
        out.m_Cells[15] = a.m_Cells[15];
    }
    return out;
};

/**
 * Rotates a matrix by the given angle around the X axis not using SIMD
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param rad the angle to rotate the matrix by
 * @returns {mat4} out
 */
mat4 mat4::rotateX(mat4& out, const mat4 a, const GLfloat rad)
{
    GLfloat s = sin(rad),
    c = cos(rad),
    a10 = a.m_Cells[4],
    a11 = a.m_Cells[5],
    a12 = a.m_Cells[6],
    a13 = a.m_Cells[7],
    a20 = a.m_Cells[8],
    a21 = a.m_Cells[9],
    a22 = a.m_Cells[10],
    a23 = a.m_Cells[11];

    if (a != out) { // If the source and destination differ, copy the unchanged rows
        out.m_Cells[0]  = a.m_Cells[0];
        out.m_Cells[1]  = a.m_Cells[1];
        out.m_Cells[2]  = a.m_Cells[2];
        out.m_Cells[3]  = a.m_Cells[3];
        out.m_Cells[12] = a.m_Cells[12];
        out.m_Cells[13] = a.m_Cells[13];
        out.m_Cells[14] = a.m_Cells[14];
        out.m_Cells[15] = a.m_Cells[15];
    }

    // Perform axis-specific matrix multiplication
    out.m_Cells[4] = a10 * c + a20 * s;
    out.m_Cells[5] = a11 * c + a21 * s;
    out.m_Cells[6] = a12 * c + a22 * s;
    out.m_Cells[7] = a13 * c + a23 * s;
    out.m_Cells[8] = a20 * c - a10 * s;
    out.m_Cells[9] = a21 * c - a11 * s;
    out.m_Cells[10] = a22 * c - a12 * s;
    out.m_Cells[11] = a23 * c - a13 * s;
    return out;
};

/**
 * Rotates a matrix by the given angle around the Y axis not using SIMD
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param rad the angle to rotate the matrix by
 * @returns {mat4} out
 */
mat4 mat4::rotateY(mat4& out, const mat4 a, const GLfloat rad)
{
    GLfloat s = sin(rad),
    c = cos(rad),
    a00 = a.m_Cells[0],
    a01 = a.m_Cells[1],
    a02 = a.m_Cells[2],
    a03 = a.m_Cells[3],
    a20 = a.m_Cells[8],
    a21 = a.m_Cells[9],
    a22 = a.m_Cells[10],
    a23 = a.m_Cells[11];

    if (a != out) { // If the source and destination differ, copy the unchanged rows
        out.m_Cells[4]  = a.m_Cells[4];
        out.m_Cells[5]  = a.m_Cells[5];
        out.m_Cells[6]  = a.m_Cells[6];
        out.m_Cells[7]  = a.m_Cells[7];
        out.m_Cells[12] = a.m_Cells[12];
        out.m_Cells[13] = a.m_Cells[13];
        out.m_Cells[14] = a.m_Cells[14];
        out.m_Cells[15] = a.m_Cells[15];
    }

    // Perform axis-specific matrix multiplication
    out.m_Cells[0] = a00 * c - a20 * s;
    out.m_Cells[1] = a01 * c - a21 * s;
    out.m_Cells[2] = a02 * c - a22 * s;
    out.m_Cells[3] = a03 * c - a23 * s;
    out.m_Cells[8] = a00 * s + a20 * c;
    out.m_Cells[9] = a01 * s + a21 * c;
    out.m_Cells[10] = a02 * s + a22 * c;
    out.m_Cells[11] = a03 * s + a23 * c;
    return out;
};

/**
 * Rotates a matrix by the given angle around the Z axis not using SIMD
 *
 * @param out the receiving matrix
 * @param a the matrix to rotate
 * @param rad the angle to rotate the matrix by
 * @returns {mat4} out
 */
mat4 mat4::rotateZ(mat4& out, const mat4 a, const GLfloat rad)
{
    GLfloat s = sin(rad),
    c = cos(rad),
    a00 = a.m_Cells[0],
    a01 = a.m_Cells[1],
    a02 = a.m_Cells[2],
    a03 = a.m_Cells[3],
    a10 = a.m_Cells[4],
    a11 = a.m_Cells[5],
    a12 = a.m_Cells[6],
    a13 = a.m_Cells[7];

    if (a != out) { // If the source and destination differ, copy the unchanged last row
        out.m_Cells[8]  = a.m_Cells[8];
        out.m_Cells[9]  = a.m_Cells[9];
        out.m_Cells[10] = a.m_Cells[10];
        out.m_Cells[11] = a.m_Cells[11];
        out.m_Cells[12] = a.m_Cells[12];
        out.m_Cells[13] = a.m_Cells[13];
        out.m_Cells[14] = a.m_Cells[14];
        out.m_Cells[15] = a.m_Cells[15];
    }

    // Perform axis-specific matrix multiplication
    out.m_Cells[0] = a00 * c + a10 * s;
    out.m_Cells[1] = a01 * c + a11 * s;
    out.m_Cells[2] = a02 * c + a12 * s;
    out.m_Cells[3] = a03 * c + a13 * s;
    out.m_Cells[4] = a10 * c - a00 * s;
    out.m_Cells[5] = a11 * c - a01 * s;
    out.m_Cells[6] = a12 * c - a02 * s;
    out.m_Cells[7] = a13 * c - a03 * s;
    return out;
};

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
mat4 mat4::fromTranslation(mat4& out, const vec3 v)
{
    out.m_Cells[0] = 1;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 1;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = 1;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = v.m_Cells[0];
    out.m_Cells[13] = v.m_Cells[1];
    out.m_Cells[14] = v.m_Cells[2];
    out.m_Cells[15] = 1;
    return out;
}

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
mat4 mat4::fromScaling(mat4& out, const vec3 v)
{
    out.m_Cells[0] = v.m_Cells[0];
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = v.m_Cells[1];
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = v.m_Cells[2];
    out.m_Cells[11] = 0;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;
    return out;
}

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
mat4 mat4::fromRotation(mat4& out, const GLfloat rad, const vec3 axis)
{
    GLfloat x = axis.m_Cells[0], y = axis.m_Cells[1], z = axis.m_Cells[2],
    len = sqrt(x * x + y * y + z * z),
    s, c, t;

if (fabs(len) < GLMAT_EPSILON) { return null; }

    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;

    s = sin(rad);
    c = cos(rad);
    t = 1 - c;

    // Perform rotation-specific matrix multiplication
    out.m_Cells[0] = x * x * t + c;
    out.m_Cells[1] = y * x * t + z * s;
    out.m_Cells[2] = z * x * t - y * s;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = x * y * t - z * s;
    out.m_Cells[5] = y * y * t + c;
    out.m_Cells[6] = z * y * t + x * s;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = x * z * t + y * s;
    out.m_Cells[9] = y * z * t - x * s;
    out.m_Cells[10] = z * z * t + c;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;
    return out;
}

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
mat4 mat4::fromXRotation(mat4& out, const GLfloat rad)
{
    GLfloat s = sin(rad),
    c = cos(rad);

    // Perform axis-specific matrix multiplication
    out.m_Cells[0]  = 1;
    out.m_Cells[1]  = 0;
    out.m_Cells[2]  = 0;
    out.m_Cells[3]  = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = c;
    out.m_Cells[6] = s;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = -s;
    out.m_Cells[10] = c;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;
    return out;
}

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
mat4 mat4::fromYRotation(mat4& out, const GLfloat rad)
{
    GLfloat s = sin(rad),
    c = cos(rad);

    // Perform axis-specific matrix multiplication
    out.m_Cells[0]  = c;
    out.m_Cells[1]  = 0;
    out.m_Cells[2]  = -s;
    out.m_Cells[3]  = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = 1;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = s;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = c;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;
    return out;
}

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
mat4 mat4::fromZRotation(mat4& out, const GLfloat rad)
{
    GLfloat s = sin(rad),
    c = cos(rad);

    // Perform axis-specific matrix multiplication
    out.m_Cells[0]  = c;
    out.m_Cells[1]  = s;
    out.m_Cells[2]  = 0;
    out.m_Cells[3]  = 0;
    out.m_Cells[4] = -s;
    out.m_Cells[5] = c;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = 1;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;
    return out;
}

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
mat4 mat4::fromRotationTranslation(mat4& out, const quat q, const vec3 v)
{
    // Quaternion math
    GLfloat x = q.m_Cells[0], y = q.m_Cells[1], z = q.m_Cells[2], w = q.m_Cells[3],
    x2 = x + x,
    y2 = y + y,
    z2 = z + z,

    xx = x * x2,
    xy = x * y2,
    xz = x * z2,
    yy = y * y2,
    yz = y * z2,
    zz = z * z2,
    wx = w * x2,
    wy = w * y2,
    wz = w * z2;

    out.m_Cells[0] = 1 - (yy + zz);
    out.m_Cells[1] = xy + wz;
    out.m_Cells[2] = xz - wy;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = xy - wz;
    out.m_Cells[5] = 1 - (xx + zz);
    out.m_Cells[6] = yz + wx;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = xz + wy;
    out.m_Cells[9] = yz - wx;
    out.m_Cells[10] = 1 - (xx + yy);
    out.m_Cells[11] = 0;
    out.m_Cells[12] = v.m_Cells[0];
    out.m_Cells[13] = v.m_Cells[1];
    out.m_Cells[14] = v.m_Cells[2];
    out.m_Cells[15] = 1;

    return out;
};

/**
 * Returns the translation vector component of a transformation
 *  matrix. If a matrix is built with fromRotationTranslation,
 *  the returned vector will be the same as the translation vector
 *  originally supplied.
 * @param  {vec3} out Vector to receive translation component
 * @param  {mat4} mat Matrix to be decomposed (input)
 * @returns {vec3} out
 */
vec3 mat4::getTranslation(vec3& out, const mat4 mat)
{
    out.m_Cells[0] = mat.m_Cells[12];
    out.m_Cells[1] = mat.m_Cells[13];
    out.m_Cells[2] = mat.m_Cells[14];

    return out;
};

/**
 * Returns a quaternion representing the rotational component
 *  of a transformation matrix. If a matrix is built with
 *  fromRotationTranslation, the returned quaternion will be the
 *  same as the quaternion originally supplied.
 * @param out Quaternion to receive the rotation component
 * @param mat Matrix to be decomposed (input)
 * @returns {quat} out
 */
quat mat4::getRotation(quat& out, const mat4 mat)
{
    // Algorithm taken from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
    GLfloat trace = mat.m_Cells[0] + mat.m_Cells[5] + mat.m_Cells[10];
    GLfloat S = 0;

    if (trace > 0) {
        S = sqrt(trace + 1.0) * 2;
        out.m_Cells[3] = 0.25 * S;
        out.m_Cells[0] = (mat.m_Cells[6] - mat.m_Cells[9]) / S;
        out.m_Cells[1] = (mat.m_Cells[8] - mat.m_Cells[2]) / S;
        out.m_Cells[2] = (mat.m_Cells[1] - mat.m_Cells[4]) / S;
    } else if ((mat.m_Cells[0] > mat.m_Cells[5])&(mat.m_Cells[0] > mat.m_Cells[10])) {
        S = sqrt(1.0 + mat.m_Cells[0] - mat.m_Cells[5] - mat.m_Cells[10]) * 2;
        out.m_Cells[3] = (mat.m_Cells[6] - mat.m_Cells[9]) / S;
        out.m_Cells[0] = 0.25 * S;
        out.m_Cells[1] = (mat.m_Cells[1] + mat.m_Cells[4]) / S;
        out.m_Cells[2] = (mat.m_Cells[8] + mat.m_Cells[2]) / S;
    } else if (mat.m_Cells[5] > mat.m_Cells[10]) {
        S = sqrt(1.0 + mat.m_Cells[5] - mat.m_Cells[0] - mat.m_Cells[10]) * 2;
        out.m_Cells[3] = (mat.m_Cells[8] - mat.m_Cells[2]) / S;
        out.m_Cells[0] = (mat.m_Cells[1] + mat.m_Cells[4]) / S;
        out.m_Cells[1] = 0.25 * S;
        out.m_Cells[2] = (mat.m_Cells[6] + mat.m_Cells[9]) / S;
    } else {
        S = sqrt(1.0 + mat.m_Cells[10] - mat.m_Cells[0] - mat.m_Cells[5]) * 2;
        out.m_Cells[3] = (mat.m_Cells[1] - mat.m_Cells[4]) / S;
        out.m_Cells[0] = (mat.m_Cells[8] + mat.m_Cells[2]) / S;
        out.m_Cells[1] = (mat.m_Cells[6] + mat.m_Cells[9]) / S;
        out.m_Cells[2] = 0.25 * S;
    }

    return out;
};

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
mat4 mat4::fromRotationTranslationScale(mat4& out, const quat q, const vec3 v, const vec3 s)
{
    // Quaternion math
    GLfloat x = q.m_Cells[0], y = q.m_Cells[1], z = q.m_Cells[2], w = q.m_Cells[3],
    x2 = x + x,
    y2 = y + y,
    z2 = z + z,

    xx = x * x2,
    xy = x * y2,
    xz = x * z2,
    yy = y * y2,
    yz = y * z2,
    zz = z * z2,
    wx = w * x2,
    wy = w * y2,
    wz = w * z2,
    sx = s.m_Cells[0],
    sy = s.m_Cells[1],
    sz = s.m_Cells[2];

    out.m_Cells[0] = (1 - (yy + zz)) * sx;
    out.m_Cells[1] = (xy + wz) * sx;
    out.m_Cells[2] = (xz - wy) * sx;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = (xy - wz) * sy;
    out.m_Cells[5] = (1 - (xx + zz)) * sy;
    out.m_Cells[6] = (yz + wx) * sy;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = (xz + wy) * sz;
    out.m_Cells[9] = (yz - wx) * sz;
    out.m_Cells[10] = (1 - (xx + yy)) * sz;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = v.m_Cells[0];
    out.m_Cells[13] = v.m_Cells[1];
    out.m_Cells[14] = v.m_Cells[2];
    out.m_Cells[15] = 1;

    return out;
};

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
mat4 mat4::fromRotationTranslationScaleOrigin(mat4& out, const quat q, const vec3 v, const vec3 s, const vec3 o)
{
    // Quaternion math
    GLfloat x = q.m_Cells[0], y = q.m_Cells[1], z = q.m_Cells[2], w = q.m_Cells[3],
    x2 = x + x,
    y2 = y + y,
    z2 = z + z,

    xx = x * x2,
    xy = x * y2,
    xz = x * z2,
    yy = y * y2,
    yz = y * z2,
    zz = z * z2,
    wx = w * x2,
    wy = w * y2,
    wz = w * z2,

    sx = s.m_Cells[0],
    sy = s.m_Cells[1],
    sz = s.m_Cells[2],

    ox = o.m_Cells[0],
    oy = o.m_Cells[1],
    oz = o.m_Cells[2];

    out.m_Cells[0] = (1 - (yy + zz)) * sx;
    out.m_Cells[1] = (xy + wz) * sx;
    out.m_Cells[2] = (xz - wy) * sx;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = (xy - wz) * sy;
    out.m_Cells[5] = (1 - (xx + zz)) * sy;
    out.m_Cells[6] = (yz + wx) * sy;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = (xz + wy) * sz;
    out.m_Cells[9] = (yz - wx) * sz;
    out.m_Cells[10] = (1 - (xx + yy)) * sz;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = v.m_Cells[0] + ox - (out.m_Cells[0] * ox + out.m_Cells[4] * oy + out.m_Cells[8] * oz);
    out.m_Cells[13] = v.m_Cells[1] + oy - (out.m_Cells[1] * ox + out.m_Cells[5] * oy + out.m_Cells[9] * oz);
    out.m_Cells[14] = v.m_Cells[2] + oz - (out.m_Cells[2] * ox + out.m_Cells[6] * oy + out.m_Cells[10] * oz);
    out.m_Cells[15] = 1;

    return out;
};

/**
 * Calculates a 4x4 matrix from the given quaternion
 *
 * @param out mat4 receiving operation result
 * @param q Quaternion to create matrix from
 *
 * @returns {mat4} out
 */
mat4 mat4::fromQuat(mat4& out, const quat q)
{
    GLfloat x = q.m_Cells[0], y = q.m_Cells[1], z = q.m_Cells[2], w = q.m_Cells[3],
    x2 = x + x,
    y2 = y + y,
    z2 = z + z,

    xx = x * x2,
    yx = y * x2,
    yy = y * y2,
    zx = z * x2,
    zy = z * y2,
    zz = z * z2,
    wx = w * x2,
    wy = w * y2,
    wz = w * z2;

    out.m_Cells[0] = 1 - yy - zz;
    out.m_Cells[1] = yx + wz;
    out.m_Cells[2] = zx - wy;
    out.m_Cells[3] = 0;

    out.m_Cells[4] = yx - wz;
    out.m_Cells[5] = 1 - xx - zz;
    out.m_Cells[6] = zy + wx;
    out.m_Cells[7] = 0;

    out.m_Cells[8] = zx + wy;
    out.m_Cells[9] = zy - wx;
    out.m_Cells[10] = 1 - xx - yy;
    out.m_Cells[11] = 0;

    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = 0;
    out.m_Cells[15] = 1;

    return out;
};

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
mat4 mat4::frustum(mat4& out, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat near, const GLfloat far)
{
    GLfloat rl = 1 / (right - left),
    tb = 1 / (top - bottom),
    nf = 1 / (near - far);
    out.m_Cells[0] = (near * 2) * rl;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = (near * 2) * tb;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = (right + left) * rl;
    out.m_Cells[9] = (top + bottom) * tb;
    out.m_Cells[10] = (far + near) * nf;
    out.m_Cells[11] = -1;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = (far * near * 2) * nf;
    out.m_Cells[15] = 0;
    return out;
};

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
mat4 mat4::perspective(mat4& out, const GLfloat fovy, const GLfloat aspect, const GLfloat near, const GLfloat far)
{
    GLfloat f = 1.0 / tan(fovy / 2),
    nf = 1 / (near - far);
    out.m_Cells[0] = f / aspect;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = f;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = (far + near) * nf;
    out.m_Cells[11] = -1;
    out.m_Cells[12] = 0;
    out.m_Cells[13] = 0;
    out.m_Cells[14] = (2 * far * near) * nf;
    out.m_Cells[15] = 0;
    return out;
};

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
mat4 mat4::ortho(mat4& out, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat near, const GLfloat far)
{
    GLfloat lr = 1 / (left - right),
    bt = 1 / (bottom - top),
    nf = 1 / (near - far);
    out.m_Cells[0] = -2 * lr;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = 0;
    out.m_Cells[5] = -2 * bt;
    out.m_Cells[6] = 0;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = 0;
    out.m_Cells[9] = 0;
    out.m_Cells[10] = 2 * nf;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = (left + right) * lr;
    out.m_Cells[13] = (top + bottom) * bt;
    out.m_Cells[14] = (far + near) * nf;
    out.m_Cells[15] = 1;
    return out;
};

/**
 * Generates a look-at matrix with the given eye position, focal point, and up axis
 *
 * @param out mat4 frustum matrix will be written into
 * @param eye Position of the viewer
 * @param center Point the viewer is looking at
 * @param up vec3 pointing up
 * @returns {mat4} out
 */
mat4 mat4::lookAt(mat4& out, const vec3 eye, const vec3 center, const vec3 up)
{
    GLfloat x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
    eyex = eye.m_Cells[0],
    eyey = eye.m_Cells[1],
    eyez = eye.m_Cells[2],
    upx = up.m_Cells[0],
    upy = up.m_Cells[1],
    upz = up.m_Cells[2],
    centerx = center.m_Cells[0],
    centery = center.m_Cells[1],
    centerz = center.m_Cells[2];

    if (fabs(eyex - centerx) < GLMAT_EPSILON &&
    fabs(eyey - centery) < GLMAT_EPSILON &&
    fabs(eyez - centerz) < GLMAT_EPSILON) {
        return mat4::identity(out);
    }

    z0 = eyex - centerx;
    z1 = eyey - centery;
    z2 = eyez - centerz;

    len = 1 / sqrt(z0 * z0 + z1 * z1 + z2 * z2);
    z0 *= len;
    z1 *= len;
    z2 *= len;

    x0 = upy * z2 - upz * z1;
    x1 = upz * z0 - upx * z2;
    x2 = upx * z1 - upy * z0;
    len = sqrt(x0 * x0 + x1 * x1 + x2 * x2);
    if (!len) {
        x0 = 0;
        x1 = 0;
        x2 = 0;
    } else {
        len = 1 / len;
        x0 *= len;
        x1 *= len;
        x2 *= len;
    }

    y0 = z1 * x2 - z2 * x1;
    y1 = z2 * x0 - z0 * x2;
    y2 = z0 * x1 - z1 * x0;

    len = sqrt(y0 * y0 + y1 * y1 + y2 * y2);
    if (!len) {
        y0 = 0;
        y1 = 0;
        y2 = 0;
    } else {
        len = 1 / len;
        y0 *= len;
        y1 *= len;
        y2 *= len;
    }

    out.m_Cells[0] = x0;
    out.m_Cells[1] = y0;
    out.m_Cells[2] = z0;
    out.m_Cells[3] = 0;
    out.m_Cells[4] = x1;
    out.m_Cells[5] = y1;
    out.m_Cells[6] = z1;
    out.m_Cells[7] = 0;
    out.m_Cells[8] = x2;
    out.m_Cells[9] = y2;
    out.m_Cells[10] = z2;
    out.m_Cells[11] = 0;
    out.m_Cells[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
    out.m_Cells[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
    out.m_Cells[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
    out.m_Cells[15] = 1;

    return out;
};

/**
 * Returns Frobenius norm of a mat4
 *
 * @param a the matrix to calculate Frobenius norm of
 * @returns {Number} Frobenius norm
 */
GLfloat mat4::frob(const mat4 a)
{
    return(sqrt(pow(a.m_Cells[0], 2) + pow(a.m_Cells[1], 2) + pow(a.m_Cells[2], 2) + pow(a.m_Cells[3], 2) + pow(a.m_Cells[4], 2) + pow(a.m_Cells[5], 2) + pow(a.m_Cells[6], 2) + pow(a.m_Cells[7], 2) + pow(a.m_Cells[8], 2) + pow(a.m_Cells[9], 2) + pow(a.m_Cells[10], 2) + pow(a.m_Cells[11], 2) + pow(a.m_Cells[12], 2) + pow(a.m_Cells[13], 2) + pow(a.m_Cells[14], 2) + pow(a.m_Cells[15], 2) ));
};

/**
 * Adds two mat4's
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat4} out
 */
mat4 mat4::add(mat4& out, const mat4 a, const mat4 b)
{
    out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] + b.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4] + b.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5] + b.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6] + b.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7] + b.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8] + b.m_Cells[8];
    out.m_Cells[9] = a.m_Cells[9] + b.m_Cells[9];
    out.m_Cells[10] = a.m_Cells[10] + b.m_Cells[10];
    out.m_Cells[11] = a.m_Cells[11] + b.m_Cells[11];
    out.m_Cells[12] = a.m_Cells[12] + b.m_Cells[12];
    out.m_Cells[13] = a.m_Cells[13] + b.m_Cells[13];
    out.m_Cells[14] = a.m_Cells[14] + b.m_Cells[14];
    out.m_Cells[15] = a.m_Cells[15] + b.m_Cells[15];
    return out;
};

/**
 * Subtracts matrix b from matrix a
 *
 * @param out the receiving matrix
 * @param a the first operand
 * @param b the second operand
 * @returns {mat4} out
 */
mat4 mat4::subtract(mat4& out, const mat4 a, const mat4 b)
{
    out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] - b.m_Cells[3];
    out.m_Cells[4] = a.m_Cells[4] - b.m_Cells[4];
    out.m_Cells[5] = a.m_Cells[5] - b.m_Cells[5];
    out.m_Cells[6] = a.m_Cells[6] - b.m_Cells[6];
    out.m_Cells[7] = a.m_Cells[7] - b.m_Cells[7];
    out.m_Cells[8] = a.m_Cells[8] - b.m_Cells[8];
    out.m_Cells[9] = a.m_Cells[9] - b.m_Cells[9];
    out.m_Cells[10] = a.m_Cells[10] - b.m_Cells[10];
    out.m_Cells[11] = a.m_Cells[11] - b.m_Cells[11];
    out.m_Cells[12] = a.m_Cells[12] - b.m_Cells[12];
    out.m_Cells[13] = a.m_Cells[13] - b.m_Cells[13];
    out.m_Cells[14] = a.m_Cells[14] - b.m_Cells[14];
    out.m_Cells[15] = a.m_Cells[15] - b.m_Cells[15];
    return out;
};

/**
 * Multiply each element of the matrix by a scalar.
 *
 * @param out the receiving matrix
 * @param a the matrix to scale
 * @param b amount to scale the matrix's elements by
 * @returns {mat4} out
 */
mat4 mat4::multiplyScalar(mat4& out, const mat4 a, const GLfloat b)
{
    out.m_Cells[0] = a.m_Cells[0] * b;
    out.m_Cells[1] = a.m_Cells[1] * b;
    out.m_Cells[2] = a.m_Cells[2] * b;
    out.m_Cells[3] = a.m_Cells[3] * b;
    out.m_Cells[4] = a.m_Cells[4] * b;
    out.m_Cells[5] = a.m_Cells[5] * b;
    out.m_Cells[6] = a.m_Cells[6] * b;
    out.m_Cells[7] = a.m_Cells[7] * b;
    out.m_Cells[8] = a.m_Cells[8] * b;
    out.m_Cells[9] = a.m_Cells[9] * b;
    out.m_Cells[10] = a.m_Cells[10] * b;
    out.m_Cells[11] = a.m_Cells[11] * b;
    out.m_Cells[12] = a.m_Cells[12] * b;
    out.m_Cells[13] = a.m_Cells[13] * b;
    out.m_Cells[14] = a.m_Cells[14] * b;
    out.m_Cells[15] = a.m_Cells[15] * b;
    return out;
};

/**
 * Adds two mat4's after multiplying each element of the second operand by a scalar value.
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param scale the amount to scale b's elements by before adding
 * @returns {mat4} out
 */
mat4 mat4::multiplyScalarAndAdd(mat4& out, const mat4 a, const mat4 b, const GLfloat scale)
{
    out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
    out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
    out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
    out.m_Cells[3] = a.m_Cells[3] + (b.m_Cells[3] * scale);
    out.m_Cells[4] = a.m_Cells[4] + (b.m_Cells[4] * scale);
    out.m_Cells[5] = a.m_Cells[5] + (b.m_Cells[5] * scale);
    out.m_Cells[6] = a.m_Cells[6] + (b.m_Cells[6] * scale);
    out.m_Cells[7] = a.m_Cells[7] + (b.m_Cells[7] * scale);
    out.m_Cells[8] = a.m_Cells[8] + (b.m_Cells[8] * scale);
    out.m_Cells[9] = a.m_Cells[9] + (b.m_Cells[9] * scale);
    out.m_Cells[10] = a.m_Cells[10] + (b.m_Cells[10] * scale);
    out.m_Cells[11] = a.m_Cells[11] + (b.m_Cells[11] * scale);
    out.m_Cells[12] = a.m_Cells[12] + (b.m_Cells[12] * scale);
    out.m_Cells[13] = a.m_Cells[13] + (b.m_Cells[13] * scale);
    out.m_Cells[14] = a.m_Cells[14] + (b.m_Cells[14] * scale);
    out.m_Cells[15] = a.m_Cells[15] + (b.m_Cells[15] * scale);
    return out;
};

/**
 * Returns whether or not the matrices have exactly the same elements in the same position (when compared with ===)
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat4::exactEquals(const mat4 a, const mat4 b)
{
    return a.m_Cells[0] == b.m_Cells[0] && a.m_Cells[1] == b.m_Cells[1] && a.m_Cells[2] == b.m_Cells[2] && a.m_Cells[3] == b.m_Cells[3] &&
    a.m_Cells[4] == b.m_Cells[4] && a.m_Cells[5] == b.m_Cells[5] && a.m_Cells[6] == b.m_Cells[6] && a.m_Cells[7] == b.m_Cells[7] &&
    a.m_Cells[8] == b.m_Cells[8] && a.m_Cells[9] == b.m_Cells[9] && a.m_Cells[10] == b.m_Cells[10] && a.m_Cells[11] == b.m_Cells[11] &&
    a.m_Cells[12] == b.m_Cells[12] && a.m_Cells[13] == b.m_Cells[13] && a.m_Cells[14] == b.m_Cells[14] && a.m_Cells[15] == b.m_Cells[15];
};

/**
 * Returns whether or not the matrices have approximately the same elements in the same position.
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @returns {Boolean} True if the matrices are equal, false otherwise.
 */
bool mat4::equals(const mat4 a, const mat4 b)
{
    GLfloat a0  = a.m_Cells[0],  a1  = a.m_Cells[1],  a2  = a.m_Cells[2],  a3  = a.m_Cells[3],
    a4  = a.m_Cells[4],  a5  = a.m_Cells[5],  a6  = a.m_Cells[6],  a7  = a.m_Cells[7],
    a8  = a.m_Cells[8],  a9  = a.m_Cells[9],  a10 = a.m_Cells[10], a11 = a.m_Cells[11],
    a12 = a.m_Cells[12], a13 = a.m_Cells[13], a14 = a.m_Cells[14], a15 = a.m_Cells[15];

    GLfloat b0  = b.m_Cells[0],  b1  = b.m_Cells[1],  b2  = b.m_Cells[2],  b3  = b.m_Cells[3],
    b4  = b.m_Cells[4],  b5  = b.m_Cells[5],  b6  = b.m_Cells[6],  b7  = b.m_Cells[7],
    b8  = b.m_Cells[8],  b9  = b.m_Cells[9],  b10 = b.m_Cells[10], b11 = b.m_Cells[11],
    b12 = b.m_Cells[12], b13 = b.m_Cells[13], b14 = b.m_Cells[14], b15 = b.m_Cells[15];

    return (fabs(a0 - b0) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a0), fabs(b0))) &&
    fabs(a1 - b1) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a1), fabs(b1))) &&
    fabs(a2 - b2) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a2), fabs(b2))) &&
    fabs(a3 - b3) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a3), fabs(b3))) &&
    fabs(a4 - b4) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a4), fabs(b4))) &&
    fabs(a5 - b5) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a5), fabs(b5))) &&
    fabs(a6 - b6) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a6), fabs(b6))) &&
    fabs(a7 - b7) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a7), fabs(b7))) &&
    fabs(a8 - b8) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a8), fabs(b8))) &&
    fabs(a9 - b9) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a9), fabs(b9))) &&
    fabs(a10 - b10) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a10), fabs(b10))) &&
    fabs(a11 - b11) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a11), fabs(b11))) &&
    fabs(a12 - b12) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a12), fabs(b12))) &&
    fabs(a13 - b13) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a13), fabs(b13))) &&
    fabs(a14 - b14) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a14), fabs(b14))) &&
    fabs(a15 - b15) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a15), fabs(b15))));
};



/**
 * Returns a string representation of a mat4
 *
 * @param a to represent as a string
 * @returns {String} string representation of the mat4
 */
std::string mat4::str(const mat4& a)
{
    std::stringstream ss;
    ss << "mat4(" << a.m_Cells[0];
    for (int i=1; i<16; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {mat4} out
 */
mat4 mat4::zero(mat4& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    out.m_Cells[2] = 0.0;
    out.m_Cells[3] = 0.0;
    out.m_Cells[4] = 0.0;
    out.m_Cells[5] = 0.0;
    out.m_Cells[6] = 0.0;
    out.m_Cells[7] = 0.0;
    out.m_Cells[8] = 0.0;
    out.m_Cells[9] = 0.0;
    out.m_Cells[10] = 0.0;
    out.m_Cells[11] = 0.0;
    out.m_Cells[12] = 0.0;
    out.m_Cells[13] = 0.0;
    out.m_Cells[14] = 0.0;
    out.m_Cells[15] = 0.0;
    return out;
};
/**
 * calls glUniformMatrix4fv for the matrix a
 *
 * @param loc the location of the vector
 * @param a the vector to send to OpenGL shader
 * @returns {void}
 */
void mat4::glUniformMatrix(const GLint loc, const mat4 a)
{
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&a);
};

const vec2 vec2::null;

/**
 * Creates a new, empty vec2
 *
 * @returns {vec2} a new 2D vector
 */
vec2 vec2::create()
{
    vec2 out;
    out.m_Cells[0] = 0;
    out.m_Cells[1] = 0;
    return out;
};

/**
 * Creates a new vec2 initialized with values from an existing vector
 *
 * @param a vector to clone
 * @returns {vec2} a new 2D vector
 */
vec2 vec2::clone(const vec2 a)
{
    vec2 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    return out;
};

/**
 * Creates a new vec2 initialized with the given values
 *
 * @param x X component
 * @param y Y component
 * @returns {vec2} a new 2D vector
 */
vec2 vec2::fromValues(const GLfloat x, const GLfloat y)
{
    vec2 out;
    out.m_Cells[0] = x;
    out.m_Cells[1] = y;
    return out;
};

/**
 * Copy the values from one vec2 to another
 *
 * @param out the receiving vector
 * @param a the source vector
 * @returns {vec2} out
 */
vec2 vec2::copy(vec2& out, const vec2 a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    return out;
};

/**
 * Set the components of a vec2 to the given values
 *
 * @param out the receiving vector
 * @param x X component
 * @param y Y component
 * @returns {vec2} out
 */
vec2 vec2::set(vec2& out, const GLfloat x, const GLfloat y)
{
    out.m_Cells[0] = x;
    out.m_Cells[1] = y;
    return out;
};

/**
 * Adds two vec2's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec2} out
 */
vec2 vec2::add(vec2& out, const vec2 a, const vec2 b)
{
    out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
    return out;
};

/**
 * Subtracts vector b from vector a
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec2} out
 */
vec2 vec2::subtract(vec2& out, const vec2 a, const vec2 b)
{
    out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    return out;
};

/**
 * Multiplies two vec2's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec2} out
 */
vec2 vec2::multiply(vec2& out, const vec2 a, const vec2 b)
{
    out.m_Cells[0] = a.m_Cells[0] * b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] * b.m_Cells[1];
    return out;
};

/**
 * Divides two vec2's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec2} out
 */
vec2 vec2::divide(vec2& out, const vec2 a, const vec2 b)
{
    out.m_Cells[0] = a.m_Cells[0] / b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] / b.m_Cells[1];
    return out;
};

/**
 * Math.ceil the components of a vec2
 *
 * @param out the receiving vector
 * @param a vector to ceil
 * @returns {vec2} out
 */
vec2 vec2::ceil(vec2& out, const vec2 a)
{
    out.m_Cells[0] = ceilf(a.m_Cells[0]);
    out.m_Cells[1] = ceilf(a.m_Cells[1]);
    return out;
};

/**
 * Math.floor the components of a vec2
 *
 * @param out the receiving vector
 * @param a vector to floor
 * @returns {vec2} out
 */
vec2 vec2::floor(vec2& out, const vec2 a)
{
    out.m_Cells[0] = floorf(a.m_Cells[0]);
    out.m_Cells[1] = floorf(a.m_Cells[1]);
    return out;
};

/**
 * Returns the minimum of two vec2's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec2} out
 */
vec2 vec2::min(vec2& out, const vec2 a, const vec2 b)
{
    out.m_Cells[0] = fmin(a.m_Cells[0], b.m_Cells[0]);
    out.m_Cells[1] = fmin(a.m_Cells[1], b.m_Cells[1]);
    return out;
};

/**
 * Returns the maximum of two vec2's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec2} out
 */
vec2 vec2::max(vec2& out, const vec2 a, const vec2 b)
{
    out.m_Cells[0] = fmax(a.m_Cells[0], b.m_Cells[0]);
    out.m_Cells[1] = fmax(a.m_Cells[1], b.m_Cells[1]);
    return out;
};

/**
 * Math.round the components of a vec2
 *
 * @param out the receiving vector
 * @param a vector to round
 * @returns {vec2} out
 */
vec2 vec2::round(vec2& out, const vec2 a)
{
    out.m_Cells[0] = roundf(a.m_Cells[0]);
    out.m_Cells[1] = roundf(a.m_Cells[1]);
    return out;
};

/**
 * Scales a vec2 by a scalar Number
 *
 * @param out the receiving vector
 * @param a the vector to scale
 * @param b amount to scale the vector by
 * @returns {vec2} out
 */
vec2 vec2::scale(vec2& out, const vec2 a, const GLfloat b)
{
    out.m_Cells[0] = a.m_Cells[0] * b;
    out.m_Cells[1] = a.m_Cells[1] * b;
    return out;
};

/**
 * Adds two vec2's after scaling the second operand by a scalar value
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param scale the amount to scale b by before adding
 * @returns {vec2} out
 */
vec2 vec2::scaleAndAdd(vec2& out, const vec2 a, const vec2 b, const GLfloat scale)
{
    out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
    out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
    return out;
};

/**
 * Calculates the euclidian distance between two vec2's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} distance between a and b
 */
GLfloat vec2::distance(const vec2 a, const vec2 b)
{
    GLfloat x = b.m_Cells[0] - a.m_Cells[0],
    y = b.m_Cells[1] - a.m_Cells[1];
    return sqrt(x*x + y*y);
};

/**
 * Calculates the squared euclidian distance between two vec2's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} squared distance between a and b
 */
GLfloat vec2::squaredDistance(const vec2 a, const vec2 b)
{
    GLfloat x = b.m_Cells[0] - a.m_Cells[0],
    y = b.m_Cells[1] - a.m_Cells[1];
    return x*x + y*y;
};

/**
 * Calculates the length of a vec2
 *
 * @param a vector to calculate length of
 * @returns {Number} length of a
 */
GLfloat vec2::length(const vec2 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1];
    return sqrt(x*x + y*y);
};

/**
 * Calculates the squared length of a vec2
 *
 * @param a vector to calculate squared length of
 * @returns {Number} squared length of a
 */
GLfloat vec2::squaredLength(const vec2 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1];
    return x*x + y*y;
};

/**
 * Negates the components of a vec2
 *
 * @param out the receiving vector
 * @param a vector to negate
 * @returns {vec2} out
 */
vec2 vec2::negate(vec2& out, const vec2 a)
{
    out.m_Cells[0] = -a.m_Cells[0];
    out.m_Cells[1] = -a.m_Cells[1];
    return out;
};

/**
 * Returns the inverse of the components of a vec2
 *
 * @param out the receiving vector
 * @param a vector to invert
 * @returns {vec2} out
 */
vec2 vec2::inverse(vec2& out, const vec2 a)
{
    out.m_Cells[0] = 1.0 / a.m_Cells[0];
    out.m_Cells[1] = 1.0 / a.m_Cells[1];
    return out;
};

/**
 * Normalize a vec2
 *
 * @param out the receiving vector
 * @param a vector to normalize
 * @returns {vec2} out
 */
vec2 vec2::normalize(vec2& out, const vec2 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1];
    GLfloat len = x*x + y*y;
    if (len > 0) {
        //TODO: evaluate use of glm_invsqrt here?
        len = 1 / sqrt(len);
        out.m_Cells[0] = a.m_Cells[0] * len;
        out.m_Cells[1] = a.m_Cells[1] * len;
    }
    return out;
};

/**
 * Calculates the dot product of two vec2's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} dot product of a and b
 */
GLfloat vec2::dot(const vec2 a, const vec2 b)
{
    return a.m_Cells[0] * b.m_Cells[0] + a.m_Cells[1] * b.m_Cells[1];
};

/**
 * Computes the cross product of two vec2's
 * Note that the cross product must by definition produce a 3D vector
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec2::cross(vec3& out, const vec2 a, const vec2 b)
{
    GLfloat z = a.m_Cells[0] * b.m_Cells[1] - a.m_Cells[1] * b.m_Cells[0];
    out.m_Cells[0] = out.m_Cells[1] = 0;
    out.m_Cells[2] = z;
    return out;
};

/**
 * Performs a linear interpolation between two vec2's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param t interpolation amount between the two inputs
 * @returns {vec2} out
 */
vec2 vec2::lerp(vec2& out, const vec2 a, const vec2 b, const GLfloat t)
{
    GLfloat ax = a.m_Cells[0],
    ay = a.m_Cells[1];
    out.m_Cells[0] = ax + t * (b.m_Cells[0] - ax);
    out.m_Cells[1] = ay + t * (b.m_Cells[1] - ay);
    return out;
};

/**
 * Generates a random vector with the given scale
 *
 * @param out the receiving vector
 * @param [scale] Length of the resulting vector. If ommitted, a unit vector will be returned
 * @returns {vec2} out
 */
vec2 vec2::random(vec2& out, const GLfloat scale)
{
    GLfloat r = rand()%1000/1000.0 * 2.0 * M_PI;
    out.m_Cells[0] = cos(r) * scale;
    out.m_Cells[1] = sin(r) * scale;
    return out;
};

/**
 * Transforms the vec2 with a mat2
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param m matrix to transform with
 * @returns {vec2} out
 */
vec2 vec2::transformMat2(vec2& out, const vec2 a, const mat2 m)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1];
    out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[2] * y;
    out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[3] * y;
    return out;
};

/**
 * Transforms the vec2 with a mat2d
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param m matrix to transform with
 * @returns {vec2} out
 */
vec2 vec2::transformMat2d(vec2& out, const vec2 a, const mat2d m)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1];
    out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[2] * y + m.m_Cells[4];
    out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[3] * y + m.m_Cells[5];
    return out;
};

/**
 * Transforms the vec2 with a mat3
 * 3rd vector component is implicitly '1'
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param m matrix to transform with
 * @returns {vec2} out
 */
vec2 vec2::transformMat3(vec2& out, const vec2 a, const mat3 m)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1];
    out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[3] * y + m.m_Cells[6];
    out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[4] * y + m.m_Cells[7];
    return out;
};

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
vec2 vec2::transformMat4(vec2& out, const vec2 a, const mat4 m)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1];
    out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[4] * y + m.m_Cells[12];
    out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[5] * y + m.m_Cells[13];
    return out;
};

/**
 * Returns whether or not the vectors exactly have the same elements in the same position (when compared with ===)
 *
 * @param a The first vector.
 * @param b The second vector.
 * @returns {Boolean} True if the vectors are equal, false otherwise.
 */
bool vec2::exactEquals(const vec2 a, const vec2 b)
{
    return a.m_Cells[0] == b.m_Cells[0] && a.m_Cells[1] == b.m_Cells[1];
};

/**
 * Returns whether or not the vectors have approximately the same elements in the same position.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @returns {Boolean} True if the vectors are equal, false otherwise.
 */
bool vec2::equals(const vec2 a, const vec2 b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1];
    GLfloat b0 = b.m_Cells[0], b1 = b.m_Cells[1];
    return (fabs(a0 - b0) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a0), fabs(b0))) &&
    fabs(a1 - b1) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a1), fabs(b1))));
};

/**
 * Returns a string representation of a vec2
 *
 * @param a to represent as a string
 * @returns {String} string representation of the vec2
 */
std::string vec2::str(const vec2& a)
{
    std::stringstream ss;
    ss << "vec2(" << a.m_Cells[0];
    for (int i=1; i<2; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {vec2} out
 */
vec2 vec2::zero(vec2& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    return out;
};
/**
 * Creates a new vec2 initialized from the given vec2
 *
 * @param a
 * @returns {vec2} a new vector
 */
vec2 vec2::fromVec(const vec2 a)
{
    vec2 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    return out;
};
/**
 * Creates a new vec2 initialized from the given vec3
 *
 * @param a
 * @returns {vec2} a new vector
 */
vec2 vec2::fromVec(const vec3 a)
{
    vec2 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    return out;
};
/**
 * Creates a new vec2 initialized from the given vec4
 *
 * @param a
 * @returns {vec2} a new vector
 */
vec2 vec2::fromVec(const vec4 a)
{
    vec2 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    return out;
};
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
vec2 vec2::hermite(vec2& out, const vec2 p0, const vec2 t0, const vec2 p1, const vec2 t1, const GLfloat k)
{
    GLfloat h00 = ((2*k) - 3)*k*k + 1;
    GLfloat h10 = ((k - 2)*k + 1)*k;
    GLfloat h01 = (3 - 2*k)*k*k;
    GLfloat h11 = (k - 1)*k*k;

    vec2::zero(out);
    vec2::scaleAndAdd(out, out, p0, h00);
    vec2::scaleAndAdd(out, out, t0, h10);
    vec2::scaleAndAdd(out, out, p1, h01);
    vec2::scaleAndAdd(out, out, t1, h11);

    return out;
};
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
vec2 vec2::bezier(vec2& out, const vec2 a, const vec2 b, const vec2 c, const vec2 d, const GLfloat t)
{
    GLfloat inverseFactor = 1 - t,
    inverseFactorTimesTwo = inverseFactor * inverseFactor,
    factorTimes2 = t * t,
    factor1 = inverseFactorTimesTwo * inverseFactor,
    factor2 = 3 * t * inverseFactorTimesTwo,
    factor3 = 3 * factorTimes2 * inverseFactor,
    factor4 = factorTimes2 * t;

    vec2::zero(out);
    vec2::scaleAndAdd(out, out, a, factor1);
    vec2::scaleAndAdd(out, out, b, factor2);
    vec2::scaleAndAdd(out, out, c, factor3);
    vec2::scaleAndAdd(out, out, d, factor4);

    return out;
};
/**
 * calls glUniform2fv for the vector a
 *
 * @param loc the location of the vector
 * @param a the vector to send to OpenGL shader
 * @returns {void}
 */
void vec2::glUniform(const GLint loc, const vec2 a)
{
    if (loc >= 0) glUniform2fv(loc, 1, (const GLfloat*)&a);
};
/**
 * calls glUniform2fv for the vector array a
 *
 * @param loc the location of the vector
 * @param a the array of vectors to send to OpenGL shader
 * @returns {void}
 */
void vec2::glUniform(const GLint loc, const std::vector<vec2>& a)
{
    if (loc >= 0) glUniform2fv(loc, a.size(), (const GLfloat*)a.data());
};

const vec3 vec3::null;

/**
 * Creates a new, empty vec3
 *
 * @returns {vec3} a new 3D vector
 */
vec3 vec3::create()
{
    vec3 out;
    out.m_Cells[0] = 0;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    return out;
};

/**
 * Creates a new vec3 initialized with values from an existing vector
 *
 * @param a vector to clone
 * @returns {vec3} a new 3D vector
 */
vec3 vec3::clone(const vec3 a)
{
    vec3 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    return out;
};

/**
 * Creates a new vec3 initialized with the given values
 *
 * @param x X component
 * @param y Y component
 * @param z Z component
 * @returns {vec3} a new 3D vector
 */
vec3 vec3::fromValues(const GLfloat x, const GLfloat y, const GLfloat z)
{
    vec3 out;
    out.m_Cells[0] = x;
    out.m_Cells[1] = y;
    out.m_Cells[2] = z;
    return out;
};

/**
 * Copy the values from one vec3 to another
 *
 * @param out the receiving vector
 * @param a the source vector
 * @returns {vec3} out
 */
vec3 vec3::copy(vec3& out, const vec3 a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    return out;
};

/**
 * Set the components of a vec3 to the given values
 *
 * @param out the receiving vector
 * @param x X component
 * @param y Y component
 * @param z Z component
 * @returns {vec3} out
 */
vec3 vec3::set(vec3& out, const GLfloat x, const GLfloat y, const GLfloat z)
{
    out.m_Cells[0] = x;
    out.m_Cells[1] = y;
    out.m_Cells[2] = z;
    return out;
};

/**
 * Adds two vec3's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec3::add(vec3& out, const vec3 a, const vec3 b)
{
    out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
    return out;
};

/**
 * Subtracts vector b from vector a
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec3::subtract(vec3& out, const vec3 a, const vec3 b)
{
    out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
    return out;
};

/**
 * Multiplies two vec3's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec3::multiply(vec3& out, const vec3 a, const vec3 b)
{
    out.m_Cells[0] = a.m_Cells[0] * b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] * b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] * b.m_Cells[2];
    return out;
};

/**
 * Divides two vec3's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec3::divide(vec3& out, const vec3 a, const vec3 b)
{
    out.m_Cells[0] = a.m_Cells[0] / b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] / b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] / b.m_Cells[2];
    return out;
};

/**
 * Math.ceil the components of a vec3
 *
 * @param out the receiving vector
 * @param a vector to ceil
 * @returns {vec3} out
 */
vec3 vec3::ceil(vec3& out, const vec3 a)
{
    out.m_Cells[0] = ceilf(a.m_Cells[0]);
    out.m_Cells[1] = ceilf(a.m_Cells[1]);
    out.m_Cells[2] = ceilf(a.m_Cells[2]);
    return out;
};

/**
 * Math.floor the components of a vec3
 *
 * @param out the receiving vector
 * @param a vector to floor
 * @returns {vec3} out
 */
vec3 vec3::floor(vec3& out, const vec3 a)
{
    out.m_Cells[0] = floorf(a.m_Cells[0]);
    out.m_Cells[1] = floorf(a.m_Cells[1]);
    out.m_Cells[2] = floorf(a.m_Cells[2]);
    return out;
};

/**
 * Returns the minimum of two vec3's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec3::min(vec3& out, const vec3 a, const vec3 b)
{
    out.m_Cells[0] = fmin(a.m_Cells[0], b.m_Cells[0]);
    out.m_Cells[1] = fmin(a.m_Cells[1], b.m_Cells[1]);
    out.m_Cells[2] = fmin(a.m_Cells[2], b.m_Cells[2]);
    return out;
};

/**
 * Returns the maximum of two vec3's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec3::max(vec3& out, const vec3 a, const vec3 b)
{
    out.m_Cells[0] = fmax(a.m_Cells[0], b.m_Cells[0]);
    out.m_Cells[1] = fmax(a.m_Cells[1], b.m_Cells[1]);
    out.m_Cells[2] = fmax(a.m_Cells[2], b.m_Cells[2]);
    return out;
};

/**
 * Math.round the components of a vec3
 *
 * @param out the receiving vector
 * @param a vector to round
 * @returns {vec3} out
 */
vec3 vec3::round(vec3& out, const vec3 a)
{
    out.m_Cells[0] = roundf(a.m_Cells[0]);
    out.m_Cells[1] = roundf(a.m_Cells[1]);
    out.m_Cells[2] = roundf(a.m_Cells[2]);
    return out;
};

/**
 * Scales a vec3 by a scalar Number
 *
 * @param out the receiving vector
 * @param a the vector to scale
 * @param b amount to scale the vector by
 * @returns {vec3} out
 */
vec3 vec3::scale(vec3& out, const vec3 a, const GLfloat b)
{
    out.m_Cells[0] = a.m_Cells[0] * b;
    out.m_Cells[1] = a.m_Cells[1] * b;
    out.m_Cells[2] = a.m_Cells[2] * b;
    return out;
};

/**
 * Adds two vec3's after scaling the second operand by a scalar value
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param scale the amount to scale b by before adding
 * @returns {vec3} out
 */
vec3 vec3::scaleAndAdd(vec3& out, const vec3 a, const vec3 b, const GLfloat scale)
{
    out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
    out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
    out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
    return out;
};

/**
 * Calculates the euclidian distance between two vec3's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} distance between a and b
 */
GLfloat vec3::distance(const vec3 a, const vec3 b)
{
    GLfloat x = b.m_Cells[0] - a.m_Cells[0],
    y = b.m_Cells[1] - a.m_Cells[1],
    z = b.m_Cells[2] - a.m_Cells[2];
    return sqrt(x*x + y*y + z*z);
};

/**
 * Calculates the squared euclidian distance between two vec3's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} squared distance between a and b
 */
GLfloat vec3::squaredDistance(const vec3 a, const vec3 b)
{
    GLfloat x = b.m_Cells[0] - a.m_Cells[0],
    y = b.m_Cells[1] - a.m_Cells[1],
    z = b.m_Cells[2] - a.m_Cells[2];
    return x*x + y*y + z*z;
};

/**
 * Calculates the length of a vec3
 *
 * @param a vector to calculate length of
 * @returns {Number} length of a
 */
GLfloat vec3::length(const vec3 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1],
    z = a.m_Cells[2];
    return sqrt(x*x + y*y + z*z);
};

/**
 * Calculates the squared length of a vec3
 *
 * @param a vector to calculate squared length of
 * @returns {Number} squared length of a
 */
GLfloat vec3::squaredLength(const vec3 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1],
    z = a.m_Cells[2];
    return x*x + y*y + z*z;
};

/**
 * Negates the components of a vec3
 *
 * @param out the receiving vector
 * @param a vector to negate
 * @returns {vec3} out
 */
vec3 vec3::negate(vec3& out, const vec3 a)
{
    out.m_Cells[0] = -a.m_Cells[0];
    out.m_Cells[1] = -a.m_Cells[1];
    out.m_Cells[2] = -a.m_Cells[2];
    return out;
};

/**
 * Returns the inverse of the components of a vec3
 *
 * @param out the receiving vector
 * @param a vector to invert
 * @returns {vec3} out
 */
vec3 vec3::inverse(vec3& out, const vec3 a)
{
    out.m_Cells[0] = 1.0 / a.m_Cells[0];
    out.m_Cells[1] = 1.0 / a.m_Cells[1];
    out.m_Cells[2] = 1.0 / a.m_Cells[2];
    return out;
};

/**
 * Normalize a vec3
 *
 * @param out the receiving vector
 * @param a vector to normalize
 * @returns {vec3} out
 */
vec3 vec3::normalize(vec3& out, const vec3 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1],
    z = a.m_Cells[2];
    GLfloat len = x*x + y*y + z*z;
    if (len > 0) {
        //TODO: evaluate use of glm_invsqrt here?
        len = 1 / sqrt(len);
        out.m_Cells[0] = a.m_Cells[0] * len;
        out.m_Cells[1] = a.m_Cells[1] * len;
        out.m_Cells[2] = a.m_Cells[2] * len;
    }
    return out;
};

/**
 * Calculates the dot product of two vec3's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} dot product of a and b
 */
GLfloat vec3::dot(const vec3 a, const vec3 b)
{
    return a.m_Cells[0] * b.m_Cells[0] + a.m_Cells[1] * b.m_Cells[1] + a.m_Cells[2] * b.m_Cells[2];
};

/**
 * Computes the cross product of two vec3's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec3} out
 */
vec3 vec3::cross(vec3& out, const vec3 a, const vec3 b)
{
    GLfloat ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2],
    bx = b.m_Cells[0], by = b.m_Cells[1], bz = b.m_Cells[2];

    out.m_Cells[0] = ay * bz - az * by;
    out.m_Cells[1] = az * bx - ax * bz;
    out.m_Cells[2] = ax * by - ay * bx;
    return out;
};

/**
 * Performs a linear interpolation between two vec3's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param t interpolation amount between the two inputs
 * @returns {vec3} out
 */
vec3 vec3::lerp(vec3& out, const vec3 a, const vec3 b, const GLfloat t)
{
    GLfloat ax = a.m_Cells[0],
    ay = a.m_Cells[1],
    az = a.m_Cells[2];
    out.m_Cells[0] = ax + t * (b.m_Cells[0] - ax);
    out.m_Cells[1] = ay + t * (b.m_Cells[1] - ay);
    out.m_Cells[2] = az + t * (b.m_Cells[2] - az);
    return out;
};

/**
 * Generates a random vector with the given scale
 *
 * @param out the receiving vector
 * @param [scale] Length of the resulting vector. If ommitted, a unit vector will be returned
 * @returns {vec3} out
 */
vec3 vec3::random(vec3& out, const GLfloat scale)
{

    GLfloat r = rand()%1000/1000.0 * 2.0 * M_PI;
    GLfloat z = (rand()%1000/1000.0 * 2.0) - 1.0;
    GLfloat zScale = sqrt(1.0-z*z) * scale;

    out.m_Cells[0] = cos(r) * zScale;
    out.m_Cells[1] = sin(r) * zScale;
    out.m_Cells[2] = z * scale;
    return out;
};

/**
 * Transforms the vec3 with a mat4.
 * 4th vector component is implicitly '1'
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param m matrix to transform with
 * @returns {vec3} out
 */
vec3 vec3::transformMat4(vec3& out, const vec3 a, const mat4 m)
{
    GLfloat x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2],
    w = m.m_Cells[3] * x + m.m_Cells[7] * y + m.m_Cells[11] * z + m.m_Cells[15];
    w = w || 1.0;
    out.m_Cells[0] = (m.m_Cells[0] * x + m.m_Cells[4] * y + m.m_Cells[8] * z + m.m_Cells[12]) / w;
    out.m_Cells[1] = (m.m_Cells[1] * x + m.m_Cells[5] * y + m.m_Cells[9] * z + m.m_Cells[13]) / w;
    out.m_Cells[2] = (m.m_Cells[2] * x + m.m_Cells[6] * y + m.m_Cells[10] * z + m.m_Cells[14]) / w;
    return out;
};

/**
 * Transforms the vec3 with a mat3.
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param m the 3x3 matrix to transform with
 * @returns {vec3} out
 */
vec3 vec3::transformMat3(vec3& out, const vec3 a, const mat4 m)
{
    GLfloat x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2];
    out.m_Cells[0] = x * m.m_Cells[0] + y * m.m_Cells[3] + z * m.m_Cells[6];
    out.m_Cells[1] = x * m.m_Cells[1] + y * m.m_Cells[4] + z * m.m_Cells[7];
    out.m_Cells[2] = x * m.m_Cells[2] + y * m.m_Cells[5] + z * m.m_Cells[8];
    return out;
};

/**
 * Transforms the vec3 with a quat
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param q quaternion to transform with
 * @returns {vec3} out
 */
vec3 vec3::transformQuat(vec3& out, const vec3 a, const quat q)
{
    // benchmarks: http://jsperf.com/quaternion-transform-vec3-implementations

    GLfloat x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2],
    qx = q.m_Cells[0], qy = q.m_Cells[1], qz = q.m_Cells[2], qw = q.m_Cells[3],

    // calculate quat * vec
    ix = qw * x + qy * z - qz * y,
    iy = qw * y + qz * x - qx * z,
    iz = qw * z + qx * y - qy * x,
    iw = -qx * x - qy * y - qz * z;

    // calculate result * inverse quat
    out.m_Cells[0] = ix * qw + iw * -qx + iy * -qz - iz * -qy;
    out.m_Cells[1] = iy * qw + iw * -qy + iz * -qx - ix * -qz;
    out.m_Cells[2] = iz * qw + iw * -qz + ix * -qy - iy * -qx;
    return out;
};

/**
 * Rotate a 3D vector around the x-axis
 * @param out The receiving vec3
 * @param a The vec3 point to rotate
 * @param b The origin of the rotation
 * @param c The angle of rotation
 * @returns {vec3} out
 */
vec3 vec3::rotateX(vec3& out, const vec3 a, const vec3 b, const GLfloat c)
{
    vec3 p, r;
    //Translate point to the origin
    p.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    p.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    p.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];

    //perform rotation
    r.m_Cells[0] = p.m_Cells[0];
    r.m_Cells[1] = p.m_Cells[1]*cos(c) - p.m_Cells[2]*sin(c);
    r.m_Cells[2] = p.m_Cells[1]*sin(c) + p.m_Cells[2]*cos(c);

    //translate to correct position
    out.m_Cells[0] = r.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = r.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = r.m_Cells[2] + b.m_Cells[2];

    return out;
};

/**
 * Rotate a 3D vector around the y-axis
 * @param out The receiving vec3
 * @param a The vec3 point to rotate
 * @param b The origin of the rotation
 * @param c The angle of rotation
 * @returns {vec3} out
 */
vec3 vec3::rotateY(vec3& out, const vec3 a, const vec3 b, const GLfloat c)
{
    vec3 p, r;
    //Translate point to the origin
    p.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    p.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    p.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];

    //perform rotation
    r.m_Cells[0] = p.m_Cells[2]*sin(c) + p.m_Cells[0]*cos(c);
    r.m_Cells[1] = p.m_Cells[1];
    r.m_Cells[2] = p.m_Cells[2]*cos(c) - p.m_Cells[0]*sin(c);

    //translate to correct position
    out.m_Cells[0] = r.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = r.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = r.m_Cells[2] + b.m_Cells[2];

    return out;
};

/**
 * Rotate a 3D vector around the z-axis
 * @param out The receiving vec3
 * @param a The vec3 point to rotate
 * @param b The origin of the rotation
 * @param c The angle of rotation
 * @returns {vec3} out
 */
vec3 vec3::rotateZ(vec3& out, const vec3 a, const vec3 b, const GLfloat c)
{
    vec3 p, r;
    //Translate point to the origin
    p.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    p.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    p.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];

    //perform rotation
    r.m_Cells[0] = p.m_Cells[0]*cos(c) - p.m_Cells[1]*sin(c);
    r.m_Cells[1] = p.m_Cells[0]*sin(c) + p.m_Cells[1]*cos(c);
    r.m_Cells[2] = p.m_Cells[2];

    //translate to correct position
    out.m_Cells[0] = r.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = r.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = r.m_Cells[2] + b.m_Cells[2];

    return out;
};

/**
 * Get the angle between two 3D vectors
 * @param a The first operand
 * @param b The second operand
 * @returns {Number} The angle in radians
 */
GLfloat vec3::angle(const vec3 a, const vec3 b)
{

    vec3 tempA = vec3::fromValues(a.m_Cells[0], a.m_Cells[1], a.m_Cells[2]);
    vec3 tempB = vec3::fromValues(b.m_Cells[0], b.m_Cells[1], b.m_Cells[2]);

    vec3::normalize(tempA, tempA);
    vec3::normalize(tempB, tempB);

    GLfloat cosine = vec3::dot(tempA, tempB);

    if(cosine > 1.0){
        return 0;
    } else {
        return acos(cosine);
    }
};

/**
 * Returns whether or not the vectors have exactly the same elements in the same position (when compared with ===)
 *
 * @param a The first vector.
 * @param b The second vector.
 * @returns {Boolean} True if the vectors are equal, false otherwise.
 */
bool vec3::exactEquals(const vec3 a, const vec3 b)
{
    return a.m_Cells[0] == b.m_Cells[0] && a.m_Cells[1] == b.m_Cells[1] && a.m_Cells[2] == b.m_Cells[2];
};

/**
 * Returns whether or not the vectors have approximately the same elements in the same position.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @returns {Boolean} True if the vectors are equal, false otherwise.
 */
bool vec3::equals(const vec3 a, const vec3 b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2];
    GLfloat b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2];
    return (fabs(a0 - b0) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a0), fabs(b0))) &&
    fabs(a1 - b1) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a1), fabs(b1))) &&
    fabs(a2 - b2) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a2), fabs(b2))));
};

/**
 * Returns a string representation of a vec3
 *
 * @param a to represent as a string
 * @returns {String} string representation of the vec3
 */
std::string vec3::str(const vec3& a)
{
    std::stringstream ss;
    ss << "vec3(" << a.m_Cells[0];
    for (int i=1; i<3; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {vec3} out
 */
vec3 vec3::zero(vec3& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    out.m_Cells[2] = 0.0;
    return out;
};
/**
 * Creates a new vec3 initialized from the given vec2
 *
 * @param a
 * @returns {vec3} a new vector
 */
vec3 vec3::fromVec(const vec2 a)
{
    vec3 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    return out;
};
/**
 * Creates a new vec3 initialized from the given vec3
 *
 * @param a
 * @returns {vec3} a new vector
 */
vec3 vec3::fromVec(const vec3 a)
{
    vec3 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    return out;
};
/**
 * Creates a new vec3 initialized from the given vec4
 *
 * @param a
 * @returns {vec3} a new vector
 */
vec3 vec3::fromVec(const vec4 a)
{
    vec3 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    return out;
};
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
vec3 vec3::hermite(vec3& out, const vec3 p0, const vec3 t0, const vec3 p1, const vec3 t1, const GLfloat k)
{
    GLfloat h00 = ((2*k) - 3)*k*k + 1;
    GLfloat h10 = ((k - 2)*k + 1)*k;
    GLfloat h01 = (3 - 2*k)*k*k;
    GLfloat h11 = (k - 1)*k*k;

    vec3::zero(out);
    vec3::scaleAndAdd(out, out, p0, h00);
    vec3::scaleAndAdd(out, out, t0, h10);
    vec3::scaleAndAdd(out, out, p1, h01);
    vec3::scaleAndAdd(out, out, t1, h11);

    return out;
};
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
vec3 vec3::bezier(vec3& out, const vec3 a, const vec3 b, const vec3 c, const vec3 d, const GLfloat t)
{
    GLfloat inverseFactor = 1 - t,
    inverseFactorTimesTwo = inverseFactor * inverseFactor,
    factorTimes2 = t * t,
    factor1 = inverseFactorTimesTwo * inverseFactor,
    factor2 = 3 * t * inverseFactorTimesTwo,
    factor3 = 3 * factorTimes2 * inverseFactor,
    factor4 = factorTimes2 * t;

    vec3::zero(out);
    vec3::scaleAndAdd(out, out, a, factor1);
    vec3::scaleAndAdd(out, out, b, factor2);
    vec3::scaleAndAdd(out, out, c, factor3);
    vec3::scaleAndAdd(out, out, d, factor4);

    return out;
};
/**
 * calls glUniform3fv for the vector a
 *
 * @param loc the location of the vector
 * @param a the vector to send to OpenGL shader
 * @returns {void}
 */
void vec3::glUniform(const GLint loc, const vec3 a)
{
    if (loc >= 0) glUniform3fv(loc, 1, (const GLfloat*)&a);
};
/**
 * calls glUniform3fv for the vector array a
 *
 * @param loc the location of the vector
 * @param a the array of vectors to send to OpenGL shader
 * @returns {void}
 */
void vec3::glUniform(const GLint loc, const std::vector<vec3>& a)
{
    if (loc >= 0) glUniform3fv(loc, a.size(), (const GLfloat*)a.data());
};

const vec4 vec4::null;

/**
 * Creates a new, empty vec4
 *
 * @returns {vec4} a new 4D vector
 */
vec4 vec4::create()
{
    vec4 out;
    out.m_Cells[0] = 0;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 0;
    return out;
};

/**
 * Creates a new vec4 initialized with values from an existing vector
 *
 * @param a vector to clone
 * @returns {vec4} a new 4D vector
 */
vec4 vec4::clone(const vec4 a)
{
    vec4 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    return out;
};

/**
 * Creates a new vec4 initialized with the given values
 *
 * @param x X component
 * @param y Y component
 * @param z Z component
 * @param w W component
 * @returns {vec4} a new 4D vector
 */
vec4 vec4::fromValues(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
{
    vec4 out;
    out.m_Cells[0] = x;
    out.m_Cells[1] = y;
    out.m_Cells[2] = z;
    out.m_Cells[3] = w;
    return out;
};

/**
 * Copy the values from one vec4 to another
 *
 * @param out the receiving vector
 * @param a the source vector
 * @returns {vec4} out
 */
vec4 vec4::copy(vec4& out, const vec4 a)
{
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    return out;
};

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
vec4 vec4::set(vec4& out, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
{
    out.m_Cells[0] = x;
    out.m_Cells[1] = y;
    out.m_Cells[2] = z;
    out.m_Cells[3] = w;
    return out;
};

/**
 * Adds two vec4's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec4} out
 */
vec4 vec4::add(vec4& out, const vec4 a, const vec4 b)
{
    out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] + b.m_Cells[3];
    return out;
};

/**
 * Subtracts vector b from vector a
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec4} out
 */
vec4 vec4::subtract(vec4& out, const vec4 a, const vec4 b)
{
    out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] - b.m_Cells[3];
    return out;
};

/**
 * Multiplies two vec4's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec4} out
 */
vec4 vec4::multiply(vec4& out, const vec4 a, const vec4 b)
{
    out.m_Cells[0] = a.m_Cells[0] * b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] * b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] * b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] * b.m_Cells[3];
    return out;
};

/**
 * Divides two vec4's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec4} out
 */
vec4 vec4::divide(vec4& out, const vec4 a, const vec4 b)
{
    out.m_Cells[0] = a.m_Cells[0] / b.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1] / b.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2] / b.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3] / b.m_Cells[3];
    return out;
};

/**
 * Math.ceil the components of a vec4
 *
 * @param out the receiving vector
 * @param a vector to ceil
 * @returns {vec4} out
 */
vec4 vec4::ceil(vec4& out, const vec4 a)
{
    out.m_Cells[0] = ceilf(a.m_Cells[0]);
    out.m_Cells[1] = ceilf(a.m_Cells[1]);
    out.m_Cells[2] = ceilf(a.m_Cells[2]);
    out.m_Cells[3] = ceilf(a.m_Cells[3]);
    return out;
};

/**
 * Math.floor the components of a vec4
 *
 * @param out the receiving vector
 * @param a vector to floor
 * @returns {vec4} out
 */
vec4 vec4::floor(vec4& out, const vec4 a)
{
    out.m_Cells[0] = floorf(a.m_Cells[0]);
    out.m_Cells[1] = floorf(a.m_Cells[1]);
    out.m_Cells[2] = floorf(a.m_Cells[2]);
    out.m_Cells[3] = floorf(a.m_Cells[3]);
    return out;
};

/**
 * Returns the minimum of two vec4's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec4} out
 */
vec4 vec4::min(vec4& out, const vec4 a, const vec4 b)
{
    out.m_Cells[0] = fmin(a.m_Cells[0], b.m_Cells[0]);
    out.m_Cells[1] = fmin(a.m_Cells[1], b.m_Cells[1]);
    out.m_Cells[2] = fmin(a.m_Cells[2], b.m_Cells[2]);
    out.m_Cells[3] = fmin(a.m_Cells[3], b.m_Cells[3]);
    return out;
};

/**
 * Returns the maximum of two vec4's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @returns {vec4} out
 */
vec4 vec4::max(vec4& out, const vec4 a, const vec4 b)
{
    out.m_Cells[0] = fmax(a.m_Cells[0], b.m_Cells[0]);
    out.m_Cells[1] = fmax(a.m_Cells[1], b.m_Cells[1]);
    out.m_Cells[2] = fmax(a.m_Cells[2], b.m_Cells[2]);
    out.m_Cells[3] = fmax(a.m_Cells[3], b.m_Cells[3]);
    return out;
};

/**
 * Math.round the components of a vec4
 *
 * @param out the receiving vector
 * @param a vector to round
 * @returns {vec4} out
 */
vec4 vec4::round(vec4& out, const vec4 a)
{
    out.m_Cells[0] = roundf(a.m_Cells[0]);
    out.m_Cells[1] = roundf(a.m_Cells[1]);
    out.m_Cells[2] = roundf(a.m_Cells[2]);
    out.m_Cells[3] = roundf(a.m_Cells[3]);
    return out;
};

/**
 * Scales a vec4 by a scalar Number
 *
 * @param out the receiving vector
 * @param a the vector to scale
 * @param b amount to scale the vector by
 * @returns {vec4} out
 */
vec4 vec4::scale(vec4& out, const vec4 a, const GLfloat b)
{
    out.m_Cells[0] = a.m_Cells[0] * b;
    out.m_Cells[1] = a.m_Cells[1] * b;
    out.m_Cells[2] = a.m_Cells[2] * b;
    out.m_Cells[3] = a.m_Cells[3] * b;
    return out;
};

/**
 * Adds two vec4's after scaling the second operand by a scalar value
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param scale the amount to scale b by before adding
 * @returns {vec4} out
 */
vec4 vec4::scaleAndAdd(vec4& out, const vec4 a, const vec4 b, const GLfloat scale)
{
    out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
    out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
    out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
    out.m_Cells[3] = a.m_Cells[3] + (b.m_Cells[3] * scale);
    return out;
};

/**
 * Calculates the euclidian distance between two vec4's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} distance between a and b
 */
GLfloat vec4::distance(const vec4 a, const vec4 b)
{
    GLfloat x = b.m_Cells[0] - a.m_Cells[0],
    y = b.m_Cells[1] - a.m_Cells[1],
    z = b.m_Cells[2] - a.m_Cells[2],
    w = b.m_Cells[3] - a.m_Cells[3];
    return sqrt(x*x + y*y + z*z + w*w);
};

/**
 * Calculates the squared euclidian distance between two vec4's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} squared distance between a and b
 */
GLfloat vec4::squaredDistance(const vec4 a, const vec4 b)
{
    GLfloat x = b.m_Cells[0] - a.m_Cells[0],
    y = b.m_Cells[1] - a.m_Cells[1],
    z = b.m_Cells[2] - a.m_Cells[2],
    w = b.m_Cells[3] - a.m_Cells[3];
    return x*x + y*y + z*z + w*w;
};

/**
 * Calculates the length of a vec4
 *
 * @param a vector to calculate length of
 * @returns {Number} length of a
 */
GLfloat vec4::length(const vec4 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1],
    z = a.m_Cells[2],
    w = a.m_Cells[3];
    return sqrt(x*x + y*y + z*z + w*w);
};

/**
 * Calculates the squared length of a vec4
 *
 * @param a vector to calculate squared length of
 * @returns {Number} squared length of a
 */
GLfloat vec4::squaredLength(const vec4 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1],
    z = a.m_Cells[2],
    w = a.m_Cells[3];
    return x*x + y*y + z*z + w*w;
};

/**
 * Negates the components of a vec4
 *
 * @param out the receiving vector
 * @param a vector to negate
 * @returns {vec4} out
 */
vec4 vec4::negate(vec4& out, const vec4 a)
{
    out.m_Cells[0] = -a.m_Cells[0];
    out.m_Cells[1] = -a.m_Cells[1];
    out.m_Cells[2] = -a.m_Cells[2];
    out.m_Cells[3] = -a.m_Cells[3];
    return out;
};

/**
 * Returns the inverse of the components of a vec4
 *
 * @param out the receiving vector
 * @param a vector to invert
 * @returns {vec4} out
 */
vec4 vec4::inverse(vec4& out, const vec4 a)
{
    out.m_Cells[0] = 1.0 / a.m_Cells[0];
    out.m_Cells[1] = 1.0 / a.m_Cells[1];
    out.m_Cells[2] = 1.0 / a.m_Cells[2];
    out.m_Cells[3] = 1.0 / a.m_Cells[3];
    return out;
};

/**
 * Normalize a vec4
 *
 * @param out the receiving vector
 * @param a vector to normalize
 * @returns {vec4} out
 */
vec4 vec4::normalize(vec4& out, const vec4 a)
{
    GLfloat x = a.m_Cells[0],
    y = a.m_Cells[1],
    z = a.m_Cells[2],
    w = a.m_Cells[3];
    GLfloat len = x*x + y*y + z*z + w*w;
    if (len > 0) {
        len = 1 / sqrt(len);
        out.m_Cells[0] = x * len;
        out.m_Cells[1] = y * len;
        out.m_Cells[2] = z * len;
        out.m_Cells[3] = w * len;
    }
    return out;
};

/**
 * Calculates the dot product of two vec4's
 *
 * @param a the first operand
 * @param b the second operand
 * @returns {Number} dot product of a and b
 */
GLfloat vec4::dot(const vec4 a, const vec4 b)
{
    return a.m_Cells[0] * b.m_Cells[0] + a.m_Cells[1] * b.m_Cells[1] + a.m_Cells[2] * b.m_Cells[2] + a.m_Cells[3] * b.m_Cells[3];
};

/**
 * Performs a linear interpolation between two vec4's
 *
 * @param out the receiving vector
 * @param a the first operand
 * @param b the second operand
 * @param t interpolation amount between the two inputs
 * @returns {vec4} out
 */
vec4 vec4::lerp(vec4& out, const vec4 a, const vec4 b, const GLfloat t)
{
    GLfloat ax = a.m_Cells[0],
    ay = a.m_Cells[1],
    az = a.m_Cells[2],
    aw = a.m_Cells[3];
    out.m_Cells[0] = ax + t * (b.m_Cells[0] - ax);
    out.m_Cells[1] = ay + t * (b.m_Cells[1] - ay);
    out.m_Cells[2] = az + t * (b.m_Cells[2] - az);
    out.m_Cells[3] = aw + t * (b.m_Cells[3] - aw);
    return out;
};

/**
 * Generates a random vector with the given scale
 *
 * @param out the receiving vector
 * @param [scale] Length of the resulting vector. If ommitted, a unit vector will be returned
 * @returns {vec4} out
 */
vec4 vec4::random(vec4& out, const GLfloat scale)
{

    //TODO: This is a pretty awful way of doing this. Find something better.
    out.m_Cells[0] = rand()%1000/1000.0;
    out.m_Cells[1] = rand()%1000/1000.0;
    out.m_Cells[2] = rand()%1000/1000.0;
    out.m_Cells[3] = rand()%1000/1000.0;
    vec4::normalize(out, out);
    vec4::scale(out, out, scale);
    return out;
};

/**
 * Transforms the vec4 with a mat4.
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param m matrix to transform with
 * @returns {vec4} out
 */
vec4 vec4::transformMat4(vec4& out, const vec4 a, const mat4 m)
{
    GLfloat x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2], w = a.m_Cells[3];
    out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[4] * y + m.m_Cells[8] * z + m.m_Cells[12] * w;
    out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[5] * y + m.m_Cells[9] * z + m.m_Cells[13] * w;
    out.m_Cells[2] = m.m_Cells[2] * x + m.m_Cells[6] * y + m.m_Cells[10] * z + m.m_Cells[14] * w;
    out.m_Cells[3] = m.m_Cells[3] * x + m.m_Cells[7] * y + m.m_Cells[11] * z + m.m_Cells[15] * w;
    return out;
};

/**
 * Transforms the vec4 with a quat
 *
 * @param out the receiving vector
 * @param a the vector to transform
 * @param q quaternion to transform with
 * @returns {vec4} out
 */
vec4 vec4::transformQuat(vec4& out, const vec4 a, const quat q)
{
    GLfloat x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2],
    qx = q.m_Cells[0], qy = q.m_Cells[1], qz = q.m_Cells[2], qw = q.m_Cells[3],

    // calculate quat * vec
    ix = qw * x + qy * z - qz * y,
    iy = qw * y + qz * x - qx * z,
    iz = qw * z + qx * y - qy * x,
    iw = -qx * x - qy * y - qz * z;

    // calculate result * inverse quat
    out.m_Cells[0] = ix * qw + iw * -qx + iy * -qz - iz * -qy;
    out.m_Cells[1] = iy * qw + iw * -qy + iz * -qx - ix * -qz;
    out.m_Cells[2] = iz * qw + iw * -qz + ix * -qy - iy * -qx;
    out.m_Cells[3] = a.m_Cells[3];
    return out;
};

/**
 * Returns whether or not the vectors have exactly the same elements in the same position (when compared with ===)
 *
 * @param a The first vector.
 * @param b The second vector.
 * @returns {Boolean} True if the vectors are equal, false otherwise.
 */
bool vec4::exactEquals(const vec4 a, const vec4 b)
{
    return a.m_Cells[0] == b.m_Cells[0] && a.m_Cells[1] == b.m_Cells[1] && a.m_Cells[2] == b.m_Cells[2] && a.m_Cells[3] == b.m_Cells[3];
};

/**
 * Returns whether or not the vectors have approximately the same elements in the same position.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @returns {Boolean} True if the vectors are equal, false otherwise.
 */
bool vec4::equals(const vec4 a, const vec4 b)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3];
    GLfloat b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3];
    return (fabs(a0 - b0) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a0), fabs(b0))) &&
    fabs(a1 - b1) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a1), fabs(b1))) &&
    fabs(a2 - b2) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a2), fabs(b2))) &&
    fabs(a3 - b3) <= GLMAT_EPSILON*fmax(1.0, fmax(fabs(a3), fabs(b3))));
};

/**
 * Returns a string representation of a vec4
 *
 * @param a to represent as a string
 * @returns {String} string representation of the vec4
 */
std::string vec4::str(const vec4& a)
{
    std::stringstream ss;
    ss << "vec4(" << a.m_Cells[0];
    for (int i=1; i<4; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {vec4} out
 */
vec4 vec4::zero(vec4& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    out.m_Cells[2] = 0.0;
    out.m_Cells[3] = 0.0;
    return out;
};
/**
 * Creates a new vec4 initialized from the given vec2
 *
 * @param a
 * @returns {vec4} a new vector
 */
vec4 vec4::fromVec(const vec2 a)
{
    vec4 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    return out;
};
/**
 * Creates a new vec4 initialized from the given vec3
 *
 * @param a
 * @returns {vec4} a new vector
 */
vec4 vec4::fromVec(const vec3 a)
{
    vec4 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    return out;
};
/**
 * Creates a new vec4 initialized from the given vec4
 *
 * @param a
 * @returns {vec4} a new vector
 */
vec4 vec4::fromVec(const vec4 a)
{
    vec4 out;
    out.m_Cells[0] = a.m_Cells[0];
    out.m_Cells[1] = a.m_Cells[1];
    out.m_Cells[2] = a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    return out;
};
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
vec4 vec4::hermite(vec4& out, const vec4 p0, const vec4 t0, const vec4 p1, const vec4 t1, const GLfloat k)
{
    GLfloat h00 = ((2*k) - 3)*k*k + 1;
    GLfloat h10 = ((k - 2)*k + 1)*k;
    GLfloat h01 = (3 - 2*k)*k*k;
    GLfloat h11 = (k - 1)*k*k;

    vec4::zero(out);
    vec4::scaleAndAdd(out, out, p0, h00);
    vec4::scaleAndAdd(out, out, t0, h10);
    vec4::scaleAndAdd(out, out, p1, h01);
    vec4::scaleAndAdd(out, out, t1, h11);

    return out;
};
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
vec4 vec4::bezier(vec4& out, const vec4 a, const vec4 b, const vec4 c, const vec4 d, const GLfloat t)
{
    GLfloat inverseFactor = 1 - t,
    inverseFactorTimesTwo = inverseFactor * inverseFactor,
    factorTimes2 = t * t,
    factor1 = inverseFactorTimesTwo * inverseFactor,
    factor2 = 3 * t * inverseFactorTimesTwo,
    factor3 = 3 * factorTimes2 * inverseFactor,
    factor4 = factorTimes2 * t;

    vec4::zero(out);
    vec4::scaleAndAdd(out, out, a, factor1);
    vec4::scaleAndAdd(out, out, b, factor2);
    vec4::scaleAndAdd(out, out, c, factor3);
    vec4::scaleAndAdd(out, out, d, factor4);

    return out;
};
/**
 * calls glUniform4fv for the vector a
 *
 * @param loc the location of the vector
 * @param a the vector to send to OpenGL shader
 * @returns {void}
 */
void vec4::glUniform(const GLint loc, const vec4 a)
{
    if (loc >= 0) glUniform4fv(loc, 1, (const GLfloat*)&a);
};
/**
 * calls glUniform4fv for the vector array a
 *
 * @param loc the location of the vector
 * @param a the array of vectors to send to OpenGL shader
 * @returns {void}
 */
void vec4::glUniform(const GLint loc, const std::vector<vec4>& a)
{
    if (loc >= 0) glUniform4fv(loc, a.size(), (const GLfloat*)a.data());
};

const quat quat::null;

/**
 * Creates a new identity quat
 *
 * @returns {quat} a new quaternion
 */
quat quat::create()
{
    quat out;
    out.m_Cells[0] = 0;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 1;
    return out;
};

/**
 * Set a quat to the identity quaternion
 *
 * @param out the receiving quaternion
 * @returns {quat} out
 */
quat quat::identity(quat& out)
{
    out.m_Cells[0] = 0;
    out.m_Cells[1] = 0;
    out.m_Cells[2] = 0;
    out.m_Cells[3] = 1;
    return out;
};

/**
 * Sets a quat from the given angle and rotation axis,
 * then returns it.
 *
 * @param out the receiving quaternion
 * @param axis the axis around which to rotate
 * @param rad the angle in radians
 * @returns {quat} out
 **/
quat quat::setAxisAngle(quat& out, const vec3 axis, const GLfloat rad)
{
    GLfloat angle = rad * 0.5;
    GLfloat s = sin(angle);
    out.m_Cells[0] = s * axis.m_Cells[0];
    out.m_Cells[1] = s * axis.m_Cells[1];
    out.m_Cells[2] = s * axis.m_Cells[2];
    out.m_Cells[3] = cos(angle);
    return out;
};

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
GLfloat quat::getAxisAngle(vec3& out_axis, const quat q)
{
    GLfloat rad = acos(q.m_Cells[3]) * 2.0;
    GLfloat s = sin(rad / 2.0);
    if (s != 0.0) {
        out_axis.m_Cells[0] = q.m_Cells[0] / s;
        out_axis.m_Cells[1] = q.m_Cells[1] / s;
        out_axis.m_Cells[2] = q.m_Cells[2] / s;
    } else {
        // If s is zero, return any axis (no rotation - axis does not matter)
        out_axis.m_Cells[0] = 1;
        out_axis.m_Cells[1] = 0;
        out_axis.m_Cells[2] = 0;
    }
    return rad;
};

/**
 * Multiplies two quat's
 *
 * @param out the receiving quaternion
 * @param a the first operand
 * @param b the second operand
 * @returns {quat} out
 */
quat quat::multiply(quat& out, const quat a, const quat b)
{
    GLfloat ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
    bx = b.m_Cells[0], by = b.m_Cells[1], bz = b.m_Cells[2], bw = b.m_Cells[3];

    out.m_Cells[0] = ax * bw + aw * bx + ay * bz - az * by;
    out.m_Cells[1] = ay * bw + aw * by + az * bx - ax * bz;
    out.m_Cells[2] = az * bw + aw * bz + ax * by - ay * bx;
    out.m_Cells[3] = aw * bw - ax * bx - ay * by - az * bz;
    return out;
};

/**
 * Rotates a quaternion by the given angle about the X axis
 *
 * @param out quat receiving operation result
 * @param a quat to rotate
 * @param rad angle (in radians) to rotate
 * @returns {quat} out
 */
quat quat::rotateX(quat& out, const quat a, const GLfloat rad)
{
    GLfloat angle = rad * 0.5;

    GLfloat ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
    bx = sin(angle), bw = cos(angle);

    out.m_Cells[0] = ax * bw + aw * bx;
    out.m_Cells[1] = ay * bw + az * bx;
    out.m_Cells[2] = az * bw - ay * bx;
    out.m_Cells[3] = aw * bw - ax * bx;
    return out;
};

/**
 * Rotates a quaternion by the given angle about the Y axis
 *
 * @param out quat receiving operation result
 * @param a quat to rotate
 * @param rad angle (in radians) to rotate
 * @returns {quat} out
 */
quat quat::rotateY(quat& out, const quat a, const GLfloat rad)
{
    GLfloat angle = rad * 0.5;

    GLfloat ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
    by = sin(angle), bw = cos(angle);

    out.m_Cells[0] = ax * bw - az * by;
    out.m_Cells[1] = ay * bw + aw * by;
    out.m_Cells[2] = az * bw + ax * by;
    out.m_Cells[3] = aw * bw - ay * by;
    return out;
};

/**
 * Rotates a quaternion by the given angle about the Z axis
 *
 * @param out quat receiving operation result
 * @param a quat to rotate
 * @param rad angle (in radians) to rotate
 * @returns {quat} out
 */
quat quat::rotateZ(quat& out, const quat a, const GLfloat rad)
{
    GLfloat angle = rad * 0.5;

    GLfloat ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
    bz = sin(angle), bw = cos(angle);

    out.m_Cells[0] = ax * bw + ay * bz;
    out.m_Cells[1] = ay * bw - ax * bz;
    out.m_Cells[2] = az * bw + aw * bz;
    out.m_Cells[3] = aw * bw - az * bz;
    return out;
};

/**
 * Calculates the W component of a quat from the X, Y, and Z components.
 * Assumes that quaternion is 1 unit in length.
 * Any existing W component will be ignored.
 *
 * @param out the receiving quaternion
 * @param a quat to calculate W component of
 * @returns {quat} out
 */
quat quat::calculateW(quat& out, const quat a)
{
    GLfloat x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2];

    out.m_Cells[0] = x;
    out.m_Cells[1] = y;
    out.m_Cells[2] = z;
    out.m_Cells[3] = sqrt(fabs(1.0 - x * x - y * y - z * z));
    return out;
};

/**
 * Performs a spherical linear interpolation between two quat
 *
 * @param out the receiving quaternion
 * @param a the first operand
 * @param b the second operand
 * @param t interpolation amount between the two inputs
 * @returns {quat} out
 */
quat quat::slerp(quat& out, const quat a, const quat b, const GLfloat t)
{
    // benchmarks:
    //    http://jsperf.com/quaternion-slerp-implementations

    GLfloat ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
    bx = b.m_Cells[0], by = b.m_Cells[1], bz = b.m_Cells[2], bw = b.m_Cells[3];

    GLfloat omega, cosom, sinom, scale0, scale1;

    // calc cosine
    cosom = ax * bx + ay * by + az * bz + aw * bw;
    // adjust signs (if necessary)
    if ( cosom < 0.0 ) {
        cosom = -cosom;
        bx = - bx;
        by = - by;
        bz = - bz;
        bw = - bw;
    }
    // calculate coefficients
    if ( (1.0 - cosom) > 0.000001 ) {
        // standard case (slerp)
        omega  = acos(cosom);
        sinom  = sin(omega);
        scale0 = sin((1.0 - t) * omega) / sinom;
        scale1 = sin(t * omega) / sinom;
    } else {
        // "from" and "to" quaternions are very close
        //  ... so we can do a linear interpolation
        scale0 = 1.0 - t;
        scale1 = t;
    }
    // calculate final values
    out.m_Cells[0] = scale0 * ax + scale1 * bx;
    out.m_Cells[1] = scale0 * ay + scale1 * by;
    out.m_Cells[2] = scale0 * az + scale1 * bz;
    out.m_Cells[3] = scale0 * aw + scale1 * bw;

    return out;
};

/**
 * Calculates the inverse of a quat
 *
 * @param out the receiving quaternion
 * @param a quat to calculate inverse of
 * @returns {quat} out
 */
quat quat::invert(quat& out, const quat a)
{
    GLfloat a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],
    dot = a0*a0 + a1*a1 + a2*a2 + a3*a3,
    invDot = dot ? 1.0/dot : 0;

    // TODO: Would be faster to return [0,0,0,0] immediately if dot == 0

    out.m_Cells[0] = -a0*invDot;
    out.m_Cells[1] = -a1*invDot;
    out.m_Cells[2] = -a2*invDot;
    out.m_Cells[3] = a3*invDot;
    return out;
};

/**
 * Calculates the conjugate of a quat
 * If the quaternion is normalized, this function is faster than quat.inverse and produces the same result.
 *
 * @param out the receiving quaternion
 * @param a quat to calculate conjugate of
 * @returns {quat} out
 */
quat quat::conjugate(quat& out, const quat a)
{
    out.m_Cells[0] = -a.m_Cells[0];
    out.m_Cells[1] = -a.m_Cells[1];
    out.m_Cells[2] = -a.m_Cells[2];
    out.m_Cells[3] = a.m_Cells[3];
    return out;
};

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
quat quat::fromMat3(quat& out, const mat3 m)
{
    // Algorithm in Ken Shoemake"s article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".
    GLfloat fTrace = m.m_Cells[0] + m.m_Cells[4] + m.m_Cells[8];
    GLfloat fRoot;

    if ( fTrace > 0.0 ) {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0);  // 2w
        out.m_Cells[3] = 0.5 * fRoot;
        fRoot = 0.5/fRoot;  // 1/(4w)
        out.m_Cells[0] = (m.m_Cells[5]-m.m_Cells[7])*fRoot;
        out.m_Cells[1] = (m.m_Cells[6]-m.m_Cells[2])*fRoot;
        out.m_Cells[2] = (m.m_Cells[1]-m.m_Cells[3])*fRoot;
    } else {
        // |w| <= 1/2
        int i = 0;
        if ( m.m_Cells[4] > m.m_Cells[0] )
        i = 1;
        if ( m.m_Cells[8] > m.m_Cells[i*3+i] )
        i = 2;
        int j = (i+1)%3;
        int k = (i+2)%3;

        fRoot = sqrt(m.m_Cells[i*3+i]-m.m_Cells[j*3+j]-m.m_Cells[k*3+k] + 1.0);
        out.m_Cells[i] = 0.5 * fRoot;
        fRoot = 0.5 / fRoot;
        out.m_Cells[3] = (m.m_Cells[j*3+k] - m.m_Cells[k*3+j]) * fRoot;
        out.m_Cells[j] = (m.m_Cells[j*3+i] + m.m_Cells[i*3+j]) * fRoot;
        out.m_Cells[k] = (m.m_Cells[k*3+i] + m.m_Cells[i*3+k]) * fRoot;
    }

    return out;
};

/**
 * Returns a string representation of a quat
 *
 * @param a to represent as a string
 * @returns {String} string representation of the quat
 */
std::string quat::str(const quat& a)
{
    std::stringstream ss;
    ss << "quat(" << a.m_Cells[0];
    for (int i=1; i<4; i++) ss << ", " << a.m_Cells[i];
    ss << ")";
    return ss.str();
};
/**
 * set all components to zero
 *
 * @param out the receiving vector
 * @returns {quat} out
 */
quat quat::zero(quat& out)
{
    out.m_Cells[0] = 0.0;
    out.m_Cells[1] = 0.0;
    out.m_Cells[2] = 0.0;
    out.m_Cells[3] = 0.0;
    return out;
};
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
quat quat::rotationTo(quat& out, const vec3 a, const vec3 b)
{
    vec3 tmpvec3 = vec3::create();
    vec3 xUnitVec3 = vec3::fromValues(1,0,0);
    vec3 yUnitVec3 = vec3::fromValues(0,1,0);

    GLfloat dot = vec3::dot(a, b);
    if (dot < -0.999999) {
        vec3::cross(tmpvec3, xUnitVec3, a);
        if (vec3::length(tmpvec3) < 0.000001) {
            vec3::cross(tmpvec3, yUnitVec3, a);
        }
        vec3::normalize(tmpvec3, tmpvec3);
        quat::setAxisAngle(out, tmpvec3, M_PI);
        return out;
    } else if (dot > 0.999999) {
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 1;
        return out;
    } else {
        vec3::cross(tmpvec3, a, b);
        out.m_Cells[0] = tmpvec3.m_Cells[0];
        out.m_Cells[1] = tmpvec3.m_Cells[1];
        out.m_Cells[2] = tmpvec3.m_Cells[2];
        out.m_Cells[3] = 1 + dot;
        return quat::normalize(out, out);
    }
};
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
quat quat::setAxes(quat& out, const vec3 view, const vec3 right, const vec3 up)
{
    mat3 matr = mat3::create();

    matr.m_Cells[0] = right.m_Cells[0];
    matr.m_Cells[3] = right.m_Cells[1];
    matr.m_Cells[6] = right.m_Cells[2];

    matr.m_Cells[1] = up.m_Cells[0];
    matr.m_Cells[4] = up.m_Cells[1];
    matr.m_Cells[7] = up.m_Cells[2];

    matr.m_Cells[2] = -view.m_Cells[0];
    matr.m_Cells[5] = -view.m_Cells[1];
    matr.m_Cells[8] = -view.m_Cells[2];

    quat tmp = quat::fromMat3(out, matr);
    return quat::normalize(out, tmp);
};
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
quat quat::sqlerp(quat& out, const quat a, const quat b, const quat c, const quat d, const GLfloat t)
{
    quat temp1 = quat::create();
    quat temp2 = quat::create();

    quat::slerp(temp1, a, d, t);
    quat::slerp(temp2, b, c, t);
    quat::slerp(out, temp1, temp2, 2 * t * (1 - t));

    return out;
};
/**
 * Normalize a quat
 *
 * @param out the receiving quaternion
 * @param a quaternion to normalize
 * @returns {quat} out
 */
quat quat::normalize(quat& out, const quat a)
{
    GLfloat x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2],
        w = a.m_Cells[3];
    GLfloat len = x*x + y*y + z*z + w*w;
    if (len > 0) {
        len = 1 / sqrt(len);
        out.m_Cells[0] = x * len;
        out.m_Cells[1] = y * len;
        out.m_Cells[2] = z * len;
        out.m_Cells[3] = w * len;
    }
    return out;
};
