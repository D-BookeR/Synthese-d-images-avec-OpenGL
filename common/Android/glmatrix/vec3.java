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
* @class 3 Dimensional Vector
* @name vec3
*/
@SuppressWarnings("unused")
public class vec3
{
    protected FloatBuffer m_Buffer = FloatBuffer.allocate(3);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new, empty vec3
    *
    * @return a new 3D vector
    */
    public static vec3 create()
    {
        vec3 out = new vec3();
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        return out;
    }

    public static vec3 zero(vec3 out)
    {
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        return out;
    }

    /**
    * Creates a new vec3 initialized with values from an existing vector
    *
    * @param a vector to clone
    * @return a new 3D vector
    */
    public static vec3 clone(vec3 a)
    {
        vec3 out = new vec3();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        return out;
    }

    /**
    * Creates a new vec3 initialized with the given values
    *
    * @param x X component
    * @param y Y component
    * @param z Z component
    * @return a new 3D vector
    */
    public static vec3 fromValues(float x, float y, float z)
    {
        vec3 out = new vec3();
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        out.m_Cells[2] = z;
        return out;
    }
    public static vec3 fromValues(double x, double y, double z)
    {
        vec3 out = new vec3();
        out.m_Cells[0] = (float) x;
        out.m_Cells[1] = (float) y;
        out.m_Cells[2] = (float) z;
        return out;
    }

    /**
     * Creates a new vec3 casted from the given vec
     *
     * @param a vector
     * @return a new 3D vector
     */
    public static vec3 cast(vec4 a)
    {
        vec3 out = new vec3();
        out.m_Cells = a.m_Cells;
        return out;
    }

    /**
     * Creates a new vec3 initialized from the given vec
     *
     * @param a vector
     * @return a new 3D vector
     */
    public static vec3 fromVec(vec2 a)
    {
        vec3 out = new vec3();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = 0;
        return out;
    }
    public static vec3 fromVec(vec3 a)
    {
        vec3 out = new vec3();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        return out;
    }
    public static vec3 fromVec(vec4 a)
    {
        vec3 out = new vec3();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        return out;
    }

    /**
    * Copy the values from one vec3 to another
    *
    * @param out the receiving vector
    * @param a the source vector
    * @return out
    */
    public static vec3 copy(vec3 out, vec3 a)
    {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        return out;
    }

    /**
    * Set the components of a vec3 to the given values
    *
    * @param out the receiving vector
    * @param x X component
    * @param y Y component
    * @param z Z component
    * @return out
    */
    public static vec3 set(vec3 out, float x, float y, float z)
    {
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        out.m_Cells[2] = z;
        return out;
    }

