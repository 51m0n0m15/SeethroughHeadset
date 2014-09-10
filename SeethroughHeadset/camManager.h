#pragma once

#include <opencv.hpp>
using namespace cv;

#include "includes.h"
#include "cfg.h"


/**
Manages the connection to two USB cameras and delivers the camera frames as OpenGL textures.

Standard usage:\n
1) open(int idLeft, int idRight) to connect to the specified cameras\n
2) refresh() to fetch the new camera frames and create textures\n
3) getLeftTex() / getRightTex() return the current textures
*/
class CamManager
{
public:
	CamManager();
	~CamManager(void);

	void open(int idLeft, int idRight);
	void refresh();
	void switchCams();
	void toggleCamOn(){camOn=!camOn;}	/**< The whole camera processing can be bypassed with this.*/
	bool camIsOn(){return camOn;}
	GLuint getLeftTex();
	GLuint getRightTex();

private:
	bool camOn;
	bool leftConnected, rightConnected;

	CvCapture *capL;
	CvCapture *capR;
	IplImage *frameL;
	IplImage *frameR;

	IplImage *frameLTmp;
	IplImage *frameRTmp;

	GLuint leftTex;
	GLuint rightTex;
};

