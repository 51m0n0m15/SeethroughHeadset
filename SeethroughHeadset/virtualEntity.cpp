#include "virtualEntity.h"
#include "mesh.h"


VirtualEntity::VirtualEntity(const char *path){ 


	//Load Content!
	loadGeometry(path);
}

VirtualEntity::~VirtualEntity(){
	materials->clear();
	delete materials;
}


void VirtualEntity::loadGeometry(const char* source){
	Assimp::Importer importer;
	assimpScene = importer.ReadFile(source, aiProcessPreset_TargetRealtime_Fast);

	//load materials
	materials = new vector<Material*>(assimpScene->mNumMaterials);
	for(int i=0; i<assimpScene->mNumMaterials; i++){
		materials->at(i)=new Material(assimpScene->mMaterials[i]);
	}


	root = new EntityNode();
	recursiveSceneLoad(root, assimpScene->mRootNode);

	cout << "Successfully loaded " << source << ": " << assimpScene->mNumMeshes << " meshes." << endl;
}


void VirtualEntity::recursiveSceneLoad(EntityNode *parent, aiNode *aiParent){
	parent->name = string(aiParent->mName.C_Str());
	
	//meshes of that node
	for(int i=0; i<aiParent->mNumMeshes; i++){
		Mesh* temp  = new Mesh(assimpScene->mMeshes[aiParent->mMeshes[i]],
			materials->at(assimpScene->mMeshes[aiParent->mMeshes[i]]->mMaterialIndex));
		parent->addObject(temp);
	}


	//look for animation
	for(int i=0; i<assimpScene->mNumAnimations; i++){
		for(int j=0; j<assimpScene->mAnimations[i]->mNumChannels; j++){
			if(assimpScene->mAnimations[i]->mChannels[j]->mNodeName == aiParent->mName){
				//import animation
				cout << "Animation found for " << aiParent->mName.C_Str() << ": " << 
					assimpScene->mAnimations[i]->mChannels[j]->mNumPositionKeys << " frames" << endl;
				for(int k=0; k<assimpScene->mAnimations[i]->mChannels[j]->mNumPositionKeys; k++){
					//translation
					mat4 translation = glm::translate(assimpScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x,
						assimpScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y,
						assimpScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z);

					//roatation
					float quatX = assimpScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.x;
					float quatY = assimpScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.y;
					float quatZ = assimpScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.z;
					float quatW = assimpScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.w;
					mat4 rotation = idMat;
					if(!(quatX==0 && quatY==0 && quatZ==0)){
						float rotX = atan2(2*quatX*quatW - 2*quatY*quatZ, 1 - 2*quatX*quatX - 2*quatZ*quatZ) * 180.0f / (float)M_PI;
						float rotY = atan2(2*quatY*quatW - 2*quatX*quatZ, 1 - 2*quatY*quatY - 2*quatZ*quatZ) * 180.0f / (float)M_PI;
						float rotZ = asin(2*quatX*quatY + 2*quatZ*quatW) * 180.0f / (float)M_PI;
						rotation = glm::rotate(rotation,rotZ, vec3(0.0f,0.0f,1.0f));
						rotation = glm::rotate(rotation,rotY, vec3(0.0f,1.0f,0.0f));
						rotation = glm::rotate(rotation,rotX, vec3(1.0f,0.0f,0.0f));
					}

					//scale
					mat4 scale = glm::scale(assimpScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x,
						assimpScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y,
						assimpScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z);
					//store frame as combined matrix
					parent->animation.push_back(translation*rotation*scale);
				}
				parent->animated = true;
			}
		}
	}



	//child nodes
	for(int i=0; i<aiParent->mNumChildren; i++){
		EntityNode *n = new EntityNode();
		aiMatrix4x4 mat = aiParent->mChildren[i]->mTransformation;
		n->transform = mat4(mat.a1,mat.b1,mat.c1,mat.d1,
			mat.a2,mat.b2,mat.c2,mat.d2,
			mat.a3,mat.b3,mat.c3,mat.d3,
			mat.a4,mat.b4,mat.c4,mat.d4);
		parent->addChild(n);
		recursiveSceneLoad(n,aiParent->mChildren[i]);
	}
}



void VirtualEntity::update(double elapsed){
	
	root->update(elapsed);
}