#pragma once

#include "includes.h"
#include <assimp/material.h>

class Material
{
public:
	Material(aiMaterial *material);
	~Material(void);

	GLuint loadTexture(const char* fName, int textureUnit);

	vec3 colorAmbient;
	vec3 colorDiffuse;
	vec3 colorSpecular;
	float shininess;

	/*
	TEXTURE UNITS:
	0: color map
	1: normal map
	2: glow map
	*/
	GLuint colormapID;
	GLuint normalmapID;
	GLuint glowmapID;
};

