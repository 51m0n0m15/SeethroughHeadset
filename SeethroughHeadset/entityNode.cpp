#include "entityNode.h"


EntityNode::EntityNode(void)
{
	transform = idMat;
	animated = false;
	frame=0;
}


EntityNode::~EntityNode(void)
{
}

/**
Loads the current frame of the animation (if node is animated).
(Sets the transform matrix of a node to a matrix from the animation vector.)\n
Animation starts from the beginning once the end is reached.\n
Repeats recursively for all child nodes.
@param elapsed time since last frame, determines which frame should be loaded
*/
void EntityNode::update(double elapsed){
	if(animated){
		frame = frame+elapsed;
		if(frame>=animation.size()-1)
			frame = frame - (double)(animation.size()-1);
		
		transform = animation.at((int)frame);
	}

	for(int i=0; i<children.size(); i++){
		children.at(i)->update(elapsed);
	}
}

/**
Building the scene graph: add a child node.
*/
void EntityNode::addChild(EntityNode *n){
	children.push_back(n);
}

/**
Building the scene graph: add a Mesh to the node.
*/
void EntityNode::addMesh(Mesh *m){
	meshes.push_back(m);
}