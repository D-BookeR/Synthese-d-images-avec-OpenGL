#ifndef GROUPEMENTS_H
#define GROUPEMENTS_H

// Définition de la classe PrimitiveGroups

#include <Mesh.h>
#include <DeferredShadingMaterial.h>


class PrimitiveGroups
{
public:

    /**
     * Constructeur
     */
    PrimitiveGroups();

    /**
     * fonction de dessin
     */
     void onDraw();

    /**
     * Destructeur
     */
    ~PrimitiveGroups();

private:

    GLint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_VertexBufferId;

    GLint m_ColorLoc;
    GLint m_ColorBufferId;

    GLint m_IndexBufferId;

};

#endif
