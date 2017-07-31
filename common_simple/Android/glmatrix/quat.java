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
* @class Quaternion
* @name quat
*/
@SuppressWarnings("unused")
public class quat
{
    protected FloatBuffer m_Buffer = FloatBuffer.allocate(4);
    protected float[] m_Cells = m_Buffer.array();

    /**
    * Creates a new identity quat
    *
    * @return a new quaternion
    */
    public static quat create()
    {
        quat out = new quat();
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 1;
        return out;
    }

    /**
    * Sets a quaternion to represent the shortest rotation from one
    * vector to another.
    *
    * Both vectors are assumed to be unit length.
    *
    * @param out the receiving quaternion.
    * @param a the initial vector
    * @param b the destination vector
    * @return out
    */
    public static quat rotationTo(quat out, vec3 a, vec3 b)
    {
        vec3 tmpvec3 = vec3.create();
        final vec3 xUnitVec3 = vec3.fromValues(1,0,0);
        final vec3 yUnitVec3 = vec3.fromValues(0,1,0);

        float dot = vec3.dot(a, b);
        if (dot < -0.999999) {
            vec3.cross(tmpvec3, xUnitVec3, a);
            if (vec3.length(tmpvec3) < 0.000001)
            vec3.cross(tmpvec3, yUnitVec3, a);
            vec3.normalize(tmpvec3, tmpvec3);
            quat.setAxisAngle(out, tmpvec3, (float) Math.PI);
            return out;
        } else if (dot > 0.999999) {
            out.m_Cells[0] = 0;
            out.m_Cells[1] = 0;
            out.m_Cells[2] = 0;
            out.m_Cells[3] = 1;
            return out;
        } else {
            vec3.cross(tmpvec3, a, b);
            out.m_Cells[0] = tmpvec3.m_Cells[0];
            out.m_Cells[1] = tmpvec3.m_Cells[1];
            out.m_Cells[2] = tmpvec3.m_Cells[2];
            out.m_Cells[3] = 1 + dot;
            return normalize(out, out);
        }
    }

    /**
    * Sets the specified quaternion with values corresponding to the given
    * axes. Each axis is a vec3 and is expected to be unit length and
    * perpendicular to all other specified axes.
    *
    * @param view  the vector representing the viewing direction
    * @param right the vector representing the local "right" direction
    * @param up    the vector representing the local "up" direction
    * @return out
    */
    public static quat setAxes(vec3 view, vec3 right, vec3 up)
    {
        quat out = new quat();
        mat3 matr = mat3.create();

        matr.m_Cells[0] = right.m_Cells[0];
        matr.m_Cells[3] = right.m_Cells[1];
        matr.m_Cells[6] = right.m_Cells[2];

        matr.m_Cells[1] = up.m_Cells[0];
        matr.m_Cells[4] = up.m_Cells[1];
        matr.m_Cells[7] = up.m_Cells[2];

        matr.m_Cells[2] = -view.m_Cells[0];
        matr.m_Cells[5] = -view.m_Cells[1];
        matr.m_Cells[8] = -view.m_Cells[2];

        quat.fromMat3(out, matr);
        return normalize(out, out);
    }

    /**
    * Creates a new quat initialized with values from an existing quaternion
    *
    * @param a quaternion to clone
    * @return a new quaternion
    */
    public static quat clone(quat a)
    {
        quat out = new quat();
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }

    /**
    * Creates a new quat initialized with the given values
    *
    * @param x X component
    * @param y Y component
    * @param z Z component
    * @param w W component
    * @return a new quaternion
    */
    public static quat fromValues(float x, float y, float z, float w)
    {
        quat out = new quat();
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        out.m_Cells[2] = z;
        out.m_Cells[3] = w;
        return out;
    }

    /**
    * Copy the values from one quat to another
    *
    * @param out the receiving quaternion
    * @param a the source quaternion
    * @return out
    */
    public static quat copy(quat out, quat a)
    {
        out.m_Cells[0] = a.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }
    /**
    * Set the components of a quat to the given values
    *
    * @param out the receiving quaternion
    * @param x X component
    * @param y Y component
    * @param z Z component
    * @param w W component
    * @return out
    */
    public static quat set(quat out, float x, float y, float z, float w)
    {
        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        out.m_Cells[2] = z;
        out.m_Cells[3] = w;
        return out;
    }

