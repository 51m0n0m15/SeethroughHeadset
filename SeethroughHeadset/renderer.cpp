#include "renderer.h"


Renderer::Renderer(GLuint w, GLuint h)
{
	vertexLoc=1;
	normalLoc=2;
	texCoordLoc=3;

	stdWidth = w;
	stdHeight = h;
	glowWidth = w/2;
	glowHeight = h/2;
	glowEnabled = true;
}

Renderer::~Renderer(void)
{
	delete camManager;
	delete riftManager;
}

void Renderer::setScene(VirtualEntity *_scene){
	scene = _scene;
}

void Renderer::init(){
	//Set-UP OPENGL-Stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_FRAMEBUFFER);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0,0.0,0.0,1.0);
	glClearStencil(0x0);

	compileAndLinkShaders();
	setupBuffers(scene->root);

	//create the fbos and render textures
	int numFBOs = 4;
	FBOs = new GLuint[numFBOs];
	renderTex = new GLuint[numFBOs];
	depthTex = new GLuint[numFBOs];
	//main
	setupFBO(stdWidth, stdHeight, 0, true, true, true);
	//glow
	setupFBO(glowWidth, glowHeight, 1, true, true, false);
	setupFBO(glowWidth, glowHeight, 2, true, true, false);

	//for direct rendering of texture
	setupQuad();

	//start webcams
	camManager->open();
}

