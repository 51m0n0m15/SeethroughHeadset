//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include "includes.h"
#include "renderer.h"
#include "riftManager.h"
#include "camManager.h"
#include "virtualEntity.h"


int windowHandle;
int windowWidth = 1280;
int windowHeight = 800;
bool fullscreen = false;
char *stdScenePath = "models/testscene.dae";


//MOUSE INPUT
int mouseX=windowWidth/2;
int mouseY=windowHeight/2;
bool mouseLocked=true;

//main components
Renderer *renderer;
RiftManager *riftManager;
CamManager *camManager;
VirtualEntity *scene;

void closeApp(){
	glutDestroyWindow(windowHandle);
	delete renderer;
}


/////////////////////////////////
//GLUT CALLBACK FUNCTIONS
/////////////////////////////////
void changeSize(int w, int h) {
	//scene->resize(w, h);
}

void renderScene(void) {
	renderer->render();
}

/////////////////////////////////
//CONTROLLS
/////////////////////////////////
char keymap[256];

char keyIsDown(unsigned char c)
{
	return keymap[(unsigned int)c];
}

void keyUp(unsigned char key, int x, int y)
{
	keymap[(unsigned int)key] = 0;
}

void keyDown(unsigned char key, int x, int y)
{
	keymap[(unsigned int)key] = 1;
	
	//escape key
	if((unsigned int)key==27){
		if(mouseLocked)
			mouseLocked = false;
		else{
			closeApp();
		}
	}

	//switch cameras l/r
	if((unsigned int)key=='f'){
		renderer->switchCams();
	}

}

void initKeymapManager()
{
	int i;
	for (i = 0; i < 256; i++) keymap[i] = 0;
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
}


clock_t lastFrame = 0;
void animation(void){

	//first execution, set current time and return
	if(lastFrame == 0){
		lastFrame = clock();
		return;
	}

	clock_t now = clock();
	double elapsed = double(now - lastFrame) / CLOCKS_PER_SEC;
	lastFrame=now;

	//cout << 1/elapsed << " fps";
	//cout.flush();
	//cout << '\r';

	double fps = 24;
	double moveSpeed = 30;


	//move
	/*
	if (keyIsDown('w')) {
		scene->cam->move(-elapsed*moveSpeed,0);
	}
	if (keyIsDown('s')) {
		scene->cam->move(elapsed*moveSpeed,0);
	}
	if (keyIsDown('a')) {
		scene->cam->move(0,-elapsed*moveSpeed);
	}
	if (keyIsDown('d')) {
		scene->cam->move(0,elapsed*moveSpeed);
	}*/



	/*
	if(mouseLocked){
		//look
		scene->cam->look((glutGet(GLUT_WINDOW_WIDTH)/2 - mouseX)/10,
			(glutGet(GLUT_WINDOW_HEIGHT)/2 - mouseY)/10);
		//reset mouse
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,
			glutGet(GLUT_WINDOW_HEIGHT)/2);
	}
	*/

	scene->update(elapsed*fps);

	renderer->render();
}

void passiveMouse(int x, int y){
	mouseX = x;
	mouseY = y;
}

void activeMouse(int button, int state, int x, int y){
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
		mouseLocked = true;
	}
}





int main(int argc, char *argv[])
{
	
	
	glutInit(&argc, argv);
	
	char *scenePath;
	
	
	if(argc >= 3){
		if(argv[1][0] == 'f')
			fullscreen = true;
			scenePath = argv[2];
	} else if(argc >=2){
		if(argv[1][0] == 'f')
			fullscreen = true;
		scenePath=stdScenePath;
	} else{
		scenePath=stdScenePath;
	}
	

	//glutInitContextVersion(4,3);	//strange: wird hier die version festgelegt, gibts keine texturen. und keine beleuchtung.
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(windowWidth,windowHeight);
	
	
	windowHandle = glutCreateWindow("Seethrough Headset Demo");

	if(fullscreen){
		glutFullScreen();
		windowWidth = glutGet(GLUT_WINDOW_WIDTH);
		windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	}

	glutDisplayFunc(renderScene);
	glutIdleFunc(animation);
	glutReshapeFunc(changeSize);
	glutPassiveMotionFunc(passiveMouse);
	glutMouseFunc(activeMouse);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	

	// Init KeymapManager
	initKeymapManager();

	cout << "Using OpenGL version " << glGetString(GL_VERSION) << ", core profile." << endl;
	// Check OpenGL-Version
	glewExperimental = GL_TRUE;
	glewInit();
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 not supported\n");
		exit(1);
	}

	

	//create rift manager
	riftManager = new RiftManager();
	riftManager->setEyeHeight(1.5f);
	riftManager->setFov(57.0f);
	camManager = new CamManager();

	//create scene
	scene = new VirtualEntity(scenePath);
	
	//create renderer
	renderer = new Renderer(windowWidth, windowHeight);
	renderer->setScene(scene);
	renderer->setCamManager(camManager);
	renderer->setRiftManager(riftManager);
	renderer->init();

	// Start Loop
	glutMainLoop();


	
	
	return 0;
}