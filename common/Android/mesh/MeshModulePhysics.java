package com.dbooker.livreopengl.mesh;

import android.util.Log;

import com.dbooker.livreopengl.glmatrix.vec3;


/**
 * Ce module rajoute des fonctions pour calculer les éléments physiques
 * d'un Mesh : volume et masse, CdG, matrice d'inertie
 *
 * Biblio :  Fast and Accurate Computation of Polyhedral Mass Properties - Mirtich 1996
 * http://www.cs.berkeley.edu/~jfc/mirtich/massProps.html
 */
@SuppressWarnings("unused")
public class MeshModulePhysics extends MeshModule
{
    private float m_Density;
    private float m_Volume;
    private float m_Mass;
    private vec3 m_CoG;


    /**
     * initialise le module sans maillage
     */
    public MeshModulePhysics()
    {
        super();
        m_CoG = vec3.create();
    }


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    public MeshModulePhysics(Mesh mesh)
    {
        super(mesh);
        m_CoG = vec3.create();
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    static private float Math_SQR(float x)
    {
        return x * x;
    }

    static private float Math_CUBE(float x)
    {
        return x * x * x;
    }


    /**
     * Définit la densité du maillage
     * @param density valeur de la densité
     */
    public void setDensity(float density)
    {
        m_Density = density;
    }


    /**
     * Cette méthode calcule et affiche différentes informations physiques sur le maillage
     * Utiliser les getters pour les récupérer
     */
    public void compVolumeIntegrals()
    {
        float T0 = 0.0f;
        vec3 T1 = vec3.create();
        vec3 T2 = vec3.create();
        vec3 TP = vec3.create();

        for (MeshTriangle triangle: m_Mesh.getTriangleList()) {

            // calculer le coefficient W du triangle
            triangle.setW(-vec3.dot(triangle.getNormal(), triangle.getVertex0().getCoord()));

            // déterminer quelle permutation des axes x,y,z donne la plus grande projection du triangle
            int A,B,C;
            float x = Math.abs(triangle.getNormal().get(0));
            float y = Math.abs(triangle.getNormal().get(1));
            float z = Math.abs(triangle.getNormal().get(2));
            if (x > y && x > z) {
                // le triangle est plutôt face à X
                C = 0;
            } else if (y > z) {
                // le triangle est plutôt face à Y
                C = 1;
            } else {
                // le triangle est plutôt face à Z
                C = 2;
            }
            A = (C + 1) % 3;
            B = (A + 1) % 3;

            /// compProjectionIntegrals

            float P1 = 0.0f;
            float Pa = 0.0f;
            float Pb = 0.0f;
            float Paa = 0.0f;
            float Pab = 0.0f;
            float Pbb = 0.0f;
            float Paaa = 0.0f;
            float Paab = 0.0f;
            float Pabb = 0.0f;
            float Pbbb = 0.0f;

            for (int iv=0; iv<3; iv++) {
                float a0 = triangle.getVertex(iv).getCoord().get(A);
                float b0 = triangle.getVertex(iv).getCoord().get(B);
                float a1 = triangle.getVertex((iv+1) % 3).getCoord().get(A);
                float b1 = triangle.getVertex((iv+1) % 3).getCoord().get(B);
                float da = a1 - a0;
                float db = b1 - b0;
                float a0_2 = a0 * a0; float a0_3 = a0_2 * a0; float a0_4 = a0_3 * a0;
                float b0_2 = b0 * b0; float b0_3 = b0_2 * b0; float b0_4 = b0_3 * b0;
                float a1_2 = a1 * a1; float a1_3 = a1_2 * a1;
                float b1_2 = b1 * b1; float b1_3 = b1_2 * b1;

                float C1 = a1 + a0;
                float Ca = a1*C1 + a0_2;
                float Caa = a1*Ca + a0_3;
                float Caaa = a1*Caa + a0_4;
                float Cb = b1*(b1 + b0) + b0_2;
                float Cbb = b1*Cb + b0_3;
                float Cbbb = b1*Cbb + b0_4;
                float Cab = 3*a1_2 + 2*a1*a0 + a0_2;
                float Kab = a1_2 + 2*a1*a0 + 3*a0_2;
                float Caab = a0*Cab + 4*a1_3;
                float Kaab = a1*Kab + 4*a0_3;
                float Cabb = 4*b1_3 + 3*b1_2*b0 + 2*b1*b0_2 + b0_3;
                float Kabb = b1_3 + 2*b1_2*b0 + 3*b1*b0_2 + 4*b0_3;

                P1 += db*C1;
                Pa += db*Ca;
                Paa += db*Caa;
                Paaa += db*Caaa;
                Pb += da*Cb;
                Pbb += da*Cbb;
                Pbbb += da*Cbbb;
                Pab += db*(b1*Cab + b0*Kab);
                Paab += db*(b1*Caab + b0*Kaab);
                Pabb += da*(a1*Cabb + a0*Kabb);
            }

            P1 /= 2.0;
            Pa /= 6.0;
            Paa /= 12.0;
            Paaa /= 20.0;
            Pb /= -6.0;
            Pbb /= -12.0;
            Pbbb /= -20.0;
            Pab /= 24.0;
            Paab /= 60.0;
            Pabb /= -60.0;


            /// compFaceIntegrals

            float k1 = 1.0f / triangle.getNormal().get(C);
            float k2 = k1 * k1;
            float k3 = k2 * k1;
            float k4 = k3 * k1;

            float Fa = k1 * Pa;
            float Fb = k1 * Pb;
            float Fc = -k2 * (triangle.getNormal().get(A)*Pa + triangle.getNormal().get(B)*Pb + triangle.getW()*P1);

            float Faa = k1 * Paa;
            float Fbb = k1 * Pbb;
            float Fcc = k3 * (Math_SQR(triangle.getNormal().get(A))*Paa + 2*triangle.getNormal().get(A)*triangle.getNormal().get(B)*Pab
                    + Math_SQR(triangle.getNormal().get(B))*Pbb + triangle.getW()*(2*(triangle.getNormal().get(A)*Pa + triangle.getNormal().get(B)*Pb) + triangle.getW()*P1));

            float Faaa = k1 * Paaa;
            float Fbbb = k1 * Pbbb;
            float Fccc = -k4 * (Math_CUBE(triangle.getNormal().get(A))*Paaa + 3*Math_SQR(triangle.getNormal().get(A))*triangle.getNormal().get(B)*Paab
                    + 3*triangle.getNormal().get(A)*Math_SQR(triangle.getNormal().get(B))*Pabb + Math_CUBE(triangle.getNormal().get(B))*Pbbb
                    + 3*triangle.getW()*(Math_SQR(triangle.getNormal().get(A))*Paa + 2*triangle.getNormal().get(A)*triangle.getNormal().get(B)*Pab + Math_SQR(triangle.getNormal().get(B))*Pbb)
                    + triangle.getW()*triangle.getW()*(3*(triangle.getNormal().get(A)*Pa + triangle.getNormal().get(B)*Pb) + triangle.getW()*P1));

            float Faab = k1 * Paab;
            float Fbbc = -k2 * (triangle.getNormal().get(A)*Pabb + triangle.getNormal().get(B)*Pbbb + triangle.getW()*Pbb);
            float Fcca = k3 * (Math_SQR(triangle.getNormal().get(A))*Paaa + 2*triangle.getNormal().get(A)*triangle.getNormal().get(B)*Paab
                    + Math_SQR(triangle.getNormal().get(B))*Pabb + triangle.getW()*(2*(triangle.getNormal().get(A)*Paa + triangle.getNormal().get(B)*Pab) + triangle.getW()*Pa));

            if (A == 0) {
                T0 += triangle.getNormal().get(0) * Fa;
            } else if (B == 0) {
                T0 += triangle.getNormal().get(0) * Fb;
            } else {
                T0 += triangle.getNormal().get(0) * Fc;
            }

            vec3.set(T1, A, T1.get(A) + triangle.getNormal().get(A) * Faa);
            vec3.set(T1, B, T1.get(B) + triangle.getNormal().get(B) * Fbb);
            vec3.set(T1, C, T1.get(C) + triangle.getNormal().get(C) * Fcc);
            vec3.set(T2, A, T2.get(A) + triangle.getNormal().get(A) * Faaa);
            vec3.set(T2, B, T2.get(B) + triangle.getNormal().get(B) * Fbbb);
            vec3.set(T2, C, T2.get(C) + triangle.getNormal().get(C) * Fccc);
            vec3.set(TP, A, TP.get(A) + triangle.getNormal().get(A) * Faab);
            vec3.set(TP, B, TP.get(B) + triangle.getNormal().get(B) * Fbbc);
            vec3.set(TP, C, TP.get(C) + triangle.getNormal().get(C) * Fcca);
        }
        vec3.scale(T1, T1, 0.5f);
        vec3.scale(T2, T2, 1.0f/3.0f);
        vec3.scale(TP, TP, 0.5f);

        // volume et masse
        m_Volume = T0;
        Log.i(TAG, "volume = "+m_Volume);
        m_Mass = m_Density * T0;
        Log.i(TAG, "mass  = "+m_Mass);

        // centre de gravité
        vec3.zero(m_CoG);
        vec3.scale(m_CoG, T1, 1.0f / T0);
        Log.i(TAG, "CoG = "+vec3.str(m_CoG));
    }


    /**
     * retourne le volume du maillage
     * @return volume du maillage
     */
    public float getVolume()
    {
        return m_Volume;
    }


    /**
     * retourne la masse du maillage
     * @return masse du maillage
     */
    public float getMass()
    {
        return m_Mass;
    }


    /**
     * retourne le centre de gravité du maillage
     * @return centre de gravité du maillage
     */
    public vec3 getCoG()
    {
        return m_CoG;
    }
}
