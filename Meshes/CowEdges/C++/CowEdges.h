#ifndef COWARETES_H
#define COWARETES_H

// Définition de la fonction CowEdges

#include <gl-matrix.h>
#include <Mesh.h>
#include <Material.h>
#include <VBOset.h>


class CowEdges
{
public:

    CowEdges();
    ~CowEdges();


    /**
     * Cette méthode dessine la vache
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);


private:

    Mesh* m_Mesh;

    Material* m_TrianglesMaterial;
    Material* m_EdgesMaterial;

    VBOset* m_VBOsetTriangles;
    VBOset* m_VBOsetEdges;

};

#endif
