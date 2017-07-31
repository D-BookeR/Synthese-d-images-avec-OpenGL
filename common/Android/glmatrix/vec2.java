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
* @class 2 Dimensional Vector
* @name vec2
*/
@SuppressWarnings("unused")
public class vec2
{

    protected FloatBuffer m_Buffer = FloatBuffer.allocate(2);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new, empty vec2
    *
    * @return a new 2D vector
    */
    public static vec2 create()
    {
        vec2 out = new vec2();
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        return out;
    }

    public static vec2 zero(vec2 out)
    {
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        return out;
    }

    /**
    * Creates a new vec2 initialized with values from an existing vector
    *
    * @param a vector to clone
    * @return a new 2D vector
    */
    public static vec2 clone(vec2 a)
    {
        vec2 out = new vec2();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        return out;
    }

    /**
    * Creates a new vec2 initialized with the given values
    *
    * @param x X component
    * @param y Y component
    * @return a new 2D vector
    */
    public static vec2 fromValues(float x, float y)
    {
        vec2 out = new vec2();
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        return out;
    }
    public static vec2 fromValues(double x, double y)
    {
        vec2 out = new vec2();
        out.m_Cells[0] = (float) x;
        out.m_Cells[1] = (float) y;
        return out;
    }

    /**
     * Creates a new vec3 casted from the given vec
     *
     * @param a vector
     * @return a new 3D vector
     */
    public static vec2 cast(vec4 a)
    {
        vec2 out = new vec2();
        out.m_Cells = a.m_Cells;
        return out;
    }
    public static vec2 cast(vec3 a)
    {
        vec2 out = new vec2();
        out.m_Cells = a.m_Cells;
        return out;
    }

    /**
     * Creates a new vec2 initialized from the given vec
     *
     * @param a vector
     * @return a new 2D vector
     */
    public static vec2 fromVec(vec2 a)
    {
        vec2 out = new vec2();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        return out;
    }
    public static vec2 fromVec(vec3 a)
    {
        vec2 out = new vec2();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        return out;
    }
    public static vec2 fromVec(vec4 a)
    {
        vec2 out = new vec2();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        return out;
    }

    /**
    * Copy the values from one vec2 to another
    *
    * @param out the receiving vector
    * @param a the source vector
    * @return out
    */
    public static vec2 copy(vec2 out, vec2 a)
    {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        return out;
    }

    /**
    * Set the components of a vec2 to the given values
    *
    * @param out the receiving vector
    * @param x X component
    * @param y Y component
    * @return out
    */
    public static vec2 set(vec2 out, float x, float y)
    {
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        return out;
    }

