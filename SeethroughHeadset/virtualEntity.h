#pragma once

#include "includes.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GLSLProgram.h"
#include "mesh.h"
#include "entityNode.h"

class VirtualEntity
{
private:
	const aiScene *assimpScene;

	vector<Material*> *materials;

	void loadGeometry(char* source);
	void recursiveSceneLoad(EntityNode *parent, aiNode *aiParent);

public:
	VirtualEntity(char *path);

	//viewport
	int width, height;

	EntityNode *root;

	float glowSize;
	

	//Resize Window
	void resize(int, int);
	
	//animations
	void update(double elapsed);

	~VirtualEntity(void);
};