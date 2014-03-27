#pragma once

/*
Texture unit convention:
0: color map
1: normal map
2: glow map
*/

#include "virtualEntity.h"
#include "includes.h"
#include "GLSLProgram.h"
#include "contentLoader.h"
#include "camManager.h"
#include "riftManager.h"


enum RenderPass{PHONG,
				GLOW
};

class Renderer
{
public:
	Renderer(GLuint w, GLuint h);
	void setScene(VirtualEntity *_scene);
	void setRiftManager(RiftManager *_riftManager){riftManager = _riftManager;}
	void setCamManager(CamManager *_camManager){camManager = _camManager;}
	void init();	//must be called after setScene()
	void render();
	void switchCams(){camManager->switchCams();}
	~Renderer(void);

private:
	////////////MEMBERS
	
	VirtualEntity *scene;
	CamManager *camManager;
	RiftManager *riftManager;

	bool glowEnabled;

	//render target sizes
	GLuint stdWidth, stdHeight, glowWidth, glowHeight;
	
	//shaders
	GLSLProgram phong;
	GLSLProgram glow;
	GLSLProgram blend;
	GLSLProgram gaussX;
	GLSLProgram gaussY;
	GLSLProgram drawTexture;

	// Locations
	GLint vertexLoc, normalLoc, outputFLoc, texCoordLoc;
	
	// texture drawing
	GLuint quadBuffer;	//for drawing viewport filling textures
	GLuint leftCamTex, rightCamTex;

	//fbos
	/*
	0 - Main: stdWidth x stdHeight, color+depth+stencil
	1 - GlowBuffer1: glowWidth x glowHeight, color+depth
	2 - GlowBuffer2: glowWidth x glowHeight, color+depth
	3 - Depth map: stdWidth x stdHeight, depth	(not needed at the moment)
	*/
	GLuint *FBOs;
	GLuint *renderTex;
	GLuint *depthTex;
	
	


	/////////////////METHODS

	//initialization
	void compileAndLinkShaders();
	void setupBuffers(EntityNode *parent);
	void setupFBO(GLuint w, GLuint h, int index, bool color, bool depth, bool stenc);

	//rendering
	void recursiveRendering(EntityNode *parent, mat4 transform, RenderPass pass);
	
	//methods called by recursiveRendering():
	void phongPass(Mesh *m, mat4 transform);		//standard rendering
	void glowPass(Mesh *m, mat4 transform);		//for glow effect

	//for rendering texture to screen
	void setupQuad();
	void renderTextureQuad(GLuint tex);
	void makeCamTextures(Mat *left, Mat *right);
	//void render2TexturesQuad(GLuint tex1, GLuint tex2, GLSLProgram *p);

	
};

