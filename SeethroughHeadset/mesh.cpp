#include "mesh.h"

/**
Extracts geometry data from an assimp mesh and stores them in OpenGL-friendly arrays.
@param mesh pointer to the assimp mesh
@param _material pointer to a previously created material; one material can be used by many meshes.
*/
Mesh::Mesh(aiMesh *mesh, Material *_material)
{
	numVertices = mesh->mNumVertices;
	numFaces = mesh->mNumFaces;

	vertices = new GLfloat[numVertices*4];
	normals = new GLfloat[numVertices*3];
	indices = new GLuint[numFaces*3];
	textureCoord = new GLfloat[numVertices*2];

	for(int i=0; i<numVertices; i++){
		vertices[4*i] = mesh->mVertices[i].x;
		vertices[4*i+1] = mesh->mVertices[i].y;
		vertices[4*i+2] = mesh->mVertices[i].z;
		vertices[4*i+3] = 1.0;

		normals[3*i] = mesh->mNormals[i].x;
		normals[3*i+1] = mesh->mNormals[i].y;
		normals[3*i+2] = mesh->mNormals[i].z;
	}

	for(int i=0; i<numFaces; i++){
		indices[i*3] = mesh->mFaces[i].mIndices[0];
		indices[i*3+1] = mesh->mFaces[i].mIndices[1];
		indices[i*3+2] = mesh->mFaces[i].mIndices[2];
	}

	if(mesh->HasTextureCoords(0)){

		for(int i=0; i<numVertices; i++){
			textureCoord[i*2] = mesh->mTextureCoords[0][i].x;
			textureCoord[i*2+1] = mesh->mTextureCoords[0][i].y;
		}
	}
	material = _material;
}


Mesh::~Mesh(void)
{
	delete[] vertices;
	delete[] indices;
	delete[] normals;
	delete[] textureCoord;
}