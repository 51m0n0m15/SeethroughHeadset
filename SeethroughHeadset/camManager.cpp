#include "camManager.h"


CamManager::CamManager()
{
	camOn=true;

	leftConnected=false;
	rightConnected=false;
}


CamManager::~CamManager(void)
{
	cvReleaseCapture(&capL);
	cvReleaseCapture(&capR);
}


void CamManager::open(){

	//left
	capL = cvCaptureFromCAM(Cfg::camIdLeft);
	if(capL){
		frameLTmp = cvQueryFrame(capL);
		frameL = cvCreateImage(cvSize(frameLTmp->height,frameLTmp->width),frameLTmp->depth,frameLTmp->nChannels);
		glGenTextures(1, &leftTex);
		leftConnected = true;
	}
	else
		cout << "Left cam (id " << Cfg::camIdLeft << ") not connected." << endl;
	

	//right
	capR = cvCaptureFromCAM(Cfg::camIdRight);
	if(capR){
		frameRTmp = cvQueryFrame(capR);
		frameR = cvCreateImage(cvSize(frameRTmp->height,frameRTmp->width),frameRTmp->depth,frameRTmp->nChannels);
		glGenTextures(1, &rightTex);
		rightConnected = true;
	}
	else
		cout << "Right cam (id " << Cfg::camIdRight << ") not connected." << endl;
}

void CamManager::refresh(){

	//black background if cameras off
	if(!camOn){
		return;
	}
	
	
	//left frame
	if(leftConnected){
		frameLTmp = cvQueryFrame(capL);
		cvTranspose(frameLTmp, frameL);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, leftTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Image is memory aligned which means we there may be extra space at the end
		// of each row. gluBuild2DMipmaps needs contiguous data, so we buffer it here
		char * buffer = new char[frameL->width*frameL->height*frameL->nChannels];
		int step     = frameL->widthStep;
		int height   = frameL->height;
		int width    = frameL->width;
		int channels = frameL->nChannels;
		char * data  = (char *)frameL->imageData;

		for(int i=0;i<height;i++)
		{
			memcpy(&buffer[i*width*channels],&(data[i*step]),width*channels);
		}

		// Create Texture
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			frameL->width,
			frameL->height,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			buffer);

		// Clean up buffer
		delete[] buffer;
	}


	//right frame
	if(rightConnected){
		frameRTmp = cvQueryFrame(capR);
		cvTranspose(frameRTmp, frameR);
		if(Cfg::rotRightCam180){
			cvFlip(frameR, frameR, 0);
			cvFlip(frameR, frameR, 1);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rightTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Image is memory aligned which means we there may be extra space at the end
		// of each row. gluBuild2DMipmaps needs contiguous data, so we buffer it here
		char * buffer = new char[frameR->width*frameR->height*frameR->nChannels];
		int step     = frameR->widthStep;
		int height   = frameR->height;
		int width    = frameR->width;
		int channels = frameR->nChannels;
		char * data  = (char *)frameR->imageData;

		for(int i=0;i<height;i++)
		{
			memcpy(&buffer[i*width*channels],&(data[i*step]),width*channels);
		}

		// Create Texture
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			frameR->width,
			frameR->height,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			buffer);


		// Clean up buffer
		delete[] buffer;
	}
}

void CamManager::switchCams(){
	CvCapture *tmp = capL;
	capL = capR;
	capR = tmp;
}

GLuint CamManager::getLeftTex(){
	if(leftConnected)
		return leftTex;
	else
		return 0;
}

GLuint CamManager::getRightTex(){
	if(rightConnected)
		return rightTex;
	else
		return 0;
}