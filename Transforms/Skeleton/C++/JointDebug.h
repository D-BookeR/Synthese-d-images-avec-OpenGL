#ifndef JOINTUREDEBUG_H
#define JOINTUREDEBUG_H

// Définition de la classe JointDebug

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include "Joint.h"


class JointDebug: public Joint
{
public:
    /**
     * Constructeur
     * @param nom de la jointure
     * @param parent jointure parente
     * @param os indique s'il faut dessiner l'os ou les poids
     */
    JointDebug(std::string nom, Joint* parent, bool os=true);

    /**
     * Cette méthode supprime les ressources allouées
     */
    ~JointDebug();

    /**
     * dessine une représentation de la jointure
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    virtual void onDraw(mat4& mat4Projection, mat4& mat4View);


protected:

    /** pour dessiner la jointure */
    GLuint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_ColorLoc;
    GLint m_MatProjectionLoc;
    GLint m_MatModelViewLoc;
    GLint m_VertexBufferId;
    GLint m_ColorBufferId;
    int m_TriangleCount;
    bool m_IsBone;

};

#endif
