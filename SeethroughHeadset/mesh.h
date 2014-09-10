#pragma once

#include "includes.h"
#include <assimp/mesh.h>
#include "material.h"

/**
Stores geometry, material and vertex array object handle of a mesh (demo application).
*/
class Mesh
{
public:
	Mesh(aiMesh *mesh, Material *_material);
	~Mesh(void);

	//geometry
	GLfloat* vertices;			/**< vertex coordinates. size=numVertices*4\n structure: v0.x,v0.y,v0.z,v0.w, v1.x,v1.y,... */
	GLuint* indices;			/**< face indices. size=numFaces*3\n structure: f0.v0,f0.v1,f0.v2, f1.v0,f1.v1,... */
	GLfloat* normals;			/**< vertex normals. size=numVertices*3\n structure: v0.nx,v0.ny,v0.nz, v1.nx,v1.ny,... */
	GLfloat* textureCoord;		/**<  texture coordinates. size=numVertices*2\n structure: v0.u,v0.v, v1.u,v1.v,... */

	//material
	Material *material;			/**< pointer to the Material assigned to the mesh */

	//info
	int numVertices;			/**< number of vertices, copied from assimp mesh */
	int numFaces;				/**< nunber of faces, copied from assimp mesh */

	//vao handle
	GLuint vao;					/**< the vertex array object handle */
};

