#pragma once

#include "includes.h"
using namespace cv;

class CamManager
{
public:
	CamManager();
	~CamManager(void);

	void open();
	void refresh();
	void switchCams();
	void toggleCamOn(){camOn=!camOn;}
	bool camIsOn(){return camOn;}
	//Mat* getFrameL(){return frameL;}
	//Mat* getFrameR(){return frameR;}
	GLuint getLeftTex();
	GLuint getRightTex();

private:
	bool camOn;
	bool leftConnected, rightConnected;

	/*VideoCapture *capL;
	VideoCapture *capR;
	Mat *frameL;
	Mat *frameR;
	*/

	CvCapture *capL;
	CvCapture *capR;
	IplImage *frameL;
	IplImage *frameR;

	IplImage *frameLTmp;
	IplImage *frameRTmp;

	GLuint leftTex;
	GLuint rightTex;
};