    /**
    * Set one of the components of a quat to the given values
    *
    * @param out the receiving quaternion
    * @param c number 0..3 of component
    * @param v value of component
    * @return out
    */
    public static quat set(quat out, int c, float v)
    {
        out.m_Cells[c] = v;
        return out;
    }

    /**
    * Get one of the components of a quat
    *
    * @param in the vector
    * @param c number 0..1 of component
    * @return value of component
    */
    public static float get(quat in, int c)
    {
        return in.m_Cells[c];
    }

    /**
    * Set a quat to the identity quaternion
    *
    * @param out the receiving quaternion
    * @return out
    */
    public static quat identity(quat out)
    {
        out.m_Cells[0] = 0;
        out.m_Cells[1] = 0;
        out.m_Cells[2] = 0;
        out.m_Cells[3] = 1;
        return out;
    }

    /**
    * Sets a quat from the given angle and rotation axis,
    * then returns it.
    *
    * @param out the receiving quaternion
    * @param axis the axis around which to rotate
    * @param rad the angle in radians
    * @return out
    **/
    public static quat setAxisAngle(quat out, vec3 axis, float rad)
    {
        rad = rad * 0.5f;
        float s = (float) Math.sin(rad);
        out.m_Cells[0] = s * axis.m_Cells[0];
        out.m_Cells[1] = s * axis.m_Cells[1];
        out.m_Cells[2] = s * axis.m_Cells[2];
        out.m_Cells[3] = (float) Math.cos(rad);
        return out;
    }

    /**
    * Adds two quat's
    *
    * @param out the receiving quaternion
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static quat add(quat out, quat a, quat b)
    {
        out.m_Cells[0] = a.m_Cells[0] + b.m_Cells[0];
        out.m_Cells[1] = a.m_Cells[1] + b.m_Cells[1];
        out.m_Cells[2] = a.m_Cells[2] + b.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3] + b.m_Cells[3];
        return out;
    }

    /**
    * Multiplies two quat's
    *
    * @param out the receiving quaternion
    * @param a the first operand
    * @param b the second operand
    * @return out
    */
    public static quat multiply(quat out, quat a, quat b)
    {
        float ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
        bx = b.m_Cells[0], by = b.m_Cells[1], bz = b.m_Cells[2], bw = b.m_Cells[3];

        out.m_Cells[0] = ax * bw + aw * bx + ay * bz - az * by;
        out.m_Cells[1] = ay * bw + aw * by + az * bx - ax * bz;
        out.m_Cells[2] = az * bw + aw * bz + ax * by - ay * bx;
        out.m_Cells[3] = aw * bw - ax * bx - ay * by - az * bz;
        return out;
    }

    /**
    * Scales a quat by a scalar number
    *
    * @param out the receiving vector
    * @param a the vector to scale
    * @param scale amount to scale the vector by
    * @return out
    */
    public static quat scale(quat out, quat a, float scale)
    {
        out.m_Cells[0] = a.m_Cells[0] * scale;
        out.m_Cells[1] = a.m_Cells[1] * scale;
        out.m_Cells[2] = a.m_Cells[2] * scale;
        out.m_Cells[3] = a.m_Cells[3] * scale;
        return out;
    }

    /**
    * Rotates a quaternion by the given angle about the X axis
    *
    * @param out quat receiving operation result
    * @param a quat to rotate
    * @param rad angle (in radians) to rotate
    * @return out
    */
    public static quat rotateX(quat out, quat a, float rad)
    {
        rad *= 0.5;

        float ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
        bx = (float) Math.sin(rad), bw = (float) Math.cos(rad);

        out.m_Cells[0] = ax * bw + aw * bx;
        out.m_Cells[1] = ay * bw + az * bx;
        out.m_Cells[2] = az * bw - ay * bx;
        out.m_Cells[3] = aw * bw - ax * bx;
        return out;
    }

