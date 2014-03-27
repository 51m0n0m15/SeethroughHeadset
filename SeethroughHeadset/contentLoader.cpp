#include "contentLoader.h"
#include <il.h>	

ContentLoader::ContentLoader(void)
{
}


ContentLoader::~ContentLoader(void)
{
}

GLuint ContentLoader::loadTexture(const char* fName, int textureUnit)
{
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION){
		printf("wrong DevIL-Version.\n");	
		return 1;
	}

	// SET UP DevIL and his origin
	ilInit();
	ilEnable(IL_ORIGIN_SET);


	ILboolean loaded;
	ILuint w, h, format, texture_ID_il;
	GLuint texID;

	// Textures - DevIL
	ilGenImages(1, &texture_ID_il);		/* Generate IL-ID for textures */

	ilBindImage(texture_ID_il);					/* Binding of image name */
	loaded = ilLoadImage((wchar_t*)fName);	/* Loading of image */	
	
	if(loaded == IL_FALSE){
		printf("DevIL - ERROR Loading file %s!\n", fName);
		return 0;
	}
	
	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	format = ilGetInteger(IL_IMAGE_FORMAT);
	//printf("DevIL: the image resolution is: %d, %d\n", w, h);
	
	if (loaded) {
		loaded = ilConvertImage(format, IL_UNSIGNED_BYTE);	/* Convert every colour component into unsigned byte. */
		if (!loaded){
			printf("DevIL - ERROR converting Image %s!\n", fName);
			return 0;
		}
		
		glActiveTexture(GL_TEXTURE0+textureUnit);						/* Activate texture unit*/
		glGenTextures(1, &texID);										/* Texture name generation */
		glBindTexture(GL_TEXTURE_2D, texID);							/* Binding of texture name */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	/* We will use linear interpolation for magnification filter */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	//mipmapping for min filter
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);	//enable mipmapgeneration
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), w, h, 0, format, GL_UNSIGNED_BYTE, ilGetData());
	}

	 ilDeleteImages(1, &texture_ID_il); /* Because we have already copied image data into texture data we can release memory used by image. */
	 printf("Successfully Created Texture: %s! with ID: %i\n", fName, texID);
	 return texID;
}