#include "entityInstance.h"


EntityInstance::EntityInstance(VirtualEntity *_entity)
{
	entity = _entity;
	globalRotation = idMat;
	localRotation = idMat;
	translation = idMat;
	scale = idMat;
}


EntityInstance::~EntityInstance(void)
{
}


void EntityInstance::translate(float x, float y, float z){
	translation = translation * glm::translate(x,y,z);
}