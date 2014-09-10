#pragma once

#include "includes.h"
#include "virtualEntity.h"
#include "entityInstance.h"
#include "GLSLProgram.h"
#include "camManager.h"
#include "riftManager.h"
#include "cfg.h"


/**
Takes care of everything related to rendering.
This class is taylored to the demo application and probably 
must be modified when realizing other applications.
*/
class Renderer
{
public:
	Renderer();
	void setRiftManager(RiftManager *_riftManager){riftManager = _riftManager;}		/**< sets a RiftManager; must be called befor rendering virtual objects */
	void setCamManager(CamManager *_camManager){camManager = _camManager;}			/**< sets a CamManager; must be called befor initialization */
	void init(vector<VirtualEntity*> *entities);
	void render(vector<EntityInstance*> *entities);
	void toggleLenseCorrection(){lenseCorrection = !lenseCorrection;}				/**< can disable/enable lens correction */
	void toggleShowGrid(){showGrid=!showGrid;}										/**< can show a regular grid for calibration purposes */
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
	
	// textures
	GLuint quadBuffer;	//for drawing viewport filling textures
	GLuint leftCamTex, rightCamTex;
	GLuint grid;

	//fbos
	GLuint *FBOs;
	GLuint *renderTex;
	
	//some flags
	bool lenseCorrection;
	bool showGrid;
	


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

};

