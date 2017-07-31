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
* @class 3x3 Matrix
* @name mat3
*/
@SuppressWarnings("unused")
public class mat3
{
    static float GLMAT_EPSILON = 0.000001f;

    protected FloatBuffer m_Buffer = FloatBuffer.allocate(9);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new identity mat3
    *
    * @return a new 3x3 matrix
    */
    public static mat3 create()
    {
        mat3 out = new mat3();
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
    }

    /**
    * Copies the upper-left 3x3 values into the given mat3.
    *
    * @param out the receiving 3x3 matrix
    * @param a   the source 4x4 matrix
    * @return out
    */
    public static mat3 fromMat4(mat3 out, mat4 a)
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
    }

    /**
    * Creates a new mat3 initialized with values from an existing matrix
    *
    * @param a matrix to clone
    * @return a new 3x3 matrix
    */
    public static mat3 clone(mat3 a)
    {
        mat3 out = new mat3();
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
    }

    /**
    * Copy the values from one mat3 to another
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat3 copy(mat3 out, mat3 a)
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
    }

    /**
    * Set one of the components of a mat3 to the given values
    *
    * @param out the receiving matrix
    * @param c number 0..8 of component
    * @param v value of component
    * @return out
    */
    public static mat3 set(mat3 out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a mat3
    *
    * @param in the matrix
    * @param c number 0..8 of component
    * @return value of component
    */
    public static float get(mat3 in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Set a mat3 to the identity matrix
    *
    * @param out the receiving matrix
    * @return out
    */
    public static mat3 identity(mat3 out)
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
    }

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
     * @returns A new mat3
     */
    public static mat3 fromValues(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
    {
        mat3 out = new mat3();
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
    }

    /**
    * Transpose the values of a mat3
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat3 transpose(mat3 out, mat3 a)
    {
        // If we are transposing ourselves we can skip a few steps but have to cache some values
        if (out == a) {
            float a01 = a.m_Cells[1], a02 = a.m_Cells[2], a12 = a.m_Cells[5];
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
    }

    /**
    * Inverts a mat3
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat3 invert(mat3 out, mat3 a)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
        a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
        a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8],

        b01 = a22 * a11 - a12 * a21,
        b11 = -a22 * a10 + a12 * a20,
        b21 = a21 * a10 - a11 * a20,

        // Calculate the determinant
        det = a00 * b01 + a01 * b11 + a02 * b21;

        if (Math.abs(det) < GLMAT_EPSILON) {
            return a;
        }
        det = 1.0f / det;

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
    }

    /**
    * Calculates the adjugate of a mat3
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat3 adjoint(mat3 out, mat3 a)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
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
    }

    /**
    * Calculates the determinant of a mat3
    *
    * @param a the source matrix
    * @return determinant of a
    */
    public static float determinant(mat3 a)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
        a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
        a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8];

        return a00 * (a22 * a11 - a12 * a21) + a01 * (-a22 * a10 + a12 * a20) + a02 * (a21 * a10 - a11 * a20);
    }

    /**
    * Multiplies two mat3's
    *
    * @param out the receiving matrix
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static mat3 multiply(mat3 out, mat3 a, mat3 b)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
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
    }

    /**
    * Translate a mat3 by the given vector
    *
    * @param out the receiving matrix
    * @param a the matrix to translate
    * @param v vector to translate by
    * @return out
    */
    public static mat3 translate(mat3 out, mat3 a, vec3 v)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
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
    }

    /**
    * Rotates a mat3 by the given angle
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param rad the angle to rotate the matrix by
    * @return out
    */
    public static mat3 rotate(mat3 out, mat3 a, float rad)
    {
        float a00 = a.m_Cells[0], a01 = a.m_Cells[1], a02 = a.m_Cells[2],
        a10 = a.m_Cells[3], a11 = a.m_Cells[4], a12 = a.m_Cells[5],
        a20 = a.m_Cells[6], a21 = a.m_Cells[7], a22 = a.m_Cells[8],

        s = (float) Math.sin(rad),
        c = (float) Math.cos(rad);

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
    }

    /**
    * Scales the mat3 by the dimensions in the given vec2
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param v the vec2 to scale the matrix by
    * @return out
    **/
    public static mat3 scale(mat3 out, mat3 a, vec3 v)
    {
        float x = v.m_Cells[0], y = v.m_Cells[1];

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
    }

    /**
    * Copies the values from a mat2d into a mat3
    *
    * @param out the receiving matrix
    * @param a the matrix to copy
    * @return out
    **/
    public static mat3 fromMat2d(mat3 out, mat3 a)
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
    }

    /**
    * Calculates a 3x3 matrix from the given quaternion
    *
    * @param out mat3 receiving operation result
    * @param q Quaternion to create matrix from
    *
    * @return out
    */
    public static mat3 fromQuat(mat3 out, quat q)
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
        out.m_Cells[3] = yx - wz;
        out.m_Cells[6] = zx + wy;

        out.m_Cells[1] = yx + wz;
        out.m_Cells[4] = 1 - xx - zz;
        out.m_Cells[7] = zy - wx;

        out.m_Cells[2] = zx - wy;
        out.m_Cells[5] = zy + wx;
        out.m_Cells[8] = 1 - xx - yy;

        return out;
    }

    /**
    * Calculates a 3x3 normal matrix (transpose inverse) from the 4x4 matrix
    *
    * @param out mat3 receiving operation result
    * @param a Mat4 to derive the normal matrix from
    *
    * @return out
    */
    public static mat3 normalFromMat4(mat3 out, mat3 a)
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
        out.m_Cells[1] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
        out.m_Cells[2] = (a10 * b10 - a11 * b08 + a13 * b06) * det;

        out.m_Cells[3] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
        out.m_Cells[4] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
        out.m_Cells[5] = (a01 * b08 - a00 * b10 - a03 * b06) * det;

        out.m_Cells[6] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
        out.m_Cells[7] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
        out.m_Cells[8] = (a30 * b04 - a31 * b02 + a33 * b00) * det;

        return out;
    }

    /**
    * Returns a string representation of a mat3
    *
    * @param a matrix to represent as a string
    * @return string representation of the matrix
    */
    public static String str(mat3 a)
    {
        return "mat3(" + a.m_Cells[0] + ", " + a.m_Cells[1] + ", " + a.m_Cells[2]
            + ", " + a.m_Cells[3] + ", " + a.m_Cells[4] + ", " + a.m_Cells[5]
            + ", " + a.m_Cells[6] + ", " + a.m_Cells[7] + ", " + a.m_Cells[8] + ")";
    }

    /**
    * Returns Frobenius norm of a mat3
    *
    * @param a the matrix to calculate Frobenius norm of
    * @return Frobenius norm
    */
    public static float frob(mat3 a)
    {
        return((float)Math.sqrt(Math.pow(a.m_Cells[0], 2) + Math.pow(a.m_Cells[1], 2) + Math.pow(a.m_Cells[2], 2) + Math.pow(a.m_Cells[3], 2) + Math.pow(a.m_Cells[4], 2) + Math.pow(a.m_Cells[5], 2) + Math.pow(a.m_Cells[6], 2) + Math.pow(a.m_Cells[7], 2) + Math.pow(a.m_Cells[8], 2)));
    }

    /**
    * calls GLES20.glUniformMatrix3fv for the matrix a
    *
    * @param loc the location of the matrix
    * @param a the matrix to send to OpenGL shader
    */
    public static void glUniformMatrix(int loc, mat3 a)
    {
        if (loc >= 0) GLES20.glUniformMatrix3fv(loc, 1, false, a.m_Buffer);
    }
}
