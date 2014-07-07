#pragma once

/*
Texture unit convention:
0: color map
1: normal map
2: glow map
*/

#include "virtualEntity.h"
#include "entityInstance.h"
#include "includes.h"
#include "GLSLProgram.h"
#include "camManager.h"
#include "riftManager.h"



class Renderer
{
public:
	Renderer();
	void setRiftManager(RiftManager *_riftManager){riftManager = _riftManager;}
	void setCamManager(CamManager *_camManager){camManager = _camManager;}
	void init(vector<VirtualEntity*> *entities);	//must be called after setScene()
	void render(vector<EntityInstance*> *entities);
	~Renderer(void);

private:
	////////////MEMBERS
	CamManager *camManager;
	RiftManager *riftManager;
	
	//shaders
	GLSLProgram phong;
	GLSLProgram drawTexture;
	GLSLProgram warp;

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
	void recursiveRendering(EntityNode *parent, mat4 transform);
	void phongPass(Mesh *m, mat4 transform);		//standard rendering

	//for rendering texture to screen
	void setupQuad();
	void renderTextureQuad(GLuint tex);
	void makeCamTextures(Mat *left, Mat *right);

	
};

