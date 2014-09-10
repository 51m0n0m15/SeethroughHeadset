#include "renderer.h"


Renderer::Renderer()
{
	vertexLoc=1;
	normalLoc=2;
	texCoordLoc=3;
	lenseCorrection = true;
	showGrid=false;
}

Renderer::~Renderer(void)
{
	delete camManager;
	delete riftManager;
}

/**
Initialization.
A CamManager must be set before calling this method.\n
the method:\n
- sets desired OpenGL states\n
- calls the compileAndLinkShaders() method\n
- creates vertex array objects for all available entities\n
- creates necessairy framebuffer objects and render textures for multi pass rendering\n
- creates a screen filling quad that can be used for texture display\n
- opens the webcam connection\n
@param entities a vector containing all virtual entities that should be prepared for rendering
*/
void Renderer::init(vector<VirtualEntity*> *entities){
	//Set-UP OPENGL-Stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_FRAMEBUFFER);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0,0.0,0.0,1.0);
	glClearStencil(0x0);

	//create shader programs
	compileAndLinkShaders();
	
	//create vertex array objects
	for(int i=0; i<entities->size(); i++){
		setupBuffers(entities->at(i)->root);
	}

	//create the fbos and render textures
	int numFBOs = 2;
	FBOs = new GLuint[numFBOs];
	renderTex = new GLuint[numFBOs];

	//left and right half images
	setupFBO(Cfg::displayW/2, Cfg::displayH, 0, true, true, true);
	setupFBO(Cfg::displayW/2, Cfg::displayH, 1, true, true, true);
	
	//for direct rendering of texture
	setupQuad();

	//a regular grid that can be displayed for calibration purposes
	grid = Material::loadTexture("models/texture/grid.jpg", 0);

	//start webcams
	camManager->open(Cfg::camIdLeft, Cfg::camIdRight);
}


/**
Compiles and links shader programs from files. Currently:\n
- phong: a simple standard phong shader which is used to render virtual objects\n
- drawTexture: can be used to draw a textures to the screen, in our case to draw the camera frames\n
- warp: corrects distortion and chromatic abberation on the final image\n
Shader compilation errors are written to the file "shader.log".\n
other shaders can be added.
*/
void Renderer::compileAndLinkShaders()
{
	std::ofstream log;
	log.open("shader.log");

	//phong
	if(!phong.compileShaderFromFile("shader/phong.vert",GLSLShader::VERTEX)) log << phong.log().c_str() << endl;
	if(!phong.compileShaderFromFile("shader/phong.frag",GLSLShader::FRAGMENT)) log << phong.log().c_str() << endl;
	phong.bindAttribLocation(vertexLoc, "position");
	phong.bindAttribLocation(normalLoc, "normal");
	phong.bindAttribLocation(texCoordLoc, "textureCoord");
	if(!phong.link()) log << phong.log().c_str() << endl;
	if(!phong.validate()){
		log << phong.log().c_str() << endl;
		exit(1);
	}

	//drawTexture
	if(!drawTexture.compileShaderFromFile("shader/passthrough.vert",GLSLShader::VERTEX)) log << drawTexture.log().c_str() << endl;
	if(!drawTexture.compileShaderFromFile("shader/drawTexture.frag",GLSLShader::FRAGMENT)) log << drawTexture.log().c_str() << endl;
	drawTexture.bindAttribLocation(vertexLoc, "position");
	if(!drawTexture.link()) log << drawTexture.log().c_str() << endl;
	if(!drawTexture.validate()){
		log << drawTexture.log().c_str() << endl;
		exit(1);
	}

	//warp
	if(!warp.compileShaderFromFile("shader/passthrough.vert",GLSLShader::VERTEX)) log << warp.log().c_str() << endl;
	if(!warp.compileShaderFromFile("shader/warpWithChromeAb.frag",GLSLShader::FRAGMENT)) log << warp.log().c_str() << endl;
	warp.bindAttribLocation(vertexLoc, "position");
	if(!warp.link()) log << warp.log().c_str() << endl;
	if(!warp.validate()){
		log << warp.log().c_str() << endl;
		exit(1);
	}

	//ADD MORE SHADERS HERE

	log.close();
}


