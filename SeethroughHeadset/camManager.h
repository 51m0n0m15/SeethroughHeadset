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
	void setResolution(int width, int height){capWidth = width; capHeight = height;}
	Mat* getFrameL(){return frameL;}
	Mat* getFrameR(){return frameR;}

private:
	int capWidth, capHeight;

	VideoCapture *capL;
	VideoCapture *capR;
	Mat *frameL;
	Mat *frameR;
};