void Renderer::compileAndLinkShaders()
{
	std::ofstream log;
	log.open("log.txt");

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
	

	//glow
	if(!glow.compileShaderFromFile("shader/glow.vert",GLSLShader::VERTEX)) log << glow.log().c_str() << endl;
	if(!glow.compileShaderFromFile("shader/glow.frag",GLSLShader::FRAGMENT)) log << glow.log().c_str() << endl;
	glow.bindAttribLocation(vertexLoc, "position");
	glow.bindAttribLocation(texCoordLoc, "textureCoord");
	if(!glow.link()) log << glow.log().c_str() << endl;
	if(!glow.validate()){
		log << glow.log().c_str() << endl;
		exit(1);
	}


	//gaussX
	if(!gaussX.compileShaderFromFile("shader/gaussX.vert",GLSLShader::VERTEX)) log << gaussX.log().c_str() << endl;
	if(!gaussX.compileShaderFromFile("shader/gauss.frag",GLSLShader::FRAGMENT)) log << gaussX.log().c_str() << endl;
	gaussX.bindAttribLocation(vertexLoc, "position");
	if(!gaussX.link()) log << gaussX.log().c_str() << endl;
	if(!gaussX.validate()){
		log << gaussX.log().c_str() << endl;
		exit(1);
	}

	//gaussY
	if(!gaussY.compileShaderFromFile("shader/gaussY.vert",GLSLShader::VERTEX)) log << gaussY.log().c_str() << endl;
	if(!gaussY.compileShaderFromFile("shader/gauss.frag",GLSLShader::FRAGMENT)) log << gaussY.log().c_str() << endl;
	gaussY.bindAttribLocation(vertexLoc, "position");
	if(!gaussY.link()) log << gaussY.log().c_str() << endl;
	if(!gaussY.validate()){
		log << gaussY.log().c_str() << endl;
		exit(1);
	}

	//blend
	if(!blend.compileShaderFromFile("shader/passthrough.vert",GLSLShader::VERTEX)) log << blend.log().c_str() << endl;
	if(!blend.compileShaderFromFile("shader/blend.frag",GLSLShader::FRAGMENT)) log << blend.log().c_str() << endl;
	blend.bindAttribLocation(vertexLoc, "position");
	if(!blend.link()) log << blend.log().c_str() << endl;
	if(!blend.validate()){
		log << blend.log().c_str() << endl;
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

	log.close();
}


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
	if(!stenc){
		glGenRenderbuffers(1, &depthBuf);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		// Bind the depth buffer to the FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
	}

	if(!stenc && depth){
		// Create the texture object
		glGenTextures(1, &depthTex[index]);
		glActiveTexture(GL_TEXTURE1); // Use texture unit 1?
		glBindTexture(GL_TEXTURE_2D, depthTex[index]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Bind the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex[index], NULL);
	}
	
	GLuint stencil;

	if(stenc){
		// Create the depth-stencil buffer
		glGenRenderbuffers(1, &stencil);
		glBindRenderbuffer(GL_RENDERBUFFER, stencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, w, h);
		// Bind the depth-stencil buffer to the FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil);
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


void Renderer::render(){

	riftManager->updateViewMatrices();
	camManager->refresh();
	makeCamTextures(camManager->getFrameL(), camManager->getFrameR());
	

	//prepare main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	

	////////////////////////DRAW VIRTUAL OBJECTS
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	phong.use();
	phong.setUniform("lightPos", vec3(0,riftManager->getEyeHeight(),0));
	//left
	glViewport(0, 0, stdWidth/2, stdHeight);
	phong.setUniform("viewMatrix", riftManager->getViewLeft());
	phong.setUniform("projMatrix", riftManager->getProjLeft());
	phong.setUniform("cameraPos", vec3(riftManager->getViewLeft()*vec4(0,0,0,1)));
	recursiveRendering(scene->root, idMat, PHONG);
	//right
	glViewport(stdWidth/2, 0, stdWidth/2, stdHeight);
	phong.setUniform("viewMatrix", riftManager->getViewRight());
	phong.setUniform("projMatrix", riftManager->getProjRight());
	phong.setUniform("cameraPos", vec3(riftManager->getViewRight()*vec4(0,0,0,1)));
	recursiveRendering(scene->root, idMat, PHONG);



	////////////////////////DRAW CAMERA FRAMES
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	drawTexture.use();
	drawTexture.setUniform("tex",0);
	//left
	glViewport(0, 0, stdWidth/2, stdHeight);
	renderTextureQuad(leftCamTex);
	//right
	glViewport(stdWidth/2, 0, stdWidth/2, stdHeight);
	renderTextureQuad(rightCamTex);



	////////////////////////GLOW PASS		(careful: clears stencil and depth bit of main buffer!)
	if(glowEnabled){
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[1]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glow.use();
		//left
		glViewport(0, 0, glowWidth/2, glowHeight);
		glow.setUniform("viewMatrix", riftManager->getViewLeft());
		glow.setUniform("projMatrix", riftManager->getProjLeft());
		recursiveRendering(scene->root, idMat, GLOW);
		//right
		glViewport(glowWidth/2, 0, glowWidth/2, glowHeight);
		glow.setUniform("viewMatrix", riftManager->getViewRight());
		glow.setUniform("projMatrix", riftManager->getProjRight());
		recursiveRendering(scene->root, idMat, GLOW);
		
		//filtering: filters whole frame, not left/right separately; may cause artifacts at left/right-boundary
		glViewport(0, 0, glowWidth, glowHeight);
		//horizontal: fbo1->fbo2
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[2]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gaussX.use();
		gaussX.setUniform("tex",0);
		gaussX.setUniform("blurSize", scene->glowSize);
		renderTextureQuad(renderTex[1]);
		//vertical: fbo2->fbo1
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[1]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gaussY.use();
		gaussY.setUniform("tex",0);
		gaussX.setUniform("blurSize", scene->glowSize);
		renderTextureQuad(renderTex[2]);

		//COMBINE GLOW WITH OTHER IMAGE ->screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glViewport(0, 0, stdWidth, stdHeight);
		drawTexture.use();
		drawTexture.setUniform("tex",0);
		renderTextureQuad(renderTex[1]);
		glDisable(GL_BLEND);
	}



	//Swap Buffers after finished screen
	glutSwapBuffers();
}



void Renderer::makeCamTextures(Mat *left, Mat *right){
	
	glActiveTexture(GL_TEXTURE0);
	// Generate a number for our textureID's unique handle
	glDeleteTextures(1, &leftCamTex);
	glGenTextures(1, &leftCamTex);
 
	// Bind to our texture handle
	glBindTexture(GL_TEXTURE_2D, leftCamTex);
 
	// Set texture interpolation methods for minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Set incoming texture format to:
	// GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
	// GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
	// Work out other mappings as required ( there's a list in comments in main() )
	GLenum inputColourFormat = GL_BGR;
	if (left->channels() == 1)
	{
		inputColourFormat = GL_LUMINANCE;
	}
 
	// Create the texture
	glTexImage2D(GL_TEXTURE_2D,     // Type of texture
	             0,                 // Pyramid level (for mip-mapping) - 0 is the top level
	             GL_RGB,            // Internal colour format to convert to
	             left->cols,          // Image width  i.e. 640 for Kinect in standard mode
	             left->rows,          // Image height i.e. 480 for Kinect in standard mode
	             0,                 // Border width in pixels (can either be 1 or 0)
	             inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
	             GL_UNSIGNED_BYTE,  // Image data type
	             left->ptr());        // The actual image data itself



	// Generate a number for our textureID's unique handle
	glDeleteTextures(1, &rightCamTex);
	glGenTextures(1, &rightCamTex);
 
	// Bind to our texture handle
	glBindTexture(GL_TEXTURE_2D, rightCamTex);
 
	// Set texture interpolation methods for minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Set incoming texture format to:
	// GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
	// GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
	// Work out other mappings as required ( there's a list in comments in main() )
	if (left->channels() == 1)
	{
		inputColourFormat = GL_LUMINANCE;
	}
 
	// Create the texture
	glTexImage2D(GL_TEXTURE_2D,     // Type of texture
	             0,                 // Pyramid level (for mip-mapping) - 0 is the top level
	             GL_RGB,            // Internal colour format to convert to
	             right->cols,          // Image width  i.e. 640 for Kinect in standard mode
	             right->rows,          // Image height i.e. 480 for Kinect in standard mode
	             0,                 // Border width in pixels (can either be 1 or 0)
	             inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
	             GL_UNSIGNED_BYTE,  // Image data type
	             right->ptr());        // The actual image data itself
}



void Renderer::recursiveRendering(EntityNode *parent, mat4 transform, RenderPass pass){
	
	for(unsigned int i=0; i<parent->meshes.size(); i++){
		switch(pass){
		case PHONG:
			phongPass(parent->meshes.at(i), parent->transform*transform);
			break;
		case GLOW:
			glowPass(parent->meshes.at(i), parent->transform*transform);
			break;
		}
	}

	//recursion
	for(unsigned int i=0; i<parent->children.size(); i++){
		recursiveRendering(parent->children.at(i), parent->transform*transform, pass);
	}
}

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

void Renderer::glowPass(Mesh *m, mat4 transform){
	glow.setUniform("modelMatrix", transform);

	glow.setUniform("glowmap", 2);
	glow.setUniform("hasGlowmap", m->material->glowmapID!=0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m->material->colormapID);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m->material->glowmapID);

	
	glBindVertexArray(m->vao);	
	glDrawElements(GL_TRIANGLES, m->numFaces*3, GL_UNSIGNED_INT, m->indices);
}



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

void Renderer::renderTextureQuad(GLuint tex){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	glBindVertexArray(quadBuffer);	
	GLuint indices[] = {0,3,2, 0,1,3};
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
}


/*void Renderer::render2TexturesQuad(GLuint tex1, GLuint tex2, GLSLProgram *p){

	p->setUniform("tex1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);

	p->setUniform("tex2", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);

	glEnableVertexAttribArray(vertexLoc);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(vertexLoc);
}*/