    /**
    * Set one of the components of a vec3 to the given values
    *
    * @param out the receiving vector
    * @param c number 0..2 of component
    * @param v value of component
    * @return out
    */
    public static vec3 set(vec3 out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a vec3
    *
    * @param in the vector
    * @param c number 0..2 of component
    * @return value of component
    */
    public static float get(vec3 in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Adds two vec3's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec3 add(vec3 out, vec3 a, vec3 b)
    {
        out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
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
    public static vec3 subtract(vec3 out, vec3 a, vec3 b)
    {
        out.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];
        return out;
    }

    /**
    * Multiplies two vec3's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec3 multiply(vec3 out, vec3 a, vec3 b)
    {
        out.m_Cells[0] = a.m_Cells[0] * b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] * b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] * b.m_Cells[2];
        return out;
    }

    /**
    * Divides two vec3's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec3 divide(vec3 out, vec3 a, vec3 b)
    {
        out.m_Cells[0] = a.m_Cells[0] / b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] / b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] / b.m_Cells[2];
        return out;
    }

    /**
    * Returns the minimum of two vec3's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec3 min(vec3 out, vec3 a, vec3 b)
    {
        out.m_Cells[0] = Math.min(a.m_Cells[0], b.m_Cells[0]);
        out.m_Cells[1] = Math.min(a.m_Cells[1], b.m_Cells[1]);
        out.m_Cells[2] = Math.min(a.m_Cells[2], b.m_Cells[2]);
        return out;
    }

    /**
    * Returns the maximum of two vec3's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec3 max(vec3 out, vec3 a, vec3 b)
    {
        out.m_Cells[0] = Math.max(a.m_Cells[0], b.m_Cells[0]);
        out.m_Cells[1] = Math.max(a.m_Cells[1], b.m_Cells[1]);
        out.m_Cells[2] = Math.max(a.m_Cells[2], b.m_Cells[2]);
        return out;
    }

    /**
    * Scales a vec3 by a scalar number
    *
    * @param out the receiving vector
    * @param a the vector to scale
    * @param b amount to scale the vector by
    * @return out
    */
    public static vec3 scale(vec3 out, vec3 a, float b)
    {
        out.m_Cells[0] = a.m_Cells[0] * b;
        out.m_Cells[1] = a.m_Cells[1] * b;
        out.m_Cells[2] = a.m_Cells[2] * b;
        return out;
    }

    /**
    * Adds two vec3's after scaling the second operand by a scalar value
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @param scale the amount to scale b by before adding
    * @return out
    */
    public static vec3 scaleAndAdd(vec3 out, vec3 a, vec3 b, float scale)
    {
        out.m_Cells[0] = a.m_Cells[0] + (b.m_Cells[0] * scale);
        out.m_Cells[1] = a.m_Cells[1] + (b.m_Cells[1] * scale);
        out.m_Cells[2] = a.m_Cells[2] + (b.m_Cells[2] * scale);
        return out;
    }

    /**
    * Calculates the euclidian distance between two vec3's
    *
    * @param a the first operand
    * @param b the second operand
    * @return distance between a and b
    */
    public static float distance(vec3 a, vec3 b)
    {
        float x = b.m_Cells[0] - a.m_Cells[0],
        y = b.m_Cells[1] - a.m_Cells[1],
        z = b.m_Cells[2] - a.m_Cells[2];
        return (float)Math.sqrt((double)(x*x + y*y + z*z));
    }

    /**
    * Calculates the squared euclidian distance between two vec3's
    *
    * @param a the first operand
    * @param b the second operand
    * @return squared distance between a and b
    */
    public static float squaredDistance(vec3 a, vec3 b)
    {
        float x = b.m_Cells[0] - a.m_Cells[0],
        y = b.m_Cells[1] - a.m_Cells[1],
        z = b.m_Cells[2] - a.m_Cells[2];
        return x*x + y*y + z*z;
    }

    /**
    * Calculates the length of a vec3
    *
    * @param a vector to calculate length of
    * @return length of a
    */
    public static float length(vec3 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2];
        return (float)Math.sqrt((double)(x*x + y*y + z*z));
    }

    /**
    * Calculates the squared length of a vec3
    *
    * @param a vector to calculate squared length of
    * @return squared length of a
    */
    public static float squaredLength(vec3 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2];
        return x*x + y*y + z*z;
    }

    /**
    * Negates the components of a vec3
    *
    * @param out the receiving vector
    * @param a vector to negate
    * @return out
    */
    public static vec3 negate(vec3 out, vec3 a)
    {
        out.m_Cells[0] = -a.m_Cells[0];
        out.m_Cells[1] = -a.m_Cells[1];
        out.m_Cells[2] = -a.m_Cells[2];
        return out;
    }

    /**
     * Returns the inverse of the components of a vec3
     *
     * @param out the receiving vector
     * @param a vector to invert
     * @return out
     */
    public static vec3 inverse(vec3 out, vec3 a)
    {
        out.m_Cells[0] = 1.0f / a.m_Cells[0];
        out.m_Cells[1] = 1.0f / a.m_Cells[1];
        out.m_Cells[2] = 1.0f / a.m_Cells[2];
        return out;
    }

