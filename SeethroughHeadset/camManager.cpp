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


/**
Establishes connections to two specified webcams.
If a specified camera is not connected, it is just marked as offline.
@param idLeft/idRight Possible camera IDs range from 0 to (number of connected cameras)-1.
*/
void CamManager::open(int idLeft, int idRight){

	//left
	capL = cvCaptureFromCAM(idLeft);
	if(capL){
		frameLTmp = cvQueryFrame(capL);
		frameL = cvCreateImage(cvSize(frameLTmp->height,frameLTmp->width),frameLTmp->depth,frameLTmp->nChannels);
		glGenTextures(1, &leftTex);
		leftConnected = true;
	}
	else
		cout << "Left cam (id " << idLeft << ") not connected." << endl;
	

	//right
	capR = cvCaptureFromCAM(idRight);
	if(capR){
		frameRTmp = cvQueryFrame(capR);
		frameR = cvCreateImage(cvSize(frameRTmp->height,frameRTmp->width),frameRTmp->depth,frameRTmp->nChannels);
		glGenTextures(1, &rightTex);
		rightConnected = true;
	}
	else
		cout << "Right cam (id " << idRight << ") not connected." << endl;
}


/**
Fetches camera frames, rotates them by 90&deg; and converts them to OpenGL textures.
If a camera is not connected, it is omited.
*/
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

/**
Switches the right/left assignments of the cameras (if both are connected).
*/
void CamManager::switchCams(){
	if(!leftConnected || !rightConnected) return;
	CvCapture *tmp = capL;
	capL = capR;
	capR = tmp;
}

/**
Returns the texture handle to the left frame or 0, if the corresponding camera is not connected.
*/
GLuint CamManager::getLeftTex(){
	if(leftConnected && camOn)
		return leftTex;
	else
		return 0;
}

/**
Returns the texture handle to the right frame or 0, if the corresponding camera is not connected.
*/
GLuint CamManager::getRightTex(){
	if(rightConnected && camOn)
		return rightTex;
	else
		return 0;
}