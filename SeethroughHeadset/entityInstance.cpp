#include "entityInstance.h"


EntityInstance::EntityInstance(VirtualEntity *_entity)
{
	entity = _entity;
	globalRotation = idMat;
	localRotation = idMat;
	translation = idMat;
	scaleFactor = 1.0f;
}


EntityInstance::~EntityInstance(void)
{
}

/**
Returns the combined transformation matrix of the instance.
*/
mat4 EntityInstance::getTransform(){
	return globalRotation
		*translation
		*localRotation
		*glm::scale(scaleFactor, scaleFactor, scaleFactor);
}