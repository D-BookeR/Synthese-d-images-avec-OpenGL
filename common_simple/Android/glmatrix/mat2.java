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

package com.dbooker.livreopengl.glmatrix;

import java.nio.FloatBuffer;
import android.opengl.GLES20;

/**
* @class 2x2 Matrix
* @name mat2
*/
@SuppressWarnings("unused")
public class mat2
{
    static float GLMAT_EPSILON = 0.000001f;

    protected FloatBuffer m_Buffer = FloatBuffer.allocate(4);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new identity mat2
    *
    * @return a new 2x2 matrix
    */
    public static mat2 create()
    {
        mat2 out = new mat2();
        out.m_Cells[0] = 1;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 1;
        return out;
    }

    /**
    * Creates a new mat2 initialized with values from an existing matrix
    *
    * @param a matrix to clone
    * @return a new 2x2 matrix
    */
    public static mat2 clone(mat2 a)
    {
        mat2 out = new mat2();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }

    /**
    * Copy the values from one mat2 to another
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat2 copy(mat2 out, mat2 a)
    {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }

    /**
    * Set one of the components of a mat2 to the given value
    *
    * @param out the receiving matrix
    * @param c number 0..3 of component
    * @param v value of component
    * @return out
    */
    public static mat2 set(mat2 out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a mat2
    *
    * @param in the matrix
    * @param c number 0..3 of component
    * @return value of component
    */
    public static float get(mat2 in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Set a mat2 to the identity matrix
    *
    * @param out the receiving matrix
    * @return out
    */
    public static mat2 identity(mat2 out)
    {
        out.m_Cells[0] = 1;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 1;
        return out;
    }

    /**
     * Create a new mat2 with the given values
     *
     * @param m00 Component in column 0, row 0 position (index 0)
     * @param m01 Component in column 0, row 1 position (index 1)
     * @param m10 Component in column 1, row 0 position (index 2)
     * @param m11 Component in column 1, row 1 position (index 3)
     * @returns out A new 2x2 matrix
     */
    public static mat2 fromValues(float m00, float m01, float m10, float m11)
    {
        mat2 out = new mat2();
        out.m_Cells[0] = m00;
        out.m_Cells[1] = m01;
        out.m_Cells[2] = m10;
        out.m_Cells[3] = m11;
        return out;
    }

    /**
    * Transpose the values of a mat2
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat2 transpose(mat2 out, mat2 a)
    {
        // If we are transposing ourselves we can skip a few steps but have to cache some values
        if (out == a) {
            float a1 = a.m_Cells[1];
            out.m_Cells[1] = a.m_Cells[2];
            out.m_Cells[2] = a1;
        } else {
            out.m_Cells[0] = a.m_Cells[0];
            out.m_Cells[1] = a.m_Cells[2];
            out.m_Cells[2] = a.m_Cells[1];
            out.m_Cells[3] = a.m_Cells[3];
        }

        return out;
    }

    /**
    * Inverts a mat2
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat2 invert(mat2 out, mat2 a)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],

        // Calculate the determinant
        det = a0 * a3 - a2 * a1;

        if (Math.abs(det) < GLMAT_EPSILON) {
            return a;
        }
        det = 1.0f / det;

        out.m_Cells[0] =  a3 * det;
        out.m_Cells[1] = -a1 * det;
        out.m_Cells[2] = -a2 * det;
        out.m_Cells[3] =  a0 * det;

        return out;
    }

    /**
    * Calculates the adjugate of a mat2
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat2 adjoint(mat2 out, mat2 a)
    {
        // Caching this value is nessecary if out == a
        float a0 = a.m_Cells[0];
        out.m_Cells[0] =  a.m_Cells[3];
        out.m_Cells[1] = -a.m_Cells[1];
        out.m_Cells[2] = -a.m_Cells[2];
        out.m_Cells[3] =  a0;

        return out;
    }

    /**
    * Calculates the determinant of a mat2
    *
    * @param a the source matrix
    * @return determinant of a
    */
    public static float determinant(mat2 a)
    {
        return a.m_Cells[0] * a.m_Cells[3] - a.m_Cells[2] * a.m_Cells[1];
    }

    /**
    * Multiplies two mat2's
    *
    * @param out the receiving matrix
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static mat2 multiply(mat2 out, mat2 a, mat2 b)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3];
        float b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3];
        out.m_Cells[0] = a0 * b0 + a2 * b1;
        out.m_Cells[1] = a1 * b0 + a3 * b1;
        out.m_Cells[2] = a0 * b2 + a2 * b3;
        out.m_Cells[3] = a1 * b2 + a3 * b3;
        return out;
    }

    /**
    * Rotates a mat2 by the given angle
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param rad the angle to rotate the matrix by
    * @return out
    */
    public static mat2 rotate(mat2 out, mat2 a, float rad)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],
        s = (float) Math.sin(rad),
        c = (float) Math.cos(rad);
        out.m_Cells[0] = a0 *  c + a2 * s;
        out.m_Cells[1] = a1 *  c + a3 * s;
        out.m_Cells[2] = a0 * -s + a2 * c;
        out.m_Cells[3] = a1 * -s + a3 * c;
        return out;
    }

    /**
    * Scales the mat2 by the dimensions in the given vec2
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param v the vec2 to scale the matrix by
    * @return out
    **/
    public static mat2 scale(mat2 out, mat2 a, vec3 v)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],
        v0 = v.m_Cells[0], v1 = v.m_Cells[1];
        out.m_Cells[0] = a0 * v0;
        out.m_Cells[1] = a1 * v0;
        out.m_Cells[2] = a2 * v1;
        out.m_Cells[3] = a3 * v1;
        return out;
    }

    /**
    * Returns a string representation of a mat2
    *
    * @param a matrix to represent as a string
    * @return string representation of the matrix
    */
    public static String str(mat2 a)
    {
        return "mat2(" + a.m_Cells[0] + ", " + a.m_Cells[1] + ", " + a.m_Cells[2] + ", " + a.m_Cells[3] + ")";
    }

    /**
    * Returns Frobenius norm of a mat2
    *
    * @param a the matrix to calculate Frobenius norm of
    * @return Frobenius norm
    */
    public static float frob(mat2 a)
    {
        return((float)Math.sqrt(Math.pow(a.m_Cells[0], 2) + Math.pow(a.m_Cells[1], 2) + Math.pow(a.m_Cells[2], 2) + Math.pow(a.m_Cells[3], 2)));
    }

    /**
    * Returns L, D and U matrices (Lower triangular, Diagonal and Upper triangular) by factorizing the input matrix
    * @param L the lower triangular matrix
    * @param D the diagonal matrix
    * @param U the upper triangular matrix
    * @param a the input matrix to factorize
    */
    public static void LDU(mat2 L, mat2 D, mat2 U, mat2 a)
    {
        L.m_Cells[2] = a.m_Cells[2]/a.m_Cells[0];
        U.m_Cells[0] = a.m_Cells[0];
        U.m_Cells[1] = a.m_Cells[1];
        U.m_Cells[3] = a.m_Cells[3] - L.m_Cells[2] * U.m_Cells[1];
    }

    /**
    * calls GLES20.glUniformMatrix2fv for the matrix a
    *
    * @param loc the location of the matrix
    * @param a the matrix to send to OpenGL shader
    */
    public static void glUniformMatrix(int loc, mat2 a)
    {
        if (loc >= 0) GLES20.glUniformMatrix2fv(loc, 1, false, a.m_Buffer);
    }
}
