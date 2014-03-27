#pragma once

#include "includes.h"
#include "mesh.h"
#include "assimp\anim.h"

class EntityNode
{
public:
	EntityNode(void);
	~EntityNode(void);

	void update(double elapsed);
	void addChild(EntityNode *n);
	void addObject(Mesh *m);

	string name;
	mat4 transform;
	vector<mat4> animation;
	double frame;
	bool animated;

	vector<EntityNode*> children;
	vector<Mesh*> meshes;
};

