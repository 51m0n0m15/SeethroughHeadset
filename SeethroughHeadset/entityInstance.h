#pragma once

#include "includes.h"
#include "virtualEntity.h"

class EntityInstance
{
public:
	EntityInstance(VirtualEntity *_entity);
	~EntityInstance(void);

	VirtualEntity *entity;

	void translate(float x, float y, float z);
	void setRotation(mat4 _rotation){rotation = _rotation;}

	mat4 getTransform(){return rotation*translation;}

private:
	mat4 rotation;
	mat4 translation;
};