/**
Is used to recursively traverse the scenegraph of a VirtualEntity
and create vertex array objects for efficient rendering.
*/
void Renderer::setupBuffers(EntityNode *parent){

	//for all objects in this node:
	for(unsigned int i=0; i<parent->meshes.size(); i++){
		Mesh *m = parent->meshes.at(i);

		GLuint buffers[3];

		glGenVertexArrays(1, &(m->vao));

		glBindVertexArray(m->vao);
		// Generate 3 slots for the vertex, normal and texture coordinates
		glGenBuffers(3, buffers);

		// bind buffer for vertices and copy data into buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices*4*sizeof(GLfloat), 
			m->vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertexLoc);
		glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);


		// bind buffer for normals and copy data into buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), 
			m->normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(normalLoc);
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);


		// bind buffer for texture and copy data into buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), 
			m->textureCoord, GL_STATIC_DRAW);
		glEnableVertexAttribArray(texCoordLoc);
		glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, 0, 0, 0);
	}

	//recursive for child nodes:
	for(unsigned int i=0; i<parent->children.size(); i++){
		setupBuffers(parent->children.at(i));
	}
}


/**
Sets up a framebuffer object and a texture render target for color output (if desired)
@param w buffer width
@param h buffer height
@param index index of buffer in the list
@param color need color buffer?
@param depth need depth buffer?
@param stencil need stencil buffer?
*/
void Renderer::setupFBO(GLuint w, GLuint h, int index, bool color, bool depth, bool stenc){

	// Generate and bind the framebuffer
	glGenFramebuffers(1, &FBOs[index]);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOs[index]);

	if(color){
		// Create color texture object
		glGenTextures(1, &renderTex[index]);
		glActiveTexture(GL_TEXTURE0); // Use texture unit 0
		glBindTexture(GL_TEXTURE_2D, renderTex[index]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		// Bind the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex[index], NULL);
	}

	// Create the depth buffer
	GLuint depthBuf;
	if(!stenc && depth){
		glGenRenderbuffers(1, &depthBuf);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		// Bind the depth buffer to the FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
	}

	GLuint stencil;
	if(stenc){
		// Create the depth-stencil buffer
		glGenRenderbuffers(1, &stencil);
		glBindRenderbuffer(GL_RENDERBUFFER, stencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, w, h);
		// Bind the depth-stencil buffer to the FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil);
	}

	// Set the target for the fragment shader outputs
	if(color){
		GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, drawBufs);
	}
	else{
		glDrawBuffer(GL_NONE);
	}

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "problems with framebuffer " << index << endl;
	else
		cout << "framebuffer " << index << " ok" << endl;


	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
Performs one full render pass.
Renders virtual objects, camera frames and a regular grid if desired.
Performs lens correction.\n
This method is taylored to the demo application, but can easily 
modified to suit other applications.
*/
void Renderer::render(vector<EntityInstance*> *entities){
	
	
		
	////////////////////////DRAW VIRTUAL OBJECTS (modify this section for other applications)
	
	//make the riftManager calculate new view matrices from the sensor data
	riftManager->updateViewMatrices();

	/*	we use the stencil buffer to mark those fragments, where a virtual object exists;
		later, the camera frames will not be drawn on those fragments.
	*/
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	phong.use();
	phong.setUniform("lightPos", vec3(0,Cfg::eyeHeight,0));
	
	//render left eye image to texture 0
	glBindFramebuffer(GL_FRAMEBUFFER, FBOs[0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, Cfg::displayW/2, Cfg::displayH);
	phong.setUniform("viewMatrix", riftManager->getViewLeft());
	phong.setUniform("projMatrix", riftManager->getProjLeft());
	phong.setUniform("cameraPos", vec3(riftManager->getViewLeft()*vec4(0,0,0,1)));
	for(int i=0; i<entities->size(); i++){
		recursiveRendering(entities->at(i)->entity->root, entities->at(i)->getTransform());
	}
	//render right eye image to texture 1
	glBindFramebuffer(GL_FRAMEBUFFER, FBOs[1]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, Cfg::displayW/2, Cfg::displayH);
	phong.setUniform("viewMatrix", riftManager->getViewRight());
	phong.setUniform("projMatrix", riftManager->getProjRight());
	phong.setUniform("cameraPos", vec3(riftManager->getViewRight()*vec4(0,0,0,1)));
	for(int i=0; i<entities->size(); i++){
		recursiveRendering(entities->at(i)->entity->root, entities->at(i)->getTransform());
	}
	
	


	////////////////////////DRAW CAMERA FRAMES
	
	//refresh the camera-textures
	camManager->refresh();

	if(camManager->camIsOn()){
		//draw only on non-marked fragments
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		drawTexture.use();
		drawTexture.setUniform("tex",0);
		
		int w = Cfg::displayW/4;
		int h = Cfg::displayH/2;

		//left image
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[0]);
		glViewport(w-w*Cfg::camScale+Cfg::cameraOffset, h-h*Cfg::camScale, 
			Cfg::displayW/2*Cfg::camScale, Cfg::displayH*Cfg::camScale);
		//glViewport(0+Cfg::cameraOffset, 0, Cfg::displayW/2, Cfg::displayH);
		renderTextureQuad(camManager->getLeftTex());
		//right image
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[1]);
		glViewport(w-w*Cfg::camScale-Cfg::cameraOffset, h-h*Cfg::camScale, 
			Cfg::displayW/2*Cfg::camScale, Cfg::displayH*Cfg::camScale);
		//glViewport(0-Cfg::cameraOffset, 0, Cfg::displayW/2, Cfg::displayH);
		renderTextureQuad(camManager->getRightTex());
	}


	//DRAW REGULAR GRID
	if(showGrid){
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
		glEnable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);

		drawTexture.use();
		drawTexture.setUniform("tex",0);

		//left
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[0]);
		glViewport(0, 0, Cfg::displayW/2, Cfg::displayH);
		renderTextureQuad(grid);
		//right
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[1]);
		glViewport(0, 0, Cfg::displayW/2, Cfg::displayH);
		renderTextureQuad(grid);

		glDisable(GL_BLEND);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
	}



	//RENDER TO SCREEN (with or without lense correction)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	if(lenseCorrection){    //apply rift lense correction
		warp.use();
		warp.setUniform("tex",0);

		warp.setUniform("Scale", vec2(Cfg::distortionScale, Cfg::distortionScale*riftManager->getAspectRatio()));
		warp.setUniform("ScaleIn", vec2(2.0f, 2.0f/riftManager->getAspectRatio()));
		warp.setUniform("HmdWarpParam", riftManager->getWarpParameters());
		warp.setUniform("ChromAbParam", riftManager->getChromAbParameters());

		//left
		glViewport(0, 0, Cfg::displayW/2, Cfg::displayH);
		warp.setUniform("LensCenter", riftManager->getLensCenter(RiftManager::LEFT_EYE));
		renderTextureQuad(renderTex[0]);
		//right
		glViewport(Cfg::displayW/2, 0, Cfg::displayW/2, Cfg::displayH);
		warp.setUniform("LensCenter", riftManager->getLensCenter(RiftManager::RIGHT_EYE));
		renderTextureQuad(renderTex[1]);
	}
	else{	//no lense correction
		drawTexture.use();
		drawTexture.setUniform("tex",0);

		//left
		glViewport(0, 0, Cfg::displayW/2, Cfg::displayH);
		renderTextureQuad(renderTex[0]);
		//right
		glViewport(Cfg::displayW/2, 0, Cfg::displayW/2, Cfg::displayH);
		renderTextureQuad(renderTex[1]);
	}


	//Swap Buffers after finished screen
	glutSwapBuffers();
}