    /**
    * Set one of the components of a vec2 to the given values
    *
    * @param out the receiving vector
    * @param c number 0..1 of component
    * @param v value of component
    * @return out
    */
    public static vec2 set(vec2 out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a vec2
    *
    * @param in the vector
    * @param c number 0..1 of component
    * @return value of component
    */
    public static float get(vec2 in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Adds two vec2's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec2 add(vec2 out, vec2 a, vec2 b)
    {
        out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
        return out;
    }

    /**
    * Subtracts vector b from vector a
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec2 subtract(vec2 out, vec2 a, vec2 b)
    {
        out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
        return out;
    }

    /**
    * Multiplies two vec2's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec2 multiply(vec2 out, vec2 a, vec2 b)
    {
        out.m_Cells[0] = a.m_Cells[0] * b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] * b.m_Cells[1];
        return out;
    }

    /**
    * Divides two vec2's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec2 divide(vec2 out, vec2 a, vec2 b)
    {
        out.m_Cells[0] = a.m_Cells[0] / b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] / b.m_Cells[1];
        return out;
    }

    /**
    * Returns the minimum of two vec2's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec2 min(vec2 out, vec2 a, vec2 b)
    {
        out.m_Cells[0] = Math.min(a.m_Cells[0], b.m_Cells[0]);
        out.m_Cells[1] = Math.min(a.m_Cells[1], b.m_Cells[1]);
        return out;
    }

    /**
    * Returns the maximum of two vec2's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec2 max(vec2 out, vec2 a, vec2 b)
    {
        out.m_Cells[0] = Math.max(a.m_Cells[0], b.m_Cells[0]);
        out.m_Cells[1] = Math.max(a.m_Cells[1], b.m_Cells[1]);
        return out;
    }

    /**
    * Scales a vec2 by a scalar number
    *
    * @param out the receiving vector
    * @param a the vector to scale
    * @param b amount to scale the vector by
    * @return out
    */
    public static vec2 scale(vec2 out, vec2 a, float b)
    {
        out.m_Cells[0] = a.m_Cells[0] * b;
        out.m_Cells[1] = a.m_Cells[1] * b;
        return out;
    }

    /**
    * Adds two vec2's after scaling the second operand by a scalar value
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @param scale the amount to scale b by before adding
    * @return out
    */
    public static vec2 scaleAndAdd(vec2 out, vec2 a, vec2 b, float scale)
    {
        out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
        out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
        return out;
    }

    /**
    * Calculates the euclidian distance between two vec2's
    *
    * @param a the first operand
    * @param b the second operand
    * @return distance between a and b
    */
    public static float distance(vec2 a, vec2 b)
    {
        float x = b.m_Cells[0] - a.m_Cells[0],
        y = b.m_Cells[1] - a.m_Cells[1];
        return (float)Math.sqrt((double)(x*x + y*y));
    }

    /**
    * Calculates the squared euclidian distance between two vec2's
    *
    * @param a the first operand
    * @param b the second operand
    * @return squared distance between a and b
    */
    public static float squaredDistance(vec2 a, vec2 b)
    {
        float x = b.m_Cells[0] - a.m_Cells[0],
        y = b.m_Cells[1] - a.m_Cells[1];
        return x*x + y*y;
    }

    /**
    * Calculates the length of a vec2
    *
    * @param a vector to calculate length of
    * @return length of a
    */
    public static float length(vec2 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1];
        return (float)Math.sqrt((double)(x*x + y*y));
    }

    /**
    * Calculates the squared length of a vec2
    *
    * @param a vector to calculate squared length of
    * @return squared length of a
    */
    public static float squaredLength(vec2 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1];
        return x*x + y*y;
    }

    /**
    * Negates the components of a vec2
    *
    * @param out the receiving vector
    * @param a vector to negate
    * @return out
    */
    public static vec2 negate(vec2 out, vec2 a)
    {
        out.m_Cells[0] = -a.m_Cells[0];
        out.m_Cells[1] = -a.m_Cells[1];
        return out;
    }

    /**
     * Returns the inverse of the components of a vec2
     *
     * @param out the receiving vector
     * @param a vector to invert
     * @return out
     */
    public static vec2 inverse(vec2 out, vec2 a)
    {
        out.m_Cells[0] = 1.0f / a.m_Cells[0];
        out.m_Cells[1] = 1.0f / a.m_Cells[1];
        return out;
    }

