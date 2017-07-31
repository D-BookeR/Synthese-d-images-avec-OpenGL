#ifndef APPLEMATERIAL_H
#define APPLEMATERIAL_H

// Définition de la classe AppleMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>



class AppleMaterial: public Material
{
public:

    /**
     * constructeur
     */
    AppleMaterial();

    /** destructeur */
    ~AppleMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();


protected:

    std::string getVertexShader();
    std::string getFragmentShader();


private:
    /** identifiants liés au shader */
    GLint m_MatPloc;
    GLint m_MatMVloc;

};

#endif
