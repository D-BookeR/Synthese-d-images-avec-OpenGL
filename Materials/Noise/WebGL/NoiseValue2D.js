// Définition de la classe NoiseValue2D

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");



/**
 * retourne la valeur p(x) interpolée par une cubique entre les deux points fournis
 * @param p0 valeur de la fonction en x=0
 * @param p1 valeur de la fonction en x=1
 * @param x variant entre 0 et 1
 * @return valeur interpolée de la fonction en x
 */
function CubicInterpolation(p0, p1, x)
{
//    let k = x * x * (3 - 2 * x);  // version cubique
    let k = ((6*x - 15) * x + 10) * x * x * x;  // version quintique
    return (1.0-k)*p0 + k*p1;
}

/**
 * retourne la valeur p(x,y) interpolée par une bi-cubique
 * @param p tableau de 2*2 valeurs de la fonction
 * @param x variant entre 0 et 1
 * @param y variant entre 0 et 1
 * @return valeur de la fonction en (x,y)
 */
function BicubicInterpolation(p, x, y)
{
    // interpoler les valeurs des lignes
    let p0 = CubicInterpolation(p[0][0], p[0][1], x);
    let p1 = CubicInterpolation(p[1][0], p[1][1], x);
    // interpoler en colonne
    return CubicInterpolation(p0, p1, y);
}




/**
 * cette classe gère un tableau de réels [-1, +1[ aléatoires
 */
class RawNoise2D
{
    /** constructeur
     * @param width largeur du tableau
     * @param height hauteur du tableau
     */
    constructor(width, height)
    {
        // dimensions du bruit
        this.m_Width = Math.floor(width);
        this.m_Height = Math.floor(height);

        // tableau de réels [-1, +1] aléatoires
        this.m_Data = new Float32Array(width * height);
        for (let y=0; y<this.m_Height; y++) {
            for (let x=0; x<this.m_Width; x++) {
                this.m_Data[x + y*this.m_Width] = Math.random() * 2.0 - 1.0;
            }
        }

        // tableau intermédiaire pour l'interpolation
        this.m_Tab2x2 = [new Float32Array(2), new Float32Array(2)];
    }


    /**
     * retourne la valeur qui est dans le tableau en (x,y)
     * @param x on considère la partie entière de x
     * @param y on considère la partie entière de y
     * @return this.m_Data en (x,y)
     */
    getRawNoise(x, y)
    {
        // arrondir x à l'entier inférieur valide
        let x0 = Math.floor(x);
        if (x0 < 0) x0 = 0;
        if (x0 >= this.m_Width) x0 = this.m_Width-1;

        // arrondir y à l'entier inférieur valide
        let y0 = Math.floor(y);
        if (y0 < 0) y0 = 0;
        if (y0 >= this.m_Height) y0 = this.m_Height-1;

        // retourner la valeur en ce point
        return this.m_Data[x0 + y0*this.m_Width];
    }


    /**
     * retourne la valeur interpolée en (x,y)
     * @param x
     * @param y
     * @return l'interpolation bicubique
     */
    getSmoothNoise(x, y)
    {
        // les 4 points autour de (x,y)
        let x0 = Math.floor(x);
        let y0 = Math.floor(y);
        // remplir le tableau des 2x2 valeurs entourant (x,y)
        for (let dy=0; dy<2; dy++) {
            for (let dx=0; dx<2; dx++) {
                this.m_Tab2x2[dy][dx] = this.getRawNoise(x0+dx, y0+dy);
            }
        }
        // interpolation bicubique entre ces valeurs
        return BicubicInterpolation(this.m_Tab2x2, x-x0, y-y0);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Data = null;
    }
}



/**
 * Cette classe dérive de Texture2D
 * Elle génère une texture aléatoire de type « Value Noise »
 * voir https://en.wikipedia.org/wiki/Value_noise
 * Ce sont des fonctions aléatoires superposées et interpolées
 * de manière bicubique.
 */
class NoiseValue2D extends Texture2D
{
    /** constructeur
     * @param width largeur de la texture
     * @param height hauteur de la texture
     * @param octaves : nombre de superpositions de bruits bicubiques
     */
    constructor(width, height, octaves)
    {
        // test sur le paramètre
        if (octaves < 1) throw "NoiseValue2D: incorrect octave number (less than 1)";

        // construire la texture mais ne pas charger de fichier
        super(null);

        // générer le pixmap de la texture
        let pixmap = new Uint8Array(width * height);
        for (let y=0; y<height; y++) {
            for (let x=0; x<width; x++) {
                pixmap[y*width + x] = 0;
            }
        }

        // déterminer la fréquence spatiale et l'amplitude du premier octave
        let frequence = 1 << (octaves-1);
        if (width/frequence < 2) throw "NoiseValue2D: too many octaves for width ("+octaves+" for "+width+" pixels)";
        if (height/frequence < 2) throw "NoiseValue2D: too many octaves for height ("+octaves+" for "+height+" pixels)";
        let amplitude = 128.0 / (2.0-1.0/frequence);

        // ajouter les bruits
        for (let octave=0; octave<octaves; octave++) {

            // créer un bruit de cette fréquence
            let noise = new RawNoise2D(width/frequence, height/frequence);

            // ajouter les valeurs lissées
            for (let y=0; y<height; y++) {
                for (let x=0; x<width; x++) {
                    let value = amplitude * (noise.getSmoothNoise(x/frequence, y/frequence) + 1.0);
                    pixmap[y*width + x] = Utils.clamp(pixmap[y*width + x] + value, 0, 255);
                }
            }

            // diminuer l'amplitude et la fréquence spatiale
            amplitude = amplitude * 0.5;
            frequence = frequence / 2;

            // libérer le bruit
            noise.destroy();
        }

        // fournir le pixmap à la texture
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.LUMINANCE, width, height, 0, gl.LUMINANCE, gl.UNSIGNED_BYTE, pixmap);

        // libérer l'unité de texture
        gl.bindTexture(gl.TEXTURE_2D, null);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
