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
* @class 4 Dimensional Vector
* @name vec4
*/
@SuppressWarnings("unused")
public class vec4
{
    protected FloatBuffer m_Buffer = FloatBuffer.allocate(4);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new, empty vec4
    *
    * @return a new 4D vector
    */
    public static vec4 create()
    {
        vec4 out = new vec4();
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 0;
        return out;
    }

    public static vec4 zero(vec4 out)
    {
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 0;
        return out;
    }

    /**
    * Creates a new vec4 initialized with values from an existing vector
    *
    * @param a vector to clone
    * @return a new 4D vector
    */
    public static vec4 clone(vec4 a)
    {
        vec4 out = new vec4();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }

    /**
    * Creates a new vec4 initialized with the given values
    *
    * @param x X component
    * @param y Y component
    * @param z Z component
    * @param w W component
    * @return a new 4D vector
    */
    public static vec4 fromValues(float x, float y, float z, float w)
    {
        vec4 out = new vec4();
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        out.m_Cells[2] = z;
        out.m_Cells[3] = w;
        return out;
    }
    public static vec4 fromValues(double x, double y, double z, double w)
    {
        vec4 out = new vec4();
        out.m_Cells[0] = (float) x;
        out.m_Cells[1] = (float) y;
        out.m_Cells[2] = (float) z;
        out.m_Cells[3] = (float) w;
        return out;
    }

    /**
     * Creates a new vec4 initialized from the given vec
     *
     * @param a vector
     * @return a new 4D vector
     */
    public static vec4 fromVec(vec2 a)
    {
        vec4 out = new vec4();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 0;
        return out;
    }
    public static vec4 fromVec(vec3 a)
    {
        vec4 out = new vec4();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = 0;
        return out;
    }
    public static vec4 fromVec(vec4 a)
    {
        vec4 out = new vec4();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }

    /**
    * Copy the values from one vec4 to another
    *
    * @param out the receiving vector
    * @param a the source vector
    * @return out
    */
    public static vec4 copy(vec4 out, vec4 a)
    {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }

    /**
    * Set the components of a vec4 to the given values
    *
    * @param out the receiving vector
    * @param x X component
    * @param y Y component
    * @param z Z component
    * @param w W component
    * @return out
    */
    public static vec4 set(vec4 out, float x, float y, float z, float w)
    {
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        out.m_Cells[2] = z;
        out.m_Cells[3] = w;
        return out;
    }

    /**
    * Set one of the components of a vec4 to the given values
    *
    * @param out the receiving vector
    * @param c number 0..3 of component
    * @param v value of component
    * @return out
    */
    public static vec4 set(vec4 out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a vec4
    *
    * @param in the vector
    * @param c number 0..3 of component
    * @return value of component
    */
    public static float get(vec4 in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Adds two vec4's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec4 add(vec4 out, vec4 a, vec4 b)
    {
        out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3] + b.m_Cells[3];
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
    public static vec4 subtract(vec4 out, vec4 a, vec4 b)
    {
        out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3] - b.m_Cells[3];
        return out;
    }

    /**
    * Multiplies two vec4's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec4 multiply(vec4 out, vec4 a, vec4 b)
    {
        out.m_Cells[0] = a.m_Cells[0] * b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] * b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] * b.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3] * b.m_Cells[3];
        return out;
    }

    /**
    * Divides two vec4's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec4 divide(vec4 out, vec4 a, vec4 b)
    {
        out.m_Cells[0] = a.m_Cells[0] / b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] / b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] / b.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3] / b.m_Cells[3];
        return out;
    }

    /**
    * Returns the minimum of two vec4's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec4 min(vec4 out, vec4 a, vec4 b)
    {
        out.m_Cells[0] = Math.min(a.m_Cells[0], b.m_Cells[0]);
        out.m_Cells[1] = Math.min(a.m_Cells[1], b.m_Cells[1]);
        out.m_Cells[2] = Math.min(a.m_Cells[2], b.m_Cells[2]);
        out.m_Cells[3] = Math.min(a.m_Cells[3], b.m_Cells[3]);
        return out;
    }

    /**
    * Returns the maximum of two vec4's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec4 max(vec4 out, vec4 a, vec4 b)
    {
        out.m_Cells[0] = Math.max(a.m_Cells[0], b.m_Cells[0]);
        out.m_Cells[1] = Math.max(a.m_Cells[1], b.m_Cells[1]);
        out.m_Cells[2] = Math.max(a.m_Cells[2], b.m_Cells[2]);
        out.m_Cells[3] = Math.max(a.m_Cells[3], b.m_Cells[3]);
        return out;
    }

    /**
    * Scales a vec4 by a scalar number
    *
    * @param out the receiving vector
    * @param a the vector to scale
    * @param scale amount to scale the vector by
    * @return out
    */
    public static vec4 scale(vec4 out, vec4 a, float scale)
    {
        out.m_Cells[0] = a.m_Cells[0] * scale;
        out.m_Cells[1] = a.m_Cells[1] * scale;
        out.m_Cells[2] = a.m_Cells[2] * scale;
        out.m_Cells[3] = a.m_Cells[3] * scale;
        return out;
    }

    /**
    * Adds two vec4's after scaling the second operand by a scalar value
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @param scale the amount to scale b by before adding
    * @return out
    */
    public static vec4 scaleAndAdd(vec4 out, vec4 a, vec4 b, float scale)
    {
        out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
        out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
        out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
        out.m_Cells[3] = a.m_Cells[3] + (b.m_Cells[3] * scale);
        return out;
    }

    /**
    * Calculates the euclidian distance between two vec4's
    *
    * @param a the first operand
    * @param b the second operand
    * @return distance between a and b
    */
    public static float distance(vec4 a, vec4 b)
    {
        float x = b.m_Cells[0] - a.m_Cells[0],
        y = b.m_Cells[1] - a.m_Cells[1],
        z = b.m_Cells[2] - a.m_Cells[2],
        w = b.m_Cells[3] - a.m_Cells[3];
        return (float) Math.sqrt((double)(x*x + y*y + z*z + w*w));
    }

    /**
    * Calculates the squared euclidian distance between two vec4's
    *
    * @param a the first operand
    * @param b the second operand
    * @return squared distance between a and b
    */
    public static float squaredDistance(vec4 a, vec4 b)
    {
        float x = b.m_Cells[0] - a.m_Cells[0],
        y = b.m_Cells[1] - a.m_Cells[1],
        z = b.m_Cells[2] - a.m_Cells[2],
        w = b.m_Cells[3] - a.m_Cells[3];
        return x*x + y*y + z*z + w*w;
    }

    /**
    * Calculates the length of a vec4
    *
    * @param a vector to calculate length of
    * @return length of a
    */
    public static float length(vec4 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2],
        w = a.m_Cells[3];
        return (float) Math.sqrt((double)(x*x + y*y + z*z + w*w));
    }