    /**
    * Normalize a vec2
    *
    * @param out the receiving vector
    * @param a vector to normalize
    * @return out
    */
    public static vec2 normalize(vec2 out, vec2 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1];
        float len = x*x + y*y;
        if (len > 0) {
            //TODO: evaluate use of glm_invsqrt here?
            len = 1 / (float)Math.sqrt((double)len);
            out.m_Cells[0] = a.m_Cells[0] * len;
            out.m_Cells[1] = a.m_Cells[1] * len;
        }
        return out;
    }

    /**
    * Calculates the dot product of two vec2's
    *
    * @param a the first operand
    * @param b the second operand
    * @return dot product of a and b
    */
    public static float dot(vec2 a, vec2 b)
    {
        return a.m_Cells[0] * b.m_Cells[0] + a.m_Cells[1] * b.m_Cells[1];
    }

    /**
    * Computes the cross product of two vec2's
    * Note that the cross product must by definition produce a 3D vector
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec2 cross(vec2 out, vec2 a, vec2 b)
    {
        float z = a.m_Cells[0] * b.m_Cells[1] - a.m_Cells[1] * b.m_Cells[0];
        out.m_Cells[0] = out.m_Cells[1] = 0;
        out.m_Cells[2] = z;
        return out;
    }

    /**
    * Performs a linear interpolation between two vec2's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @param t interpolation amount between the two inputs
    * @return out
    */
    public static vec2 lerp(vec2 out, vec2 a, vec2 b, float t)
    {
        float ax = a.m_Cells[0],
        ay = a.m_Cells[1];
        out.m_Cells[0] = ax + t * (b.m_Cells[0] - ax);
        out.m_Cells[1] = ay + t * (b.m_Cells[1] - ay);
        return out;
    }

    /**
    * Generates a random vector with the given scale
    *
    * @param out the receiving vector
    * @param scale Length of the resulting vector. If ommitted, a unit vector will be returned
    * @return out
    */
    public static vec2 random(vec2 out, float scale)
    {
        float r = (float) (Math.random() * 2.0 * Math.PI);
        out.m_Cells[0] = (float) (Math.cos(r) * scale);
        out.m_Cells[1] = (float) (Math.sin(r) * scale);
        return out;
    }

    /**
    * Transforms the vec2 with a mat2
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param m matrix to transform with
    * @return out
    */
    public static vec2 transformMat2(vec2 out, vec2 a, mat2 m)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1];
        out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[2] * y;
        out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[3] * y;
        return out;
    }

    /**
    * Transforms the vec2 with a mat2d
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param m matrix to transform with
    * @return out
    */
    public static vec2 transformMat2d(vec2 out, vec2 a, mat2d m)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1];
        out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[2] * y + m.m_Cells[4];
        out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[3] * y + m.m_Cells[5];
        return out;
    }

    /**
    * Transforms the vec2 with a mat3
    * 3rd vector component is implicitly '1'
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param m matrix to transform with
    * @return out
    */
    public static vec2 transformMat3(vec2 out, vec2 a, mat3 m)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1];
        out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[3] * y + m.m_Cells[6];
        out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[4] * y + m.m_Cells[7];
        return out;
    }

    /**
    * Transforms the vec2 with a mat4
    * 3rd vector component is implicitly '0'
    * 4th vector component is implicitly '1'
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param m matrix to transform with
    * @return out
    */
    public static vec2 transformMat4(vec2 out, vec2 a, mat4 m)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1];
        out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[4] * y + m.m_Cells[12];
        out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[5] * y + m.m_Cells[13];
        return out;
    }

    /**
    * Returns a string representation of a vector
    *
    * @param a vector to represent as a string
    * @return string representation of the vector
    */
    public static String str(vec2 a)
    {
        return "vec2(" + a.m_Cells[0] + ", " + a.m_Cells[1] + ")";
    }

    /**
    * calls GLES20.glUniform2fv for the vector a
    *
    * @param loc the location of the vector
    * @param a the vector to send to OpenGL shader
    * @return void
    */
    public static void glUniform(int loc, vec2 a)
    {
        if (loc >= 0) GLES20.glUniform2fv(loc, 1, a.m_Buffer);
    }
    /**
     * calls glUniform2fv for the vector array a
     *
     * @param loc the location of the vector
     * @param a the array of vectors to send to OpenGL shader
     */
    public static void glUniform(int loc, vec2[] a)
    {
        if (loc >= 0) {
            final int nb = a.length;
            if (nb == 1) {
                GLES20.glUniform2fv(loc, 1, a[0].m_Buffer);
            } else {
                FloatBuffer buf = FloatBuffer.allocate(nb * 2);
                for (vec2 v: a) {
                    buf.put(v.m_Cells);
                }
                GLES20.glUniform2fv(loc, nb, buf);
            }
        }
    }

    /**
     * affecte une composante
     * @param i : numéro 0..1 de la composante
     * @param v : valeur de la composante
     */
    public void set(int i, float v)
    {
        m_Cells[i] = v;
    }

    /**
     * accesseur pour les composantes
     * @param i : numéro 0..1 de la composante
     */
    public float get(int i)
    {
        return m_Cells[i];
    }

    /**
     * Cette méthode calcule le vec2 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
     * @param out : résultat
     * @param p0 : coordonnées à k=0
     * @param t0 : tangente à k=0
     * @param p1 : coordonnées à k=1
     * @param t1 : tangente à k=1
     * @param k : coefficient d'interpolation
     */
    public static void hermite(vec2 out, vec2 p0, vec2 t0, vec2 p1, vec2 t1, float k)
    {
        float h00 = ((2*k) - 3)*k*k + 1;
        float h10 = ((k - 2)*k + 1)*k;
        float h01 = (3 - 2*k)*k*k;
        float h11 = (k - 1)*k*k;

        vec2.zero(out);
        vec2.scaleAndAdd(out, out, p0, h00);
        vec2.scaleAndAdd(out, out, t0, h10);
        vec2.scaleAndAdd(out, out, p1, h01);
        vec2.scaleAndAdd(out, out, t1, h11);
    }
}
