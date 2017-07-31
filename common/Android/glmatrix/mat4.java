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
* @class 4x4 Matrix
* @name mat4
*/
@SuppressWarnings("unused")
public class mat4
{
    static float GLMAT_EPSILON = 0.000001f;

    protected FloatBuffer m_Buffer = FloatBuffer.allocate(16);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new identity mat4
    *
    * @return a new 4x4 matrix
    */
    public static mat4 create()
    {
        mat4 out = new mat4();
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
    }

    /**
    * Creates a new mat4 initialized with values from an existing matrix
    *
    * @param a matrix to clone
    * @return a new 4x4 matrix
    */
    public static mat4 clone(mat4 a)
    {
        mat4 out = new mat4();
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
    }

    /**
    * Copy the values from one mat4 to another
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat4 copy(mat4 out, mat4 a)
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
    }

    /**
    * Set one of the components of a mat4 to the given values
    *
    * @param out the receiving matrix
    * @param c number 0..15 of component
    * @param v value of component
    * @return out
    */
    public static mat4 set(mat4 out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a mat4
    *
    * @param in the matrix
    * @param c number 0..15 of component
    * @return value of component
    */
    public static float get(mat4 in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Set a mat4 to the identity matrix
    *
    * @param out the receiving matrix
    * @return out
    */
    public static mat4 identity(mat4 out)
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
    }

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
     * @returns A new mat4
     */
    public static mat4 fromValues(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
    {
        mat4 out = new mat4();
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
    }

    /**
    * Transpose the values of a mat4
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat4 transpose(mat4 out, mat4 a)
    {
        // If we are transposing ourselves we can skip a few steps but have to cache some values
        if (out == a) {
            float a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
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
    }

    /**
    * Inverts a mat4
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat4 invert(mat4 out, mat4 a)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
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

        if (Math.abs(det) < GLMAT_EPSILON) {
            return a;
        }
        det = 1.0f / det;

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
    }

    /**
    * Calculates the adjugate of a mat4
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat4 adjoint(mat4 out, mat4 a)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
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
    }

    /**
    * Calculates the determinant of a mat4
    *
    * @param a the source matrix
    * @return determinant of a
    */
    public static float determinant(mat4 a)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
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
    }

    /**
    * Multiplies two mat4's
    *
    * @param out the receiving matrix
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static mat4 multiply(mat4 out, mat4 a, mat4 b)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2], a03 = a.m_Cells[3],
        a10 = a.m_Cells[4], a11 = a.m_Cells[5], a12 = a.m_Cells[6], a13 = a.m_Cells[7],
        a20 = a.m_Cells[8], a21 = a.m_Cells[9], a22 = a.m_Cells[10], a23 = a.m_Cells[11],
        a30 = a.m_Cells[12], a31 = a.m_Cells[13], a32 = a.m_Cells[14], a33 = a.m_Cells[15];

        // Cache only the current line of the second matrix
        float b0  = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3];
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
    }

    /**
    * Translate a mat4 by the given vector
    *
    * @param out the receiving matrix
    * @param a the matrix to translate
    * @param v vector to translate by
    * @return out
    */
    public static mat4 translate(mat4 out, mat4 a, vec3 v)
    {
        float x = v.m_Cells[0], y = v.m_Cells[1], z = v.m_Cells[2],
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
    }

    /**
    * Scales the mat4 by the dimensions in the given vec3
    *
    * @param out the receiving matrix
    * @param a the matrix to scale
    * @param v the vec3 to scale the matrix by
    * @return out
    **/
    public static mat4 scale(mat4 out, mat4 a, vec3 v)
    {
        float x = v.m_Cells[0], y = v.m_Cells[1], z = v.m_Cells[2];

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
    }

    /**
    * Rotates a mat4 by the given angle
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param rad the angle to rotate the matrix by
    * @param axis the axis to rotate around
    * @return out
    */
    public static mat4 rotate(mat4 out, mat4 a, float rad, vec3 axis)
    {
        float x = axis.m_Cells[0], y = axis.m_Cells[1], z = axis.m_Cells[2],
        len = (float)Math.sqrt((double)(x * x + y * y + z * z)),
        s, c, t,
        a00, a01, a02, a03,
        a10, a11, a12, a13,
        a20, a21, a22, a23,
        b00, b01, b02,
        b10, b11, b12,
        b20, b21, b22;

        if (Math.abs(len) < GLMAT_EPSILON) { return a; }

        len = 1 / len;
        x *= len;
        y *= len;
        z *= len;

        s = (float) Math.sin(rad);
        c = (float) Math.cos(rad);
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
    }

    /**
    * Rotates a matrix by the given angle around the X axis
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param rad the angle to rotate the matrix by
    * @return out
    */
    public static mat4 rotateX(mat4 out, mat4 a, float rad)
    {
        float s = (float) Math.sin(rad),
        c = (float) Math.cos(rad),
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
    }

    /**
    * Rotates a matrix by the given angle around the Y axis
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param rad the angle to rotate the matrix by
    * @return out
    */
    public static mat4 rotateY(mat4 out, mat4 a, float rad)
    {
        float s = (float) Math.sin(rad),
        c = (float) Math.cos(rad),
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
    }

    /**
    * Rotates a matrix by the given angle around the Z axis
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param rad the angle to rotate the matrix by
    * @return out
    */
    public static mat4 rotateZ(mat4 out, mat4 a, float rad)
    {
        float s = (float) Math.sin(rad),
        c = (float) Math.cos(rad),
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
    }

    /**
    * Creates a matrix from a quaternion rotation and vector translation
    * This is equivalent to (but much faster than):
    *
    *     mat4.identity(dest);
    *     mat4.translate(dest, vec);
    *     var quatMat = mat4.create();
    *     quat4.toMat4(quat, quatMat);
    *     mat4.multiply(dest, quatMat);
    *
    * @param out mat4 receiving operation result
    * @param q Rotation quaternion
    * @param v Translation vector
    * @return out
    */
    public static mat4 fromRotationTranslation(mat4 out, quat q, vec3 v)
    {
        // Quaternion math
        float x = q.m_Cells[0], y = q.m_Cells[1], z = q.m_Cells[2], w = q.m_Cells[3],
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
    }

    public static mat4 fromQuat(mat4 out, quat q)
    {
        float x = q.m_Cells[0], y = q.m_Cells[1], z = q.m_Cells[2], w = q.m_Cells[3],
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
    }

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
    * @return out
    */
    public static mat4 frustum(mat4 out, float left, float right, float bottom, float top, float near, float far)
    {
        float rl = 1 / (right - left),
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
    }

    /**
    * Generates a perspective projection matrix with the given bounds
    *
    * @param out mat4 frustum matrix will be written into
    * @param fovy Vertical field of view in radians
    * @param aspect Aspect ratio. typically viewport width/height
    * @param near Near bound of the frustum
    * @param far Far bound of the frustum
    * @return out
    */
    public static mat4 perspective(mat4 out, float fovy, float aspect, float near, float far)
    {
        float f = (float) (1.0 / Math.tan(fovy / 2)),
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
    }

    /**
    * Generates a perspective projection matrix with the given field of view.
    * This is primarily useful for generating projection matrices to be used
    * with the still experiemental WebVR API.
    *
    * @param out mat4 frustum matrix will be written into
    * @param fov Object containing the following values: upDegrees, downDegrees, leftDegrees, rightDegrees
    * @param near Near bound of the frustum
    * @param far Far bound of the frustum
    * @return out
    */
    public static mat4 perspectiveFromFieldOfView(mat4 out, float[] fov, float near, float far)
    {
        float upTan = (float) Math.tan(fov[0] * Math.PI/180.0),
        downTan = (float) Math.tan(fov[1] * Math.PI/180.0),
        leftTan = (float) Math.tan(fov[2] * Math.PI/180.0),
        rightTan = (float) Math.tan(fov[3] * Math.PI/180.0),
        xScale = 2.0f / (leftTan + rightTan),
        yScale = 2.0f / (upTan + downTan);

        out.m_Cells[0] = xScale;
        out.m_Cells[1] = 0.0f;
        out.m_Cells[2] = 0.0f;
        out.m_Cells[3] = 0.0f;
        out.m_Cells[4] = 0.0f;
        out.m_Cells[5] = yScale;
        out.m_Cells[6] = 0.0f;
        out.m_Cells[7] = 0.0f;
        out.m_Cells[8] = -((leftTan - rightTan) * xScale * 0.5f);
        out.m_Cells[9] = ((upTan - downTan) * yScale * 0.5f);
        out.m_Cells[10] = far / (near - far);
        out.m_Cells[11] = -1.0f;
        out.m_Cells[12] = 0.0f;
        out.m_Cells[13] = 0.0f;
        out.m_Cells[14] = (far * near) / (near - far);
        out.m_Cells[15] = 0.0f;
        return out;
    }

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
    * @return out
    */
    public static mat4 ortho(mat4 out, float left, float right, float bottom, float top, float near, float far)
    {
        float lr = 1 / (left - right),
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
    }

    /**
    * Generates a look-at matrix with the given eye position, focal point, and up axis
    *
    * @param out mat4 frustum matrix will be written into
    * @param eye Position of the viewer
    * @param center Point the viewer is looking at
    * @param up vec3 pointing up
    * @return out
    */
    public static mat4 lookAt(mat4 out, vec3 eye, vec3 center, vec3 up)
    {
        float x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
        eyex = eye.m_Cells[0],
        eyey = eye.m_Cells[1],
        eyez = eye.m_Cells[2],
        upx = up.m_Cells[0],
        upy = up.m_Cells[1],
        upz = up.m_Cells[2],
        centerx = center.m_Cells[0],
        centery = center.m_Cells[1],
        centerz = center.m_Cells[2];

        if (Math.abs(eyex - centerx) < GLMAT_EPSILON &&
            Math.abs(eyey - centery) < GLMAT_EPSILON &&
            Math.abs(eyez - centerz) < GLMAT_EPSILON) {
            return identity(out);
        }

        z0 = eyex - centerx;
        z1 = eyey - centery;
        z2 = eyez - centerz;

        len = 1 / (float)Math.sqrt((double)(z0 * z0 + z1 * z1 + z2 * z2));
        z0 *= len;
        z1 *= len;
        z2 *= len;

        x0 = upy * z2 - upz * z1;
        x1 = upz * z0 - upx * z2;
        x2 = upx * z1 - upy * z0;
        len = (float)Math.sqrt((double)(x0 * x0 + x1 * x1 + x2 * x2));
        if (Math.abs(len) < GLMAT_EPSILON) {
            x0 = 0;
            x1 = 0;
            x2 = 0;
        } else {
            len = 1.0f / len;
            x0 *= len;
            x1 *= len;
            x2 *= len;
        }

        y0 = z1 * x2 - z2 * x1;
        y1 = z2 * x0 - z0 * x2;
        y2 = z0 * x1 - z1 * x0;

        len = (float)Math.sqrt((double)(y0 * y0 + y1 * y1 + y2 * y2));
        if (Math.abs(len) < GLMAT_EPSILON) {
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
    }

    /**
    * Returns a string representation of a mat4
    *
    * @param a matrix to represent as a string
    * @return string representation of the matrix
    */
    public static String str(mat4 a)
    {
        return "mat3(" + a.m_Cells[0] + ", " + a.m_Cells[1] + ", " + a.m_Cells[2]+ ", " + a.m_Cells[3]
            + ", " + a.m_Cells[4] + ", " + a.m_Cells[5] + ", " + a.m_Cells[6] + ", " + a.m_Cells[7]
            + ", " + a.m_Cells[8] + ", " + a.m_Cells[9] + ", " + a.m_Cells[10] + ", " + a.m_Cells[11]
            + ", " + a.m_Cells[12] + ", " + a.m_Cells[12] + ", " + a.m_Cells[14] + ", " + a.m_Cells[15] + ")";
    }

    /**
    * Returns Frobenius norm of a mat4
    *
    * @param a the matrix to calculate Frobenius norm of
    * @return Frobenius norm
    */
    public static float frob(mat4 a)
    {
        return((float)Math.sqrt(Math.pow(a.m_Cells[0], 2) + Math.pow(a.m_Cells[1], 2) + Math.pow(a.m_Cells[2], 2) + Math.pow(a.m_Cells[3], 2) + Math.pow(a.m_Cells[4], 2) + Math.pow(a.m_Cells[5], 2) + Math.pow(a.m_Cells[6], 2) + Math.pow(a.m_Cells[6], 2) + Math.pow(a.m_Cells[7], 2) + Math.pow(a.m_Cells[8], 2) + Math.pow(a.m_Cells[9], 2) + Math.pow(a.m_Cells[10], 2) + Math.pow(a.m_Cells[11], 2) + Math.pow(a.m_Cells[12], 2) + Math.pow(a.m_Cells[13], 2) + Math.pow(a.m_Cells[14], 2) + Math.pow(a.m_Cells[15], 2) ));
    }

    /**
    * calls GLES20.glUniformMatrix4fv for the matrix a
    *
    * @param loc the location of the matrix
    * @param a the matrix to send to OpenGL shader
    */
    public static void glUniformMatrix(int loc, mat4 a)
    {
        if (loc >= 0) GLES20.glUniformMatrix4fv(loc, 1, false, a.m_Buffer);
    }
}
