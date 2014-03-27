#pragma once

#include "includes.h"
#include <assimp/mesh.h>
#include "material.h"

class Mesh
{
private:

public:
	Mesh(aiMesh *mesh, Material *_material);
	~Mesh(void);

	//geometry
	GLfloat* vertices;
	GLuint* indices;
	GLfloat* normals;
	GLfloat* textureCoord;

	//material
	Material *material;

	//info
	int numVertices;
	int numFaces;

	//vao handle
	GLuint vao;

};

