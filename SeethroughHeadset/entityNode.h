#pragma once

#include "includes.h"
#include "mesh.h"
#include "assimp\anim.h"

/**
Basic element of the scene graph of a VirtualEntity (demo application).
*/
class EntityNode
{
public:
	EntityNode(void);
	~EntityNode(void);

	void update(double elapsed);
	void addChild(EntityNode *n);
	void addMesh(Mesh *m);

	string name;						/**< Name of the node, copy of the node name in assimp. */
	mat4 transform;						/**< Current transformation. */
	vector<mat4> animation;				/**< Sequence of transformation matrices. */
	double frame;						/**< Current frame of the animation vector. 
											is stored as double, because it is updated with the elapsed time since the last frame;
											is truncated to an int to determine the actual frame index.*/
	bool animated;						/**< Flag that says if the node is animated. */

	vector<EntityNode*> children;		/**< Vector of child nodes. */
	vector<Mesh*> meshes;				/**< Vector of Mesh objects attached to this node. */

};

