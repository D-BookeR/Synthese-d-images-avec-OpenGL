#ifndef PRIMITIVERESTART_H
#define PRIMITIVERESTART_H

// Définition de la classe PrimitiveRestart

#include <Mesh.h>
#include <DeferredShadingMaterial.h>


class PrimitiveRestart
{
public:

    /**
     * Constructeur
     */
    PrimitiveRestart();

    /**
     * fonction de dessin
     */
     void onDraw();

    /**
     * Destructeur
     */
    ~PrimitiveRestart();

private:

    GLint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_VertexBufferId;

    GLint m_ColorLoc;
    GLint m_ColorBufferId;

    GLint m_IndexBufferId;
    GLint m_IndexBufferSize;

};

#endif
