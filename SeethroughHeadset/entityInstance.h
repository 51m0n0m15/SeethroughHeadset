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
	void setGlobalRotation(mat4 _rotation){globalRotation = _rotation;}
	void setLocalRotation(mat4 _rotation){localRotation = _rotation*localRotation;}
	void setScale(mat4 _scale){scale = _scale;}

	mat4 getTransform(){return globalRotation*translation*localRotation*scale;}

private:
	mat4 globalRotation;
	mat4 localRotation;
	mat4 translation;
	mat4 scale;
};

