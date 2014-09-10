#pragma once

#include "includes.h"
#include <assimp/material.h>

/**
Holds material properties and texture handles (demo application).
In particular:\n
- ambient color\n
- diffuse color\n
- specular color\n
- specular coefficient (phong illumination model)\n
- color map\n
- normal map
*/
class Material
{
public:
	Material(aiMaterial *material);
	~Material(void);

	static GLuint loadTexture(const char* fName, int textureUnit);

	vec3 colorAmbient;
	vec3 colorDiffuse;
	vec3 colorSpecular;
	float shininess;

	/*
	TEXTURE UNITS:
	0: color map
	1: normal map
	*/
	GLuint colormapID;		/**< texture handle to color map */
	GLuint normalmapID;		/**< texture handle to normal map */
};