    /**
    * Calculates the squared length of a vec4
    *
    * @param a vector to calculate squared length of
    * @return squared length of a
    */
    public static float squaredLength(vec4 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2],
        w = a.m_Cells[3];
        return x*x + y*y + z*z + w*w;
    }

    /**
    * Negates the components of a vec4
    *
    * @param out the receiving vector
    * @param a vector to negate
    * @return out
    */
    public static vec4 negate(vec4 out, vec4 a)
    {
        out.m_Cells[0] = -a.m_Cells[0];
        out.m_Cells[1] = -a.m_Cells[1];
        out.m_Cells[2] = -a.m_Cells[2];
        out.m_Cells[3] = -a.m_Cells[3];
        return out;
    }

    /**
     * Returns the inverse of the components of a vec4
     *
     * @param out the receiving vector
     * @param a vector to invert
     * @return out
     */
    public static vec4 inverse(vec4 out, vec4 a)
    {
        out.m_Cells[0] = 1.0f / a.m_Cells[0];
        out.m_Cells[1] = 1.0f / a.m_Cells[1];
        out.m_Cells[2] = 1.0f / a.m_Cells[2];
        out.m_Cells[3] = 1.0f / a.m_Cells[3];
        return out;
    }

    /**
    * Normalize a vec4
    *
    * @param out the receiving vector
    * @param a vector to normalize
    * @return out
    */
    public static vec4 normalize(vec4 out, vec4 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2],
        w = a.m_Cells[3];
        float len = x*x + y*y + z*z + w*w;
        if (len > 0) {
            len = 1.0f / (float) Math.sqrt((double)len);
            out.m_Cells[0] = a.m_Cells[0] * len;
            out.m_Cells[1] = a.m_Cells[1] * len;
            out.m_Cells[2] = a.m_Cells[2] * len;
            out.m_Cells[3] = a.m_Cells[3] * len;
        }
        return out;
    }

    /**
    * Calculates the dot product of two vec4's
    *
    * @param a the first operand
    * @param b the second operand
    * @return dot product of a and b
    */
    public static float dot(vec4 a, vec4 b)
    {
        return a.m_Cells[0] * b.m_Cells[0] + a.m_Cells[1] * b.m_Cells[1] + a.m_Cells[2] * b.m_Cells[2] + a.m_Cells[3] * b.m_Cells[3];
    }

    /**
    * Performs a linear interpolation between two vec4's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @param t interpolation amount between the two inputs
    * @return out
    */
    public static vec4 lerp(vec4 out, vec4 a, vec4 b, float t)
    {
        float ax = a.m_Cells[0],
        ay = a.m_Cells[1],
        az = a.m_Cells[2],
        aw = a.m_Cells[3];
        out.m_Cells[0] = ax + t * (b.m_Cells[0] - ax);
        out.m_Cells[1] = ay + t * (b.m_Cells[1] - ay);
        out.m_Cells[2] = az + t * (b.m_Cells[2] - az);
        out.m_Cells[3] = aw + t * (b.m_Cells[3] - aw);
        return out;
    }

    /**
    * Generates a random vector with the given scale
    *
    * @param out the receiving vector
    * @param sc Length of the resulting vector. If ommitted, a unit vector will be returned
    * @return out
    */
    public static vec4 random(vec4 out, float sc)
    {
        //TODO: This is a pretty awful way of doing this. Find something better.
        out.m_Cells[0] = (float) (Math.random()*2.0 - 1.0);
        out.m_Cells[1] = (float) (Math.random()*2.0 - 1.0);
        out.m_Cells[2] = (float) (Math.random()*2.0 - 1.0);
        out.m_Cells[3] = (float) (Math.random()*2.0 - 1.0);
        normalize(out, out);
        scale(out, out, sc);
        return out;
    }

    /**
    * Transforms the vec4 with a mat4.
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param m matrix to transform with
    * @return out
    */
    public static vec4 transformMat4(vec4 out, vec4 a, mat4 m)
    {
        float x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2], w = a.m_Cells[3];
        out.m_Cells[0] = m.m_Cells[0] * x + m.m_Cells[4] * y + m.m_Cells[8] * z + m.m_Cells[12] * w;
        out.m_Cells[1] = m.m_Cells[1] * x + m.m_Cells[5] * y + m.m_Cells[9] * z + m.m_Cells[13] * w;
        out.m_Cells[2] = m.m_Cells[2] * x + m.m_Cells[6] * y + m.m_Cells[10] * z + m.m_Cells[14] * w;
        out.m_Cells[3] = m.m_Cells[3] * x + m.m_Cells[7] * y + m.m_Cells[11] * z + m.m_Cells[15] * w;
        return out;
    }

    /**
    * Transforms the vec4 with a quat
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param q quaternion to transform with
    * @return out
    */
    public static vec4 transformQuat(vec4 out, vec4 a, quat q)
    {
        float x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2],
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
    }

    /**
    * Returns a string representation of a vector
    *
    * @param a vector to represent as a string
    * @return string representation of the vector
    */
    public static String str(vec4 a)
    {
        return "vec4(" + a.m_Cells[0] + ", " + a.m_Cells[1] + ", " + a.m_Cells[2] + ", " + a.m_Cells[3] + ")";
    }

    /**
    * calls glUniform4fv for the vector a
    *
    * @param loc the location of the vector
    * @param a the vector to send to OpenGL shader
    */
    public static void glUniform(int loc, vec4 a)
    {
        if (loc >= 0) GLES20.glUniform4fv(loc, 1, a.m_Buffer);
    }
    /**
     * calls glUniform4fv for the vector array a
     *
     * @param loc the location of the vector
     * @param a the array of vectors to send to OpenGL shader
     */
    public static void glUniform(int loc, vec4[] a)
    {
        if (loc >= 0) {
            final int nb = a.length;
            if (nb == 1) {
                GLES20.glUniform4fv(loc, 1, a[0].m_Buffer);
            } else {
                FloatBuffer buf = FloatBuffer.allocate(nb * 4);
                for (vec4 v: a) {
                    buf.put(v.m_Cells);
                }
                GLES20.glUniform4fv(loc, nb, buf);
            }
        }
    }

    /**
     * affecte une composante
     * @param i : numéro 0..3 de la composante
     * @param v : valeur de la composante
     */
    public void set(int i, float v)
    {
        m_Cells[i] = v;
    }
    
    /**
     * accesseur pour les composantes
     * @param i : numéro 0..3 de la composante
     */
    public float get(int i)
    {
        return m_Cells[i];
    }

    /**
     * Cette méthode calcule le vec4 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
     * @param out : résultat
     * @param p0 : coordonnées à k=0
     * @param t0 : tangente à k=0
     * @param p1 : coordonnées à k=1
     * @param t1 : tangente à k=1
     * @param k : coefficient d'interpolation
     */
    public static void hermite(vec4 out, vec4 p0, vec4 t0, vec4 p1, vec4 t1, float k)
    {
        float h00 = ((2*k) - 3)*k*k + 1;
        float h10 = ((k - 2)*k + 1)*k;
        float h01 = (3 - 2*k)*k*k;
        float h11 = (k - 1)*k*k;
    
        vec4.zero(out);
        vec4.scaleAndAdd(out, out, p0, h00);
        vec4.scaleAndAdd(out, out, t0, h10);
        vec4.scaleAndAdd(out, out, p1, h01);
        vec4.scaleAndAdd(out, out, t1, h11);
    }
}