    /**
    * Rotates a quaternion by the given angle about the Y axis
    *
    * @param out quat receiving operation result
    * @param a quat to rotate
    * @param rad angle (in radians) to rotate
    * @return out
    */
    public static quat rotateY(quat out, quat a, float rad)
    {
        rad *= 0.5;

        float ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
        by = (float) Math.sin(rad), bw = (float) Math.cos(rad);

        out.m_Cells[0] = ax * bw - az * by;
        out.m_Cells[1] = ay * bw + aw * by;
        out.m_Cells[2] = az * bw + ax * by;
        out.m_Cells[3] = aw * bw - ay * by;
        return out;
    }

    /**
    * Rotates a quaternion by the given angle about the Z axis
    *
    * @param out quat receiving operation result
    * @param a quat to rotate
    * @param rad angle (in radians) to rotate
    * @return out
    */
    public static quat rotateZ(quat out, quat a, float rad)
    {
        rad *= 0.5;

        float ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
        bz = (float) Math.sin(rad), bw = (float) Math.cos(rad);

        out.m_Cells[0] = ax * bw + ay * bz;
        out.m_Cells[1] = ay * bw - ax * bz;
        out.m_Cells[2] = az * bw + aw * bz;
        out.m_Cells[3] = aw * bw - az * bz;
        return out;
    }

    /**
    * Calculates the W component of a quat from the X, Y, and Z components.
    * Assumes that quaternion is 1 unit in length.
    * Any existing W component will be ignored.
    *
    * @param out the receiving quaternion
    * @param a quat to calculate W component of
    * @return out
    */
    public static quat calculateW(quat out, quat a)
    {
        float x = a.m_Cells[0], y = a.m_Cells[1], z = a.m_Cells[2];

        out.m_Cells[0] = x;
        out.m_Cells[1] = y;
        out.m_Cells[2] = z;
        out.m_Cells[3] = (float) Math.sqrt(Math.abs(1.0 - x * x - y * y - z * z));
        return out;
    }

    /**
    * Calculates the dot product of two quat's
    *
    * @param a the first operand
    * @param b the second operand
    * @return dot product of a and b
    */
    public static float dot(quat a, quat b)
    {
        return a.m_Cells[0] * b.m_Cells[0] + a.m_Cells[1] * b.m_Cells[1] + a.m_Cells[2] * b.m_Cells[2] + a.m_Cells[3] * b.m_Cells[3];
    }

    /**
    * Performs a linear interpolation between two quat's
    *
    * @param out the receiving quaternion
    * @param a the first operand
    * @param b the second operand
    * @param t interpolation amount between the two inputs
    * @return out
    */
    public static quat lerp(quat out, quat a, quat b, float t)
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
    * Performs a spherical linear interpolation between two quat
    *
    * @param out the receiving quaternion
    * @param a the first operand
    * @param b the second operand
    * @param t interpolation amount between the two inputs
    * @return out
    */
    public static quat slerp(quat out, quat a, quat b, float t)
    {
        // benchmarks:
        //    http://jsperf.com/quaternion-slerp-implementations

        float ax = a.m_Cells[0], ay = a.m_Cells[1], az = a.m_Cells[2], aw = a.m_Cells[3],
        bx = b.m_Cells[0], by = b.m_Cells[1], bz = b.m_Cells[2], bw = b.m_Cells[3];

        float omega, cosom, sinom, scale0, scale1;

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
            omega  = (float) Math.acos(cosom);
            sinom  = (float) Math.sin(omega);
            scale0 = (float) (Math.sin((1.0 - t) * omega) / sinom);
            scale1 = (float) (Math.sin(t * omega) / sinom);
        } else {
            // "from" and "to" quaternions are very close
            //  ... so we can do a linear interpolation
            scale0 = (float) (1.0 - t);
            scale1 = t;
        }
        // calculate final values
        out.m_Cells[0] = scale0 * ax + scale1 * bx;
        out.m_Cells[1] = scale0 * ay + scale1 * by;
        out.m_Cells[2] = scale0 * az + scale1 * bz;
        out.m_Cells[3] = scale0 * aw + scale1 * bw;

