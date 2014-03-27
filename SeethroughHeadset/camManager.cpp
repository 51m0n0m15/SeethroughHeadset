#include "camManager.h"


CamManager::CamManager()
{
	capWidth = 800;
	capHeight = 640;

	capL = new VideoCapture();
	capR = new VideoCapture();

	frameL = new Mat();
	frameR = new Mat();
}


CamManager::~CamManager(void)
{
	capL->release();
	capR->release();
	delete frameL;
	delete frameR;
}


void CamManager::open(){
	capL->open(0);
	capL->set(CV_CAP_PROP_FRAME_WIDTH, capWidth);
	capL->set(CV_CAP_PROP_FRAME_HEIGHT, capHeight);

	capR->open(1);
	capR->set(CV_CAP_PROP_FRAME_WIDTH, capWidth);
	capR->set(CV_CAP_PROP_FRAME_HEIGHT, capHeight);
}

void CamManager::refresh(){
	*capL >> *frameL;
	transpose(*frameL, *frameL);

	*capR >> *frameR;
	transpose(*frameR, *frameR);
	//flip(*frameR, *frameR, 0);
	//flip(*frameR, *frameR, 1);
}

void CamManager::switchCams(){
	VideoCapture *tmp = capL;
	capL = capR;
	capR = tmp;
}