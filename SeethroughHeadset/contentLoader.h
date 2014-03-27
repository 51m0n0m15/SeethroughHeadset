#pragma once
#include "includes.h"

class ContentLoader
{
public:
	ContentLoader(void);
	~ContentLoader(void);

	static GLuint loadTexture(const char* fName, int textureUnit);


};