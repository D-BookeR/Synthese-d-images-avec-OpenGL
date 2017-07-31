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


/**
* @class 2x3 Matrix
* @name mat2d
*
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
@SuppressWarnings("unused")
public class mat2d
{
    protected FloatBuffer m_Buffer = FloatBuffer.allocate(6);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new identity mat2d
    *
    * @return a new 2x3 matrix
    */
    public static mat2d create()
    {
        mat2d out = new mat2d();
        out.m_Cells[0] = 1;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 1;
        out.m_Cells[4] = 0;
        out.m_Cells[5] = 0;
        return out;
    }

    /**
    * Creates a new mat2d initialized with values from an existing matrix
    *
    * @param a matrix to clone
    * @return a new 2x3 matrix
    */
    public static mat2d clone(mat2d a)
    {
        mat2d out = new mat2d();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        out.m_Cells[4] = a.m_Cells[4];
        out.m_Cells[5] = a.m_Cells[5];
        return out;
    }

    /**
    * Copy the values from one mat2d to another
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat2d copy(mat2d out, mat2d a)
    {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        out.m_Cells[4] = a.m_Cells[4];
        out.m_Cells[5] = a.m_Cells[5];
        return out;
    }

    /**
    * Set one of the components of a mat2d to the given values
    *
    * @param out the receiving matrix
    * @param c number 0..5 of component
    * @param v value of component
    * @return out
    */
    public static mat2d set(mat2d out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a mat2d
    *
    * @param in the matrix
    * @param c number 0..5 of component
    * @return value of component
    */
    public static float get(mat2d in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Set a mat2d to the identity matrix
    *
    * @param out the receiving matrix
    * @return out
    */
    public static mat2d identity(mat2d out)
    {
        out.m_Cells[0] = 1;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 1;
        out.m_Cells[4] = 0;
        out.m_Cells[5] = 0;
        return out;
    }

    /**
     * Create a new mat2d with the given values
     *
     * @param a Component A (index 0)
     * @param b Component B (index 1)
     * @param c Component C (index 2)
     * @param d Component D (index 3)
     * @param tx Component TX (index 4)
     * @param ty Component TY (index 5)
     * @returns A new mat2d
     */
    public static mat2d fromValues(float a, float b, float c, float d, float tx, float ty)
    {
        mat2d out = new mat2d();
        out.m_Cells[0] = a;
        out.m_Cells[1] = b;
        out.m_Cells[2] = c;
        out.m_Cells[3] = d;
        out.m_Cells[4] = tx;
        out.m_Cells[5] = ty;
        return out;
    }

    /**
    * Inverts a mat2d
    *
    * @param out the receiving matrix
    * @param a the source matrix
    * @return out
    */
    public static mat2d invert(mat2d out, mat2d a)
    {
        float aa = a.m_Cells[0], ab = a.m_Cells[1], ac = a.m_Cells[2], ad = a.m_Cells[3],
        atx = a.m_Cells[4], aty = a.m_Cells[5];

        float det = aa * ad - ab * ac;
        if (Math.abs(det) < 0.000001){
            return a;
        }
        det = 1.0f / det;

        out.m_Cells[0] = ad * det;
        out.m_Cells[1] = -ab * det;
        out.m_Cells[2] = -ac * det;
        out.m_Cells[3] = aa * det;
        out.m_Cells[4] = (ac * aty - ad * atx) * det;
        out.m_Cells[5] = (ab * atx - aa * aty) * det;
        return out;
    }

    /**
    * Calculates the determinant of a mat2d
    *
    * @param a the source matrix
    * @return determinant of a
    */
    public static float determinant(mat2d a)
    {
        return a.m_Cells[0] * a.m_Cells[3] - a.m_Cells[1] * a.m_Cells[2];
    }

    /**
    * Multiplies two mat2d's
    *
    * @param out the receiving matrix
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static mat2d multiply(mat2d out, mat2d a, mat2d b)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
        b0 = b.m_Cells[0], b1 = b.m_Cells[1], b2 = b.m_Cells[2], b3 = b.m_Cells[3], b4 = b.m_Cells[4], b5 = b.m_Cells[5];
        out.m_Cells[0] = a0 * b0 + a2 * b1;
        out.m_Cells[1] = a1 * b0 + a3 * b1;
        out.m_Cells[2] = a0 * b2 + a2 * b3;
        out.m_Cells[3] = a1 * b2 + a3 * b3;
        out.m_Cells[4] = a0 * b4 + a2 * b5 + a4;
        out.m_Cells[5] = a1 * b4 + a3 * b5 + a5;
        return out;
    }

    /**
    * Rotates a mat2d by the given angle
    *
    * @param out the receiving matrix
    * @param a the matrix to rotate
    * @param rad the angle to rotate the matrix by
    * @return out
    */
    public static mat2d rotate(mat2d out, mat2d a, float rad)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
        s = (float) Math.sin(rad),
        c = (float) Math.cos(rad);
        out.m_Cells[0] = a0 *  c + a2 * s;
        out.m_Cells[1] = a1 *  c + a3 * s;
        out.m_Cells[2] = a0 * -s + a2 * c;
        out.m_Cells[3] = a1 * -s + a3 * c;
        out.m_Cells[4] = a4;
        out.m_Cells[5] = a5;
        return out;
    }

    /**
    * Scales the mat2d by the dimensions in the given vec2
    *
    * @param out the receiving matrix
    * @param a the matrix to translate
    * @param v the vec2 to scale the matrix by
    * @return out
    **/
    public static mat2d scale(mat2d out, mat2d a, vec3 v)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
        v0 = v.m_Cells[0], v1 = v.m_Cells[1];
        out.m_Cells[0] = a0 * v0;
        out.m_Cells[1] = a1 * v0;
        out.m_Cells[2] = a2 * v1;
        out.m_Cells[3] = a3 * v1;
        out.m_Cells[4] = a4;
        out.m_Cells[5] = a5;
        return out;
    }

    /**
    * Translates the mat2d by the dimensions in the given vec2
    *
    * @param out the receiving matrix
    * @param a the matrix to translate
    * @param v the vec2 to translate the matrix by
    * @return out
    **/
    public static mat2d translate(mat2d out, mat2d a, vec3 v)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3], a4 = a.m_Cells[4], a5 = a.m_Cells[5],
        v0 = v.m_Cells[0], v1 = v.m_Cells[1];
        out.m_Cells[0] = a0;
        out.m_Cells[1] = a1;
        out.m_Cells[2] = a2;
        out.m_Cells[3] = a3;
        out.m_Cells[4] = a0 * v0 + a2 * v1 + a4;
        out.m_Cells[5] = a1 * v0 + a3 * v1 + a5;
        return out;
    }

    /**
    * Returns a string representation of a mat2d
    *
    * @param a matrix to represent as a string
    * @return string representation of the matrix
    */
    public static String str(mat2d a)
    {
        return "mat2d(" + a.m_Cells[0] + ", " + a.m_Cells[1]
            + ", " + a.m_Cells[2] + ", " + a.m_Cells[3]
            + ", " + a.m_Cells[4] + ", " + a.m_Cells[5] + ")";
    }

    /**
    * Returns Frobenius norm of a mat2d
    *
    * @param a the matrix to calculate Frobenius norm of
    * @return Frobenius norm
    */
    public static float frob(mat2d a)
    {
        return((float)Math.sqrt(Math.pow(a.m_Cells[0], 2) + Math.pow(a.m_Cells[1], 2) + Math.pow(a.m_Cells[2], 2) + Math.pow(a.m_Cells[3], 2) + Math.pow(a.m_Cells[4], 2) + Math.pow(a.m_Cells[5], 2) + 1));
    }
}
