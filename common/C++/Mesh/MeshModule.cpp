/**
 * Cette classe représente un module permettant de faire des opérations sur les maillages
 *
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>


#include <MeshModule.h>


/**
 * initialise le module sur le maillage fourni
 * @param mesh : maillage concerné
 */
MeshModule::MeshModule(Mesh* mesh)
{
    this->m_Mesh = mesh;
}


/**
 * définit le maillage à gérer
 * @param mesh : maillage concerné
 */
void MeshModule::setMesh(Mesh* mesh)
{
    this->m_Mesh = mesh;
}
