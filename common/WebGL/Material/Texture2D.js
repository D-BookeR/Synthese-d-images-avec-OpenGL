/**
 * cette classe lance le chargement d'une image et en fait une texture 2D
 * NB: le chargement est fait par une requête au serveur, donc peut prendre du temps
 * pendant lequel la texture n'est pas utilisable
 * @note il se peut qu'il y ait une erreur "It is forbidden to load a WebGL texture from a cross-domain element that has not been validated with CORS. See https://developer.mozilla.org/en/WebGL/Cross-Domain_Textures"
 */
class Texture2D
{
    /**
     * le constructeur lance le chargement d'une image et en fait une texture 2D
     * @param filename : nom du fichier contenant l'image à charger
     * @param filtering : mettre gl.LINEAR ou gl.NEAREST ou gl.LINEAR_MIPMAP_LINEAR (mipmaps)
     * @param repetition : mettre gl.CLAMP_TO_EDGE ou gl.REPEAT
     */
    constructor(filename=null, filtering=gl.LINEAR, repetition=gl.CLAMP_TO_EDGE)
    {
        this.m_TextureID = gl.createTexture();
        if (filename != null) {
            let image = new Image();
            image.crossOrigin = "anonymous";
            image.onload = function(texture, image) {
                return function() {
                    texture.m_Width = this.width;
                    texture.m_Height = this.height;

                    // activer la texture, fournir le bitmap
                    gl.bindTexture(gl.TEXTURE_2D, texture.m_TextureID);
                    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
                    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

                    // filtrage avec mipmaps ?
                    if (filtering == gl.NEAREST_MIPMAP_NEAREST || filtering == gl.LINEAR_MIPMAP_NEAREST ||
                        filtering == gl.NEAREST_MIPMAP_LINEAR  || filtering == gl.LINEAR_MIPMAP_LINEAR) {
                        gl.generateMipmap(gl.TEXTURE_2D);
                        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, filtering);
                        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
                        // activer le filtering anisotropique
                        if (filtering == gl.LINEAR_MIPMAP_LINEAR) {
                            let ext = (
                                gl.getExtension('EXT_texture_filter_anisotropic') ||
                                gl.getExtension('MOZ_EXT_texture_filter_anisotropic') ||
                                gl.getExtension('WEBKIT_EXT_texture_filter_anisotropic')
                            );
                            if (ext) {
                                let max = gl.getParameter(ext.MAX_TEXTURE_MAX_ANISOTROPY_EXT);
                                gl.texParameterf(gl.TEXTURE_2D, ext.TEXTURE_MAX_ANISOTROPY_EXT, max);
                            }
                        }
                    } else {
                        // filtering antialiasing de la texture sans mipmaps
                        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, filtering);
                        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, filtering);
                    }

                    // mode de répétition de la texture
                    if (repetition != gl.CLAMP_TO_EDGE) {
                        // test de la taille de la texture pour afficher un message d'avertissement
                        let exponent = Math.log2(this.width);
                        if (exponent - Math.floor(exponent) != 0.0) {
                            console.warn("Width of texture "+filename+" is not power of 2 ("+this.width+")");
                            repetition = gl.CLAMP_TO_EDGE;
                        }
                        exponent = Math.log2(this.height);
                        if (exponent - Math.floor(exponent) != 0.0) {
                            console.warn("Height of texture "+filename+" is not power of 2 ("+this.width+")");
                            repetition = gl.CLAMP_TO_EDGE;
                        }
                    }
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, repetition);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, repetition);

                    // réglage de OpenGL pour avoir un bon rendu des textures
                    //gl.hint(gl.PERSPECTIVE_CORRECTION_HINT, gl.NICEST);

                    // libérer l'unité de texture
                    gl.bindTexture(gl.TEXTURE_2D, null);
                }
            } (this, image);
            image.src = filename;
        } else {
            // on est dans le cas d'une sous-classe de Texture2D, alors seul l'identifiant de la texture a été créé
            gl.bindTexture(gl.TEXTURE_2D, this.m_TextureID);

            // filtering antialiasing de la texture
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, filtering);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, filtering);

            // mode de répétition de la texture
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, repetition);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, repetition);

            // réglage de OpenGL pour avoir un bon rendu des textures
            //gl.hint(gl.PERSPECTIVE_CORRECTION_HINT, gl.NICEST);
        }
    }


    /**
     * supprime cette texture
     */
    destroy()
    {
        gl.deleteTexture(this.m_TextureID);
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
            error("Texture2D.setTextureUnit: first parameter, unit is not gl.TEXTURE0 ... gl.TEXTURE7");
        }
        if (locSampler != null && locSampler.constructor !== WebGLUniformLocation) {
            error("Texture2D.setTextureUnit: second parameter, locSampler is not either null or the location of a uniform variable : "+locSampler);
        }
        /*****DEBUG*****/
        gl.activeTexture(unit);
        if (locSampler == null) {
            gl.bindTexture(gl.TEXTURE_2D, null);
        } else {
            gl.bindTexture(gl.TEXTURE_2D, this.m_TextureID);
            gl.uniform1i(locSampler, unit-gl.TEXTURE0);
        }
    }
}