    /**
    * Normalize a vec3
    *
    * @param out the receiving vector
    * @param a vector to normalize
    * @return out
    */
    public static vec3 normalize(vec3 out, vec3 a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2];
        float len = x*x + y*y + z*z;
        if (len > 0) {
            //TODO: evaluate use of glm_invsqrt here?
            len = 1 / (float)Math.sqrt((double)len);
            out.m_Cells[0] = a.m_Cells[0] * len;
            out.m_Cells[1] = a.m_Cells[1] * len;
            out.m_Cells[2] = a.m_Cells[2] * len;
        }
        return out;
    }

    /**
    * Calculates the dot product of two vec3's
    *
    * @param a the first operand
    * @param b the second operand
    * @return dot product of a and b
    */
    public static float dot(vec3 a, vec3 b)
    {
        return a.m_Cells[0] * b.m_Cells[0] + a.m_Cells[1] * b.m_Cells[1] + a.m_Cells[2] * b.m_Cells[2];
    }

    /**
    * Computes the cross product of two vec3's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static vec3 cross(vec3 out, vec3 a, vec3 b)
    {
        float ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2],
        bx = b.m_Cells[0], by = b.m_Cells[1], bz = b.m_Cells[2];

        out.m_Cells[0] = ay * bz - az * by;
        out.m_Cells[1] = az * bx - ax * bz;
        out.m_Cells[2] = ax * by - ay * bx;
        return out;
    }

    /**
    * Performs a linear interpolation between two vec3's
    *
    * @param out the receiving vector
    * @param a the first operand
    * @param b the second operand
    * @param t interpolation amount between the two inputs
    * @return out
    */
    public static vec3 lerp(vec3 out, vec3 a, vec3 b, float t)
    {
        float ax = a.m_Cells[0],
        ay = a.m_Cells[1],
        az = a.m_Cells[2];
        out.m_Cells[0] = ax + t * (b.m_Cells[0] - ax);
        out.m_Cells[1] = ay + t * (b.m_Cells[1] - ay);
        out.m_Cells[2] = az + t * (b.m_Cells[2] - az);
        return out;
    }

    /**
    * Generates a random vector with the given scale
    *
    * @param out the receiving vector
    * @param scale Length of the resulting vector. If ommitted, a unit vector will be returned
    * @return out
    */
    public static vec3 random(vec3 out, float scale)
    {
        float r = (float)(Math.random() * 2.0 * Math.PI);
        float z = (float)((Math.random() * 2.0) - 1.0);
        float zScale = (float)(Math.sqrt(1.0-z*z) * scale);

        out.m_Cells[0] = (float) (Math.cos(r) * zScale);
        out.m_Cells[1] = (float) (Math.sin(r) * zScale);
        out.m_Cells[2] = z * scale;
        return out;
    }

    /**
    * Transforms the vec3 with a mat4.
    * 4th vector component is implicitly '1'
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param m matrix to transform with
    * @return out
    */
    public static vec3 transformMat4(vec3 out, vec3 a, mat4 m)
    {
        float x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2];
        float w = m.m_Cells[3] * x + m.m_Cells[7] * y + m.m_Cells[11] * z + m.m_Cells[15];
        if (w == 0.0) w = 1.0f;
        out.m_Cells[0] = (m.m_Cells[0] * x + m.m_Cells[4] * y + m.m_Cells[8] * z + m.m_Cells[12]) / w;
        out.m_Cells[1] = (m.m_Cells[1] * x + m.m_Cells[5] * y + m.m_Cells[9] * z + m.m_Cells[13]) / w;
        out.m_Cells[2] = (m.m_Cells[2] * x + m.m_Cells[6] * y + m.m_Cells[10] * z + m.m_Cells[14]) / w;
        return out;
    }

    /**
    * Transforms the vec3 with a mat3.
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param m the 3x3 matrix to transform with
    * @return out
    */
    public static vec3 transformMat3(vec3 out, vec3 a, mat3 m)
    {
        float x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2];
        out.m_Cells[0] = x * m.m_Cells[0] + y * m.m_Cells[3] + z * m.m_Cells[6];
        out.m_Cells[1] = x * m.m_Cells[1] + y * m.m_Cells[4] + z * m.m_Cells[7];
        out.m_Cells[2] = x * m.m_Cells[2] + y * m.m_Cells[5] + z * m.m_Cells[8];
        return out;
    }

    /**
    * Transforms the vec3 with a quat
    *
    * @param out the receiving vector
    * @param a the vector to transform
    * @param q quaternion to transform with
    * @return out
    */
    public static vec3 transformQuat(vec3 out, vec3 a, quat q)
    {
        // benchmarks: http://jsperf.com/quaternion-transform-vec3-implementations

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
        return out;
    }

    /**
    * Rotate a 3D vector around the x-axis
    * @param out The receiving vec3
    * @param a The vec3 point to rotate
    * @param b The origin of the rotation
    * @param c The angle of rotation
    * @return out
    */
    public static vec3 rotateX(vec3 out, vec3 a, vec3 b, float c)
    {
        vec3 p = new vec3(), r = new vec3();
        //Translate point to the origin
        p.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
        p.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
        p.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];

        //perform rotation
        r.m_Cells[0] = p.m_Cells[0];
        r.m_Cells[1] = (float) (p.m_Cells[1]*Math.cos(c) - p.m_Cells[2]*Math.sin(c));
        r.m_Cells[2] = (float) (p.m_Cells[1]*Math.sin(c) + p.m_Cells[2]*Math.cos(c));

        //translate to correct position
        out.m_Cells[0] = r.m_Cells[0] + b.m_Cells[0];
        out.m_Cells[1] = r.m_Cells[1] + b.m_Cells[1];
        out.m_Cells[2] = r.m_Cells[2] + b.m_Cells[2];

        return out;
    }

    /**
    * Rotate a 3D vector around the y-axis
    * @param out The receiving vec3
    * @param a The vec3 point to rotate
    * @param b The origin of the rotation
    * @param c The angle of rotation
    * @return out
    */
    public static vec3 rotateY(vec3 out, vec3 a, vec3 b, float c)
    {
        vec3 p = new vec3(), r = new vec3();
        //Translate point to the origin
        p.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
        p.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
        p.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];

        //perform rotation
        r.m_Cells[0] = (float) (p.m_Cells[2]*Math.sin(c) + p.m_Cells[0]*Math.cos(c));
        r.m_Cells[1] = p.m_Cells[1];
        r.m_Cells[2] = (float) (p.m_Cells[2]*Math.cos(c) - p.m_Cells[0]*Math.sin(c));

        //translate to correct position
        out.m_Cells[0] = r.m_Cells[0] + b.m_Cells[0];
        out.m_Cells[1] = r.m_Cells[1] + b.m_Cells[1];
        out.m_Cells[2] = r.m_Cells[2] + b.m_Cells[2];

        return out;
    }

    /**
    * Rotate a 3D vector around the z-axis
    * @param out The receiving vec3
    * @param a The vec3 point to rotate
    * @param b The origin of the rotation
    * @param c The angle of rotation
    * @return out
    */
    public static vec3 rotateZ(vec3 out, vec3 a, vec3 b, float c)
    {
        vec3 p = new vec3(), r = new vec3();
        //Translate point to the origin
        p.m_Cells[0] = a.m_Cells[0] - b.m_Cells[0];
        p.m_Cells[1] = a.m_Cells[1] - b.m_Cells[1];
        p.m_Cells[2] = a.m_Cells[2] - b.m_Cells[2];

        //perform rotation
        r.m_Cells[0] = (float) (p.m_Cells[0]*Math.cos(c) - p.m_Cells[1]*Math.sin(c));
        r.m_Cells[1] = (float) (p.m_Cells[0]*Math.sin(c) + p.m_Cells[1]*Math.cos(c));
        r.m_Cells[2] = p.m_Cells[2];

        //translate to correct position
        out.m_Cells[0] = r.m_Cells[0] + b.m_Cells[0];
        out.m_Cells[1] = r.m_Cells[1] + b.m_Cells[1];
        out.m_Cells[2] = r.m_Cells[2] + b.m_Cells[2];

        return out;
    }

    /**
    * Returns a string representation of a vector
    *
    * @param a vector to represent as a string
    * @return string representation of the vector
    */
    public static String str(vec3 a)
    {
        return "vec3(" + a.m_Cells[0] + ", " + a.m_Cells[1] + ", " + a.m_Cells[2] + ")";
    }

    /**
    * calls GLES20.glUniform3fv for the vector a
    *
    * @param loc the location of the vector
    * @param a the vector to send to OpenGL shader
    */
    public static void glUniform(int loc, vec3 a)
    {
        if (loc >= 0) GLES20.glUniform3fv(loc, 1, a.m_Buffer);
    }
    /**
     * calls glUniform3fv for the vector array a
     *
     * @param loc the location of the vector
     * @param a the array of vectors to send to OpenGL shader
     */
    public static void glUniform(int loc, vec3[] a)
    {
        if (loc >= 0) {
            final int nb = a.length;
            if (nb == 1) {
                GLES20.glUniform3fv(loc, 1, a[0].m_Buffer);
            } else {
                FloatBuffer buf = FloatBuffer.allocate(nb * 3);
                for (vec3 v: a) {
                    buf.put(v.m_Cells);
                }
                GLES20.glUniform3fv(loc, nb, buf);
            }
        }
    }

    /**
     * affecte une composante
     * @param i : numéro 0..2 de la composante
     * @param v : valeur de la composante
     */
    public void set(int i, float v)
    {
        m_Cells[i] = v;
    }

    /**
     * accesseur pour les composantes
     * @param i : numéro 0..2 de la composante
     */
    public float get(int i)
    {
        return m_Cells[i];
    }

    /**
     * Cette méthode calcule le vec3 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
     * @param out : résultat
     * @param p0 : coordonnées à k=0
     * @param t0 : tangente à k=0
     * @param p1 : coordonnées à k=1
     * @param t1 : tangente à k=1
     * @param k : coefficient d'interpolation
     */
    public static void hermite(vec3 out, vec3 p0, vec3 t0, vec3 p1, vec3 t1, float k)
    {
        float h00 = ((2*k) - 3)*k*k + 1;
        float h10 = ((k - 2)*k + 1)*k;
        float h01 = (3 - 2*k)*k*k;
        float h11 = (k - 1)*k*k;

        vec3.zero(out);
        vec3.scaleAndAdd(out, out, p0, h00);
        vec3.scaleAndAdd(out, out, t0, h10);
        vec3.scaleAndAdd(out, out, p1, h01);
        vec3.scaleAndAdd(out, out, t1, h11);
    }
}
