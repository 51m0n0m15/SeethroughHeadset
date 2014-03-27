#include "material.h"

Material::Material(aiMaterial *material)
{
	aiColor3D color;
	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
	colorAmbient = vec3(color.r,color.g,color.b);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	colorDiffuse = vec3(color.r,color.g,color.b);
	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	colorSpecular = vec3(color.r,color.g,color.b);
	material->Get(AI_MATKEY_SHININESS, shininess);
	material->Get(AI_MATKEY_OPACITY, opacity);
	cout << "opacity: " << opacity << endl;
	cout << "shininess: " << shininess << endl;
	
	//load textures:
	if(material->GetTextureCount(aiTextureType_DIFFUSE)>0){
		aiString path;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE,0), path);
		colormapID = ContentLoader::loadTexture(path.C_Str(),0);
	} else 
		colormapID=0;
	
	if(material->GetTextureCount(aiTextureType_SPECULAR)>0){    //as maya doesn't export normal maps, we use specular map channel
		aiString path;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR,0), path);
		normalmapID = ContentLoader::loadTexture(path.C_Str(),1);
	} else 
		normalmapID=0;

	if(material->GetTextureCount(aiTextureType_LIGHTMAP)>0){    //store glowmap to ambient texture in maya, assimp puts it in lightmap. (??)
		aiString path;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP,0), path);
		glowmapID = ContentLoader::loadTexture(path.C_Str(),2);
		//as it is impossible now to set the ambient color for this object in maya, we assign a default value
		colorAmbient = vec3(0.5f,0.5f,0.5f);
	} else 
		glowmapID=0;
}


Material::~Material(void)
{
}