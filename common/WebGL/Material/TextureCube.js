/**
 * cette classe effectue le chargement d'un groupe de 6 images et en fait une texture cube
 * NB: le chargement est fait par une requête au serveur, donc peut prendre du temps
 * pendant lequel la texture n'est pas utilisable. La texture n'est utilisable que lorsque
 * les 6 images ont toutes été chargées.
 * @note il se peut qu'il y ait une erreur "It is forbidden to load a WebGL texture from a cross-domain element that has not been validated with CORS. See https://developer.mozilla.org/en/WebGL/Cross-Domain_Textures"
 * @param chemin : chemin terminé par un / indiquant un répertoire dans lequel on trouve 6 images : {pos|neg}[xyz].jpg
 */

class TextureCube
{
    constructor(dirname, filtering=gl.LINEAR)
    {
        // créer et sélectionner la texture Cube
        this.m_TextureID = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_CUBE_MAP, this.m_TextureID);

        // filtrage antialiasing de la texture
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MIN_FILTER, filtering);
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MAG_FILTER, filtering);

        // mode de répétition de la texture
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

        // réglage de OpenGL pour avoir un bon rendu des textures
        //gl.hint(gl.PERSPECTIVE_CORRECTION_HINT, gl.NICEST);

        // charger les 6 images, chacune sur une face
        this.loadImageFace(dirname+"/posx.jpg", gl.TEXTURE_CUBE_MAP_POSITIVE_X);
        this.loadImageFace(dirname+"/negx.jpg", gl.TEXTURE_CUBE_MAP_NEGATIVE_X);
        this.loadImageFace(dirname+"/posy.jpg", gl.TEXTURE_CUBE_MAP_POSITIVE_Y);
        this.loadImageFace(dirname+"/negy.jpg", gl.TEXTURE_CUBE_MAP_NEGATIVE_Y);
        this.loadImageFace(dirname+"/posz.jpg", gl.TEXTURE_CUBE_MAP_POSITIVE_Z);
        this.loadImageFace(dirname+"/negz.jpg", gl.TEXTURE_CUBE_MAP_NEGATIVE_Z);

        // libérer l'unité de texture
        gl.bindTexture(gl.TEXTURE_CUBE_MAP, null);
    }


    /**
     * Cette méthode charge l'image nomface associée à la face identifiée par idface
     * @param face_name : nom complet du fichier contenant l'image
     * @param idface : identifiant OpenGL de la face, ex : gl.TEXTURE_CUBE_MAP_POSITIVE_X
     */
    loadImageFace(face_name, idface)
    {
        let image = new Image();
        image.crossOrigin = "Anonymous";
        image.onload = function(idtexture, idface, image) {
            return function() {
                gl.bindTexture(gl.TEXTURE_CUBE_MAP, idtexture);
                gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, false);
                gl.texImage2D(idface, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
                gl.bindTexture(gl.TEXTURE_CUBE_MAP, null);
            }
        } (this.m_TextureID, idface, image);
        image.src = face_name;
    }



    /**
     * cette fonction associe cette texture à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple gl.TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou null pour désactiver la texture
     */
    setTextureUnit(unit, locSampler)
    {
        /*****DEBUG*****/
        if (unit < gl.TEXTURE0 || unit > gl.TEXTURE7) {
            error("TextureCube.setTextureUnit: first parameter, unit is not gl.TEXTURE0 ... gl.TEXTURE7");
        }
        if (locSampler != null && locSampler.constructor !== WebGLUniformLocation) {
            error("TextureCube.setTextureUnit: second parameter, locSampler is not either null or the location of a uniform variable : "+locSampler);
        }
        /*****DEBUG*****/
        gl.activeTexture(unit);
        if (locSampler == null) {
            gl.bindTexture(gl.TEXTURE_CUBE_MAP, null);
        } else {
            gl.bindTexture(gl.TEXTURE_CUBE_MAP, this.m_TextureID);
            gl.uniform1i(locSampler, unit-gl.TEXTURE0);
        }
    }


    /**
     * supprime cette texture
     */
    destroy()
    {
        gl.deleteTexture(this.m_TextureID);
    }
}
