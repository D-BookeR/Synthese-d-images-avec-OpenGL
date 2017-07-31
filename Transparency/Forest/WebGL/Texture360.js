// Définition de la classe Texture360

Requires("libs/Material/Texture2D.js");


/** crée un ensemble de Texture2D pour un billboard */
class Texture360
{
    /**
 * définit un ensemble de textures pour un billboard
     * @param base : nom complet de la base servant à créer les noms des images. On y rajoute le numéro et .png
     * @param number : nombre de textures à charger. Les numéros vont de 1 à ce nombre.
     * @param filtering : mettre gl.LINEAR par exemple, c'est le filtrage des textures
     */
    constructor(base, number, filtering=gl.LINEAR_MIPMAP_LINEAR)
    {
        this.m_TexturesNumber = number;
        this.m_Textures = [];
        if (number == 1) {
            this.m_Textures.push(new Texture2D(base, filtering, gl.CLAMP_TO_EDGE));
        } else {
            for (let i=0; i<number; i++) {
                // construire le nom du fichier .png à partir de la base et du numéro
                let image_filename = base+"_"+(i+1)+".png";
                this.m_Textures.push(new Texture2D(image_filename, filtering, gl.CLAMP_TO_EDGE));
            }
        }
    }


    /**
     * retourne la Texture2D correspondant à l'angle
     * @param angle : float entre 0 et 1
     * @return texture webgl correspondant
     */
    select(angle)
    {
        if (this.m_TexturesNumber <= 1) {
            return this.m_Textures[0];
        } else {
            let n = Math.floor(angle * this.m_TexturesNumber);
            return this.m_Textures[n];
        }
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        for (let texture of this.m_Textures) {
            texture.destroy();
        }
    }
}
