// Cette classe permet d'appliquer un tone mapping sur un FBO
// biblio : http://www.nutty.ca/?page_id=352&link=hdr

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class ToneMapping extends Process
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("ToneMapping");

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            uniform sampler2D ColorMap;
            in vec2 frgTexCoord;

            // pour normaliser les luminances
            uniform float MaxLuminance;
            uniform float AvgLuminance;
            uniform float alpha;

            // conversion RGB vers xyY
            vec3 rgb2xyY(vec3 rgb)
            {
                const mat3 matXYZ = mat3(
                        0.4124, 0.3576, 0.1805,
                        0.2126, 0.7152, 0.0722,
                        0.0193, 0.1192, 0.9505);
                vec3 XYZ = matXYZ * rgb;
                float s = XYZ.x + XYZ.y + XYZ.z;
                return vec3(XYZ.x/s, XYZ.y/s, XYZ.y);
            }

            // conversion xyY vers RGB
            vec3 xyY2rgb(vec3 xyY)
            {
                vec3 XYZ = vec3(
                        (xyY.z / xyY.y) * xyY.x,
                        xyY.z,
                        (xyY.z / xyY.y) * (1.0 - xyY.x - xyY.y));
                const mat3 matRGB = mat3(
                         3.2406, -1.5372, -0.4986,
                        -0.9689,  1.8758,  0.0415,
                         0.0557, -0.2040,  1.0570);
                return matRGB * XYZ;
            }

            out vec4 glFragColor;

            void main()
            {
                // teinte au niveau du fragment
                vec3 Kd = texture(ColorMap, frgTexCoord).rgb;

                // conversion RGB vers xyY
                vec3 xyY = rgb2xyY(Kd);

                // luminance normalisée
                float luminance = xyY.z / AvgLuminance * alpha;

                // transformation de la luminance par la formule de Reinhard
                luminance *= (1.0 + luminance/(MaxLuminance*MaxLuminance)) / (1.0 + luminance);

                // remplacement de la luminance sans changer les couleurs
                xyY.z = luminance;

                // conversion xyY vers RGB
                Kd = xyY2rgb(xyY);

                // correction gamma
                //Kd = pow(Kd, vec3(1.0 / 2.2));

                // couleur finale du fragment
                glFragColor = vec4(Kd, 1.0);
            }`;
        return srcFragmentShader;
    }


    /**
     * détermine où sont les variables uniform spécifiques de ce traitement
     */
    findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // déterminer où sont les variables uniform spécifiques
        this.m_MaxLuminanceLoc = gl.getUniformLocation(this.m_ShaderId, "MaxLuminance");
        this.m_AvgLuminanceLoc = gl.getUniformLocation(this.m_ShaderId, "AvgLuminance");
        this.m_AlphaLoc        = gl.getUniformLocation(this.m_ShaderId, "alpha");
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à flouter
     * @param maxlum : luminosité maximale de la scène
     * @param avglum : luminosité moyenne de la scène
     * @param alpha : luminance d'un gris moyen, 0.18 par défaut
     */
    process(fbo, maxlum, avglum, alpha=0.18)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du traitement
        gl.uniform1f(this.m_MaxLuminanceLoc, maxlum);
        gl.uniform1f(this.m_AvgLuminanceLoc, avglum);
        gl.uniform1f(this.m_AlphaLoc, alpha);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

        // libérer les ressources
        this.endProcess();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
