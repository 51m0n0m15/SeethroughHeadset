
#include "includes.h"
#include "renderer.h"
#include "riftManager.h"
#include "camManager.h"
#include "virtualEntity.h"
#include "entityInstance.h"
#include <boost/filesystem.hpp>


int windowHandle;


//MOUSE INPUT
int mouseX;
int mouseY;
bool mouseLocked=true;

//placement
bool placementMode=false;
int currentEntity=0;

//main components
Renderer *renderer;
RiftManager *riftManager;
CamManager *camManager;
vector<VirtualEntity*> *availableEntities;
vector<EntityInstance*> *positionedEntities;


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
	renderer->render(positionedEntities);
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
		camManager->switchCams();
	}
	if((unsigned int)key=='c'){
		camManager->toggleCamOn();
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


	//move
	if (keyIsDown('w')) {
		//when in placement mode, move active entity away from user
		if(placementMode){
			positionedEntities->back()->translate(0.0f,0.0f,-elapsed*Cfg::moveSpeed);
		}
	}
	if (keyIsDown('s')) {
		//when in placement mode, move active entity away from user
		if(placementMode){
			positionedEntities->back()->translate(0.0f,0.0f,elapsed*Cfg::moveSpeed);
		}
	}



	
	if(mouseLocked){
		//look
		//scene->cam->look((glutGet(GLUT_WINDOW_WIDTH)/2 - mouseX)/10,
		//	(glutGet(GLUT_WINDOW_HEIGHT)/2 - mouseY)/10);

		//reset mouse
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,
			glutGet(GLUT_WINDOW_HEIGHT)/2);
	}
	

	for(int i=0; i<availableEntities->size(); i++){
		availableEntities->at(i)->update(elapsed*Cfg::fps);
	}
	if(placementMode){
		positionedEntities->back()->setRotation(glm::inverse(riftManager->getViewCenter()));
	}

	renderer->render(positionedEntities);
}

void passiveMouse(int x, int y){
	mouseX = x;
	mouseY = y;
}

void activeMouse(int button, int state, int x, int y){


	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){

		if(mouseLocked){

			if(!placementMode){
				//show first object in list of available entities

				positionedEntities->push_back(new EntityInstance(availableEntities->at(currentEntity))); 
				positionedEntities->back()->setRotation(glm::inverse(riftManager->getViewCenter()));
				positionedEntities->back()->translate(0.0f,0.0f,-2.0f);
				placementMode = true;
			}
			else{
				//fix selected object in space
				placementMode = false;
			}
		}


		else{
			mouseLocked = true;
		}
	}

}

//when in placement mode, display next/previous entity
void mouseWheel(int button, int dir, int x, int y){
	if(!placementMode) return;
	
	if(dir > 0){
		currentEntity = (currentEntity+1)%availableEntities->size();
	}
	else{
		currentEntity = (currentEntity-1)%availableEntities->size();
	}

	positionedEntities->back()->entity = availableEntities->at(currentEntity);
}




int main(int argc, char *argv[])
{
	
	//read config file
	Cfg::load("config.ini");
	

	glutInit(&argc, argv);
	

	//glutInitContextVersion(4,3);	//strange: wird hier die version festgelegt, gibts keine texturen. und keine beleuchtung.
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(Cfg::displayW, Cfg::displayH);
	
	
	windowHandle = glutCreateWindow("Seethrough Headset Demo");

	/*
	if(fullscreen){
		glutFullScreen();
		windowWidth = glutGet(GLUT_WINDOW_WIDTH);
		windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	}
	*/

	//set UI callback functions
	glutDisplayFunc(renderScene);
	glutIdleFunc(animation);
	glutReshapeFunc(changeSize);
	glutPassiveMotionFunc(passiveMouse);
	glutMouseFunc(activeMouse);
	glutMouseWheelFunc(mouseWheel);
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


	//load models from folder
	availableEntities = new vector<VirtualEntity*>;
	positionedEntities = new vector<EntityInstance*>;
	boost::filesystem::directory_iterator iterator(Cfg::modelPath);
	cout << "Loading models:" << endl;
	for(; iterator != boost::filesystem::directory_iterator(); ++iterator)
	{
		if(iterator->path().filename().extension().string()==Cfg::modelFileExt){
			string f = iterator->path().filename().string();
			availableEntities->push_back(new VirtualEntity((Cfg::modelPath+"/"+f).c_str()));
		}
	}
	


	//create rift manager & cam manager
	riftManager = new RiftManager();
	camManager = new CamManager();
	

	//create renderer
	renderer = new Renderer();
	renderer->setCamManager(camManager);
	renderer->setRiftManager(riftManager);
	renderer->init(availableEntities);


	// Start Loop
	glutMainLoop();


	
	
	return 0;
}