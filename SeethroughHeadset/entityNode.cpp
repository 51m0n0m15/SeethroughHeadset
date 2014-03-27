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

void EntityNode::update(double elapsed){
	if(animated){
		frame = frame+elapsed;
		if(frame>=animation.size()-1)
			frame=0;
		
		transform = animation.at((int)frame);
		
	}

	for(int i=0; i<children.size(); i++){
		children.at(i)->update(elapsed);
	}

	
}


void EntityNode::addChild(EntityNode *n){
	children.push_back(n);
}

void EntityNode::addObject(Mesh *m){
	meshes.push_back(m);
}