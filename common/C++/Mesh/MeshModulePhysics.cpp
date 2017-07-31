// Compléments à la classe Maillage

/**
 * Ce module rajoute des fonctions pour calculer les éléments physiques
 * d'un Maillage : volume et masse, CdG, matrice d'inertie
 *
 * Biblio :  Fast and Accurate Computation of Polyhedral Mass Properties - Mirtich 1996
 * http://www.cs.berkeley.edu/~jfc/mirtich/massProps.html
 */

#include <iostream>
#include <math.h>

#include <gl-matrix.h>

#include <MeshModulePhysics.h>


static float Math_SQR(float x)
{
    return x * x;
}

static float Math_CUBE(float x)
{
    return x * x * x;
}


/**
 * initialise le module sur le maillage fourni
 * @param mesh : maillage concerné
 */
MeshModulePhysics::MeshModulePhysics(Mesh* mesh): MeshModule(mesh)
{
    m_Mass = 0.0;
    m_Density = 1.0;
    m_Volume = 0.0;
    m_CoG = vec3::create();
}


/**
 * Définit la densité du maillage
 * @param density valeur de la densité
 */
void MeshModulePhysics::setDensity(float density)
{
    m_Density = density;
}


/**
 * Cette méthode calcule et affiche différentes informations physiques sur le maillage
 * Utiliser les getters pour les récupérer
 */
void MeshModulePhysics::compVolumeIntegrals()
{
    float T0 = 0.0;
    vec3 T1 = vec3::create();
    vec3 T2 = vec3::create();
    vec3 TP = vec3::create();

    for (MeshTriangle* triangle: m_Mesh->getTriangleList()) {

        // calculer le coefficient W du triangle
        triangle->setW(-vec3::dot(triangle->getNormal(), triangle->getVertex0()->getCoord()));

        // déterminer quelle permutation des axes x,y,z donne la plus grande projection du triangle
        int A,B,C;
        float x = fabs(triangle->getNormal()[0]);
        float y = fabs(triangle->getNormal()[1]);
        float z = fabs(triangle->getNormal()[2]);
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

        float P1 = 0.0;
        float Pa = 0.0;
        float Pb = 0.0;
        float Paa = 0.0;
        float Pab = 0.0;
        float Pbb = 0.0;
        float Paaa = 0.0;
        float Paab = 0.0;
        float Pabb = 0.0;
        float Pbbb = 0.0;

        for (int iv=0; iv<3; iv++) {
            float a0 = triangle->getVertex(iv)->getCoord()[A];
            float b0 = triangle->getVertex(iv)->getCoord()[B];
            float a1 = triangle->getVertex((iv+1) % 3)->getCoord()[A];
            float b1 = triangle->getVertex((iv+1) % 3)->getCoord()[B];
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

        float k1 = 1.0 / triangle->getNormal()[C];
        float k2 = k1 * k1;
        float k3 = k2 * k1;
        float k4 = k3 * k1;

        float Fa = k1 * Pa;
        float Fb = k1 * Pb;
        float Fc = -k2 * (triangle->getNormal()[A]*Pa + triangle->getNormal()[B]*Pb + triangle->getW()*P1);

        float Faa = k1 * Paa;
        float Fbb = k1 * Pbb;
        float Fcc = k3 * (Math_SQR(triangle->getNormal()[A])*Paa + 2*triangle->getNormal()[A]*triangle->getNormal()[B]*Pab
            + Math_SQR(triangle->getNormal()[B])*Pbb + triangle->getW()*(2*(triangle->getNormal()[A]*Pa + triangle->getNormal()[B]*Pb) + triangle->getW()*P1));

        float Faaa = k1 * Paaa;
        float Fbbb = k1 * Pbbb;
        float Fccc = -k4 * (Math_CUBE(triangle->getNormal()[A])*Paaa + 3*Math_SQR(triangle->getNormal()[A])*triangle->getNormal()[B]*Paab
            + 3*triangle->getNormal()[A]*Math_SQR(triangle->getNormal()[B])*Pabb + Math_CUBE(triangle->getNormal()[B])*Pbbb
            + 3*triangle->getW()*(Math_SQR(triangle->getNormal()[A])*Paa + 2*triangle->getNormal()[A]*triangle->getNormal()[B]*Pab + Math_SQR(triangle->getNormal()[B])*Pbb)
            + triangle->getW()*triangle->getW()*(3*(triangle->getNormal()[A]*Pa + triangle->getNormal()[B]*Pb) + triangle->getW()*P1));

        float Faab = k1 * Paab;
        float Fbbc = -k2 * (triangle->getNormal()[A]*Pabb + triangle->getNormal()[B]*Pbbb + triangle->getW()*Pbb);
        float Fcca = k3 * (Math_SQR(triangle->getNormal()[A])*Paaa + 2*triangle->getNormal()[A]*triangle->getNormal()[B]*Paab
            + Math_SQR(triangle->getNormal()[B])*Pabb + triangle->getW()*(2*(triangle->getNormal()[A]*Paa + triangle->getNormal()[B]*Pab) + triangle->getW()*Pa));

        if (A == 0) {
            T0 += triangle->getNormal()[0] * Fa;
        } else if (B == 0) {
            T0 += triangle->getNormal()[0] * Fb;
        } else {
            T0 += triangle->getNormal()[0] * Fc;
        }

        T1[A] += triangle->getNormal()[A] * Faa;
        T1[B] += triangle->getNormal()[B] * Fbb;
        T1[C] += triangle->getNormal()[C] * Fcc;
        T2[A] += triangle->getNormal()[A] * Faaa;
        T2[B] += triangle->getNormal()[B] * Fbbb;
        T2[C] += triangle->getNormal()[C] * Fccc;
        TP[A] += triangle->getNormal()[A] * Faab;
        TP[B] += triangle->getNormal()[B] * Fbbc;
        TP[C] += triangle->getNormal()[C] * Fcca;
    }
    vec3::scale(T1, T1, 0.5);
    vec3::scale(T2, T2, 1.0/3.0);
    vec3::scale(TP, TP, 0.5);

    // volume et masse
    m_Volume = T0;
    std::cout << "volume = " << m_Volume << std::endl;
    m_Mass = m_Density * T0;
    std::cout << "mass  = " << m_Mass << std::endl;

    // centre de gravité
    vec3::zero(m_CoG);
    vec3::scale(m_CoG, T1, 1.0 / T0);
    std::cout << "CoG   = " << vec3::str(m_CoG) << std::endl;
}


/**
 * retourne le volume du maillage
 * @return volume du maillage
 */
float MeshModulePhysics::getVolume()
{
    return m_Volume;
}


/**
 * retourne la masse du maillage
 * @return masse du maillage
 */
float MeshModulePhysics::getMass()
{
    return m_Mass;
}


/**
 * retourne le centre de gravité du maillage
 * @return centre de gravité du maillage
 */
vec3 MeshModulePhysics::getCoG()
{
    return m_CoG;
}
