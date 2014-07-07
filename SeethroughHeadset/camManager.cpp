#include "camManager.h"


CamManager::CamManager()
{
	camOn=true;

	capL = new VideoCapture();
	capR = new VideoCapture();

	//capWidth, capHeight, CV_32F
	frameL = new Mat();
	frameR = new Mat();

	leftConnected=false;
	rightConnected=false;
}


CamManager::~CamManager(void)
{
	capL->release();
	capR->release();
	delete frameL;
	delete frameR;
}


void CamManager::open(){
	if(capL->open(Cfg::camIdLeft)) leftConnected=true;
	capL->set(CV_CAP_PROP_FRAME_WIDTH, Cfg::captureW);
	capL->set(CV_CAP_PROP_FRAME_HEIGHT, Cfg::captureH);

	if(capR->open(Cfg::camIdRight)) rightConnected=true;
	capR->set(CV_CAP_PROP_FRAME_WIDTH, Cfg::captureW);
	capR->set(CV_CAP_PROP_FRAME_HEIGHT, Cfg::captureH);
}

void CamManager::refresh(){
	
	//black background if cameras off
	if(!camOn){
		return;
	}
	
	if(leftConnected){
		*capL >> *frameL;
		transpose(*frameL, *frameL);
	} 

	if(rightConnected){
		*capR >> *frameR;
		transpose(*frameR, *frameR);
		//flip(*frameR, *frameR, 0);
		//flip(*frameR, *frameR, 1);
	}
}

void CamManager::switchCams(){
	VideoCapture *tmp = capL;
	capL = capR;
	capR = tmp;
}