#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <math.h>

#include <utils.h>

#include <CubeOK.h>
#include <DeferredShadingMaterial.h>


CubeOK::CubeOK()
{
    m_Mesh = new Mesh("CubeOK");
    vec3 color = vec3::fromValues(0.2, 0.5, 0.3);

    /// dessus

    MeshVertex* s1a = m_Mesh->addVertex("s1a");
    MeshVertex* s2a = m_Mesh->addVertex("s2a");
    MeshVertex* s3a = m_Mesh->addVertex("s3a");
    MeshVertex* s4a = m_Mesh->addVertex("s4a");
    m_Mesh->addQuad(s1a, s2a, s3a, s4a);
    s1a->setCoord(vec3::fromValues(-1,+1,+1)); s1a->setColor(color);
    s2a->setCoord(vec3::fromValues(+1,+1,+1)); s2a->setColor(color);
    s3a->setCoord(vec3::fromValues(+1,+1,-1)); s3a->setColor(color);
    s4a->setCoord(vec3::fromValues(-1,+1,-1)); s4a->setColor(color);

    /// dessous

    MeshVertex* s1b = m_Mesh->addVertex("s1b");
    MeshVertex* s2b = m_Mesh->addVertex("s2b");
    MeshVertex* s3b = m_Mesh->addVertex("s3b");
    MeshVertex* s4b = m_Mesh->addVertex("s4b");
    m_Mesh->addQuad(s4b, s3b, s2b, s1b);
    s1b->setCoord(vec3::fromValues(-1,-1,+1)); s1b->setColor(color);
    s2b->setCoord(vec3::fromValues(+1,-1,+1)); s2b->setColor(color);
    s3b->setCoord(vec3::fromValues(+1,-1,-1)); s3b->setColor(color);
    s4b->setCoord(vec3::fromValues(-1,-1,-1)); s4b->setColor(color);

    /// devant

    MeshVertex* s1c = m_Mesh->addVertex("s1c");
    MeshVertex* s2c = m_Mesh->addVertex("s2c");
    MeshVertex* s3c = m_Mesh->addVertex("s3c");
    MeshVertex* s4c = m_Mesh->addVertex("s4c");
    m_Mesh->addQuad(s1c, s2c, s3c, s4c);
    s1c->setCoord(vec3::fromValues(-1,-1,+1)); s1c->setColor(color);
    s2c->setCoord(vec3::fromValues(+1,-1,+1)); s2c->setColor(color);
    s3c->setCoord(vec3::fromValues(+1,+1,+1)); s3c->setColor(color);
    s4c->setCoord(vec3::fromValues(-1,+1,+1)); s4c->setColor(color);

    /// derrière

    MeshVertex* s1d = m_Mesh->addVertex("s1d");
    MeshVertex* s2d = m_Mesh->addVertex("s2d");
    MeshVertex* s3d = m_Mesh->addVertex("s3d");
    MeshVertex* s4d = m_Mesh->addVertex("s4d");
    m_Mesh->addQuad(s4d, s3d, s2d, s1d);
    s1d->setCoord(vec3::fromValues(-1,-1,-1)); s1d->setColor(color);
    s2d->setCoord(vec3::fromValues(+1,-1,-1)); s2d->setColor(color);
    s3d->setCoord(vec3::fromValues(+1,+1,-1)); s3d->setColor(color);
    s4d->setCoord(vec3::fromValues(-1,+1,-1)); s4d->setColor(color);

    /// droit

    MeshVertex* s1e = m_Mesh->addVertex("s1e");
    MeshVertex* s2e = m_Mesh->addVertex("s2e");
    MeshVertex* s3e = m_Mesh->addVertex("s3e");
    MeshVertex* s4e = m_Mesh->addVertex("s4e");
    m_Mesh->addQuad(s1e, s2e, s3e, s4e);
    s1e->setCoord(vec3::fromValues(+1,-1,+1)); s1e->setColor(color);
    s2e->setCoord(vec3::fromValues(+1,-1,-1)); s2e->setColor(color);
    s3e->setCoord(vec3::fromValues(+1,+1,-1)); s3e->setColor(color);
    s4e->setCoord(vec3::fromValues(+1,+1,+1)); s4e->setColor(color);

    /// gauche

    MeshVertex* s1f = m_Mesh->addVertex("s1f");
    MeshVertex* s2f = m_Mesh->addVertex("s2f");
    MeshVertex* s3f = m_Mesh->addVertex("s3f");
    MeshVertex* s4f = m_Mesh->addVertex("s4f");
    m_Mesh->addQuad(s4f, s3f, s2f, s1f);
    s1f->setCoord(vec3::fromValues(-1,-1,+1)); s1f->setColor(color);
    s2f->setCoord(vec3::fromValues(-1,-1,-1)); s2f->setColor(color);
    s3f->setCoord(vec3::fromValues(-1,+1,-1)); s3f->setColor(color);
    s4f->setCoord(vec3::fromValues(-1,+1,+1)); s4f->setColor(color);

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode supprime les ressources allouées
 */
CubeOK::~CubeOK()
{
    delete m_Mesh;
}