/**
Recursive method for rendering the scenegraph of a VirtualEntity.
@param parent node to be processed
@param transform accumulated transformation matrices from root to current recursion depth
*/
void Renderer::recursiveRendering(EntityNode *parent, mat4 transform){
	
	for(unsigned int i=0; i<parent->meshes.size(); i++){
		phongPass(parent->meshes.at(i), transform*parent->transform);
		break;
	}

	//recursion
	for(unsigned int i=0; i<parent->children.size(); i++){
		recursiveRendering(parent->children.at(i), transform*parent->transform);
	}
}


/**
Helper method for phong rendering.
*/
void Renderer::phongPass(Mesh *m, mat4 transform){

	phong.setUniform("modelMatrix", transform);

	phong.setUniform("ambient", m->material->colorAmbient);
	phong.setUniform("diffuse", m->material->colorDiffuse);
	phong.setUniform("specular", m->material->colorSpecular);
	phong.setUniform("shininess", m->material->shininess);

	phong.setUniform("colormap", 0);	//texture unit 0
	phong.setUniform("normalmap", 1);	//texture unit 1

	phong.setUniform("hasColormap", m->material->colormapID!=0);
	phong.setUniform("hasNormalmap", m->material->normalmapID!=0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m->material->colormapID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m->material->normalmapID);

	glBindVertexArray(m->vao);	
	glDrawElements(GL_TRIANGLES, m->numFaces*3, GL_UNSIGNED_INT, m->indices);
}


/**
Creates a viewport filling quad for direct texture rendering.
*/
void Renderer::setupQuad(){
		GLfloat positions[] = { 
			-1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,
			1.0f,  1.0f, 0.0f, 1.0f
		};
		
		GLuint vertexBuffer;
		glGenVertexArrays(1, &quadBuffer);
		glBindVertexArray(quadBuffer);
		glGenBuffers(1, &vertexBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertexLoc);
		glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);
}

/**
Renders a viewport filling quad with a texture.
@param tex the texture that should be rendered
*/
void Renderer::renderTextureQuad(GLuint tex){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	glBindVertexArray(quadBuffer);	
	GLuint indices[] = {0,3,2, 0,1,3};
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
}

