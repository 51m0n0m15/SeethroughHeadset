#pragma once

#include "includes.h"
#include "entityNode.h"
#include "material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


/**
An entity, or virtual object, that can be placed somewhere in space (demo application).
Each VirtualEntity has its own scenegraph, to allow for complex animations.
A VirtualEntity is defined by one file, e.g. a .dae-file; all geometry
(and all materials) of that file belongs to the entity. \n
The Renderer currently takes a vector of EntityInstance as input for rendering; 
so, to render a VirtualEntity, it has to be assigned
to an EntityInstance and passed to the Renderer.
*/
class VirtualEntity
{
private:
	const aiScene *assimpScene;			//the asimp scene created from the input file

	vector<Material*> *materials;		//material collection

	void loadGeometry(const char* source);
	void recursiveSceneLoad(EntityNode *parent, aiNode *aiParent);

public:
	VirtualEntity(const char *path);

	EntityNode *root;				/**< root of the scenegraph */
	
	//animations
	void update(double elapsed){root->update(elapsed);}

	~VirtualEntity(void);
};