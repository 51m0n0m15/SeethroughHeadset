#pragma once

#include "includes.h"
#include "entityNode.h"
#include "material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class VirtualEntity
{
private:
	const aiScene *assimpScene;

	vector<Material*> *materials;

	void loadGeometry(const char* source);
	void recursiveSceneLoad(EntityNode *parent, aiNode *aiParent);

public:
	VirtualEntity(const char *path);

	EntityNode *root;
	
	//animations
	void update(double elapsed);

	~VirtualEntity(void);
};