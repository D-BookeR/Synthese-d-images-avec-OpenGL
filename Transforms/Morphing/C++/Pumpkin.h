#ifndef PUMPKIN_H
#define PUMPKIN_H

// Définition de la classe Pumpkin

#include <map>

#include <gl-matrix.h>
#include <SceneElement.h>
#include <Mesh.h>
#include <VBOset.h>

#include "MorphMaterial.h"


class Pumpkin: public Drawable
{
public:

    /**
     * Constructeur
     */
    Pumpkin();

    /** Destructeur */
    ~Pumpkin();

    /**
     * définit la valeur du morph pour changer la forme de l'objet
     * @param coef entre 0 et 1
     */
    void setMorphCoef(float coef);

    /**
     * dessine l'objet
     * @param mat4Projection
     * @param mat4ModelView
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

private:

    // dictionnaire des matériaux (nom_matériau, matériau)
    std::map<std::string, MorphMaterial*> m_Materials;

    // dictionnaire des VBOsets (nom_matériau, VBOset)
    std::map<std::string, VBOset*> m_VBOsets;
};

#endif
