#pragma once

#include "includes.h"
#include "virtualEntity.h"


/**
Holds a pointer to a VirtualEntity and a set of transformations (demo application).
*/
class EntityInstance
{
public:
	EntityInstance(VirtualEntity *_entity);
	~EntityInstance(void);

	VirtualEntity *entity;														/**< pointer to the VirtualObject that should be instantiated */

	void translate(float x, float y, float z)
		{translation = translation * glm::translate(x,y,z);}					/**< adds an increment to the current translation */
	void setGlobalRotation(mat4 _rotation){globalRotation = _rotation;}			/**< sets the absolute global rotation */
	void localRotate(mat4 _rotation){localRotation = _rotation*localRotation;}	/**< adds an increment to the local rotation */
	void scale(float _scale){scaleFactor *= _scale;}							/**< multiplies the input parameter to the current scale 
																					(only uniform scale possible) */
	mat4 getTransform();


private:
	mat4 globalRotation;
	mat4 localRotation;
	mat4 translation;
	float scaleFactor;
};