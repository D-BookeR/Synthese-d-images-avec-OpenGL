// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModule.js");



function Math_SQR(x)
{
    return x * x;
}

function Math_CUBE(x)
{
    return x * x * x;
}


/**
 * Ce module rajoute des fonctions pour calculer les éléments physiques
 * d'un Mesh : volume et masse, CdG, matrice d'inertie
 *
 * Biblio :  Fast and Accurate Computation of Polyhedral Mass Properties - Mirtich 1996
 * http://www.cs.berkeley.edu/~jfc/mirtich/massProps.html
 */
class MeshModulePhysics extends MeshModule
{
    /** variables membres
    private float m_Density;
    private float m_Volume;
    private float m_Mass;
    private vec3 m_CoG = vec3.create();
    **/


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    constructor(mesh)
    {
        super(mesh);
        this.m_CoG = vec3.create();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * Définit la densité du maillage
     * @param density valeur de la densité
     */
    setDensity(density)
    {
        this.m_Density = density;
    }


    /**
     * Cette méthode calcule et affiche différentes informations physiques sur le maillage
     * Utiliser les getters pour les récupérer
     */
    compVolumeIntegrals()
    {
        let T0 = 0.0;
        let T1 = vec3.create();
        let T2 = vec3.create();
        let TP = vec3.create();

        for (let triangle of this.m_Mesh.getTriangleList()) {

            // calculer le coefficient W du triangle
            triangle.setW(-vec3.dot(triangle.getNormal(), triangle.getVertex0().getCoord()));

            // déterminer quelle permutation des axes x,y,z donne la plus grande projection du triangle
            let A,B,C;
            let x = Math.abs(triangle.getNormal()[0]);
            let y = Math.abs(triangle.getNormal()[1]);
            let z = Math.abs(triangle.getNormal()[2]);
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

            let P1 = 0.0;
            let Pa = 0.0;
            let Pb = 0.0;
            let Paa = 0.0;
            let Pab = 0.0;
            let Pbb = 0.0;
            let Paaa = 0.0;
            let Paab = 0.0;
            let Pabb = 0.0;
            let Pbbb = 0.0;

            for (let iv=0; iv<3; iv++) {
                let a0 = triangle.getVertex(iv).getCoord()[A];
                let b0 = triangle.getVertex(iv).getCoord()[B];
                let a1 = triangle.getVertex((iv+1) % 3).getCoord()[A];
                let b1 = triangle.getVertex((iv+1) % 3).getCoord()[B];
                let da = a1 - a0;
                let db = b1 - b0;
                let a0_2 = a0 * a0; let a0_3 = a0_2 * a0; let a0_4 = a0_3 * a0;
                let b0_2 = b0 * b0; let b0_3 = b0_2 * b0; let b0_4 = b0_3 * b0;
                let a1_2 = a1 * a1; let a1_3 = a1_2 * a1;
                let b1_2 = b1 * b1; let b1_3 = b1_2 * b1;

                let C1 = a1 + a0;
                let Ca = a1*C1 + a0_2;
                let Caa = a1*Ca + a0_3;
                let Caaa = a1*Caa + a0_4;
                let Cb = b1*(b1 + b0) + b0_2;
                let Cbb = b1*Cb + b0_3;
                let Cbbb = b1*Cbb + b0_4;
                let Cab = 3*a1_2 + 2*a1*a0 + a0_2;
                let Kab = a1_2 + 2*a1*a0 + 3*a0_2;
                let Caab = a0*Cab + 4*a1_3;
                let Kaab = a1*Kab + 4*a0_3;
                let Cabb = 4*b1_3 + 3*b1_2*b0 + 2*b1*b0_2 + b0_3;
                let Kabb = b1_3 + 2*b1_2*b0 + 3*b1*b0_2 + 4*b0_3;

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

            let k1 = 1.0 / triangle.getNormal()[C];
            let k2 = k1 * k1;
            let k3 = k2 * k1;
            let k4 = k3 * k1;

            let Fa = k1 * Pa;
            let Fb = k1 * Pb;
            let Fc = -k2 * (triangle.getNormal()[A]*Pa + triangle.getNormal()[B]*Pb + triangle.getW()*P1);

            let Faa = k1 * Paa;
            let Fbb = k1 * Pbb;
            let Fcc = k3 * (Math_SQR(triangle.getNormal()[A])*Paa + 2*triangle.getNormal()[A]*triangle.getNormal()[B]*Pab
                + Math_SQR(triangle.getNormal()[B])*Pbb + triangle.getW()*(2*(triangle.getNormal()[A]*Pa + triangle.getNormal()[B]*Pb) + triangle.getW()*P1));

            let Faaa = k1 * Paaa;
            let Fbbb = k1 * Pbbb;
            let Fccc = -k4 * (Math_CUBE(triangle.getNormal()[A])*Paaa + 3*Math_SQR(triangle.getNormal()[A])*triangle.getNormal()[B]*Paab
                + 3*triangle.getNormal()[A]*Math_SQR(triangle.getNormal()[B])*Pabb + Math_CUBE(triangle.getNormal()[B])*Pbbb
                + 3*triangle.getW()*(Math_SQR(triangle.getNormal()[A])*Paa + 2*triangle.getNormal()[A]*triangle.getNormal()[B]*Pab + Math_SQR(triangle.getNormal()[B])*Pbb)
                + triangle.getW()*triangle.getW()*(3*(triangle.getNormal()[A]*Pa + triangle.getNormal()[B]*Pb) + triangle.getW()*P1));

            let Faab = k1 * Paab;
            let Fbbc = -k2 * (triangle.getNormal()[A]*Pabb + triangle.getNormal()[B]*Pbbb + triangle.getW()*Pbb);
            let Fcca = k3 * (Math_SQR(triangle.getNormal()[A])*Paaa + 2*triangle.getNormal()[A]*triangle.getNormal()[B]*Paab
                + Math_SQR(triangle.getNormal()[B])*Pabb + triangle.getW()*(2*(triangle.getNormal()[A]*Paa + triangle.getNormal()[B]*Pab) + triangle.getW()*Pa));

            if (A == 0) {
                T0 += triangle.getNormal()[0] * Fa;
            } else if (B == 0) {
                T0 += triangle.getNormal()[0] * Fb;
            } else {
                T0 += triangle.getNormal()[0] * Fc;
            }

            T1[A] += triangle.getNormal()[A] * Faa;
            T1[B] += triangle.getNormal()[B] * Fbb;
            T1[C] += triangle.getNormal()[C] * Fcc;
            T2[A] += triangle.getNormal()[A] * Faaa;
            T2[B] += triangle.getNormal()[B] * Fbbb;
            T2[C] += triangle.getNormal()[C] * Fccc;
            TP[A] += triangle.getNormal()[A] * Faab;
            TP[B] += triangle.getNormal()[B] * Fbbc;
            TP[C] += triangle.getNormal()[C] * Fcca;
        }
        vec3.scale(T1, T1, 0.5);
        vec3.scale(T2, T2, 1.0/3.0);
        vec3.scale(TP, TP, 0.5);

        // volume et masse
        this.m_Volume = T0;
        console.log("volume = "+this.m_Volume);
        this.m_Mass = this.m_Density * T0;
        console.log("mass  = "+this.m_Mass);

        // centre de gravité
        vec3.zero(this.m_CoG);
        vec3.scale(this.m_CoG, T1, 1.0 / T0);
        console.log("CoG = "+vec3.str(this.m_CoG));
    }


    /**
     * retourne le volume du maillage
     * @return volume du maillage
     */
    getVolume()
    {
        return this.m_Volume;
    }


    /**
     * retourne la masse du maillage
     * @return masse du maillage
     */
    getMass()
    {
        return this.m_Mass;
    }


    /**
     * retourne le centre de gravité du maillage
     * @return centre de gravité du maillage
     */
    getCoG()
    {
        return this.m_CoG;
    }
}