        return out;
    }

    /**
    * Calculates the inverse of a quat
    *
    * @param out the receiving quaternion
    * @param a quat to calculate inverse of
    * @return out
    */
    public static quat invert(quat out, quat a)
    {
        float a0 = a.m_Cells[0], a1 = a.m_Cells[1], a2 = a.m_Cells[2], a3 = a.m_Cells[3],
        dot = a0*a0 + a1*a1 + a2*a2 + a3*a3,
        invDot = (dot != 0.0f) ? 1.0f/dot : 0.0f;

        // TODO: Would be faster to return [0,0,0,0] immediately if dot == 0

        out.m_Cells[0] = -a0*invDot;
        out.m_Cells[1] = -a1*invDot;
        out.m_Cells[2] = -a2*invDot;
        out.m_Cells[3] = a3*invDot;
        return out;
    }

    /**
    * Calculates the conjugate of a quat
    * If the quaternion is normalized, this function is faster than quat.inverse and produces the same result.
    *
    * @param out the receiving quaternion
    * @param a quat to calculate conjugate of
    * @return out
    */
    public static quat conjugate(quat out, quat a)
    {
        out.m_Cells[0] = -a.m_Cells[0];
        out.m_Cells[1] = -a.m_Cells[1];
        out.m_Cells[2] = -a.m_Cells[2];
        out.m_Cells[3] = a.m_Cells[3];
        return out;
    }

    /**
    * Calculates the length of a quat
    *
    * @param a vector to calculate length of
    * @return length of a
    */
    public static float length(quat a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2],
        w = a.m_Cells[3];
        return (float)Math.sqrt((double)(x*x + y*y + z*z + w*w));
    }

    /**
    * Calculates the squared length of a quat
    *
    * @param a vector to calculate squared length of
    * @return squared length of a
    */
    public static float squaredLength(quat a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2],
        w = a.m_Cells[3];
        return x*x + y*y + z*z + w*w;
    }

    /**
    * Normalize a quat
    *
    * @param out the receiving quaternion
    * @param a quaternion to normalize
    * @return out
    */
    public static quat normalize(quat out, quat a)
    {
        float x = a.m_Cells[0],
        y = a.m_Cells[1],
        z = a.m_Cells[2],
        w = a.m_Cells[3];
        float len = x*x + y*y + z*z + w*w;
        if (len > 0) {
            len = 1.0f / (float)Math.sqrt((double)len);
            out.m_Cells[0] = a.m_Cells[0] * len;
            out.m_Cells[1] = a.m_Cells[1] * len;
            out.m_Cells[2] = a.m_Cells[2] * len;
            out.m_Cells[3] = a.m_Cells[3] * len;
        }
        return out;
    }

    /**
    * Creates a quaternion from the given 3x3 rotation matrix.
    *
    * NOTE: The resultant quaternion is not normalized, so you should be sure
    * to renormalize the quaternion yourself where necessary.
    *
    * @param out the receiving quaternion
    * @param m rotation matrix
    * @return out
    */
    public static quat fromMat3(quat out, mat3 m)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".
        float fTrace = m.m_Cells[0] + m.m_Cells[4] + m.m_Cells[8];
        float fRoot;

        if ( fTrace > 0.0 ) {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = (float)Math.sqrt(fTrace + 1.0);  // 2w
            out.m_Cells[3] = 0.5f * fRoot;
            fRoot = 0.5f/fRoot;  // 1/(4w)
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

            fRoot = (float)Math.sqrt(m.m_Cells[i*3+i]-m.m_Cells[j*3+j]-m.m_Cells[k*3+k] + 1.0);
            out.m_Cells[i] = 0.5f * fRoot;
            fRoot = 0.5f / fRoot;
            out.m_Cells[3] = (m.m_Cells[j*3+k] - m.m_Cells[k*3+j]) * fRoot;
            out.m_Cells[j] = (m.m_Cells[j*3+i] + m.m_Cells[i*3+j]) * fRoot;
            out.m_Cells[k] = (m.m_Cells[k*3+i] + m.m_Cells[i*3+k]) * fRoot;
        }

        return out;
    }

    /**
    * Returns a string representation of a quatenion
    *
    * @param a quaternion to represent as a string
    * @return string representation of the vector
    */
    public static String str(quat a)
    {
        return "quat(" + a.m_Cells[0] + ", " + a.m_Cells[1] + ", " + a.m_Cells[2] + ", " + a.m_Cells[3] + ")";
    }
}
