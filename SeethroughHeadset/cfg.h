#pragma once

#include "includes.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

class Cfg
{
public:
	Cfg(void);
	~Cfg(void);

	static void load(char* path);

	
	static int displayW;
	static int displayH;

	static string modelPath;
	static string modelFileExt;

	static float eyeHeight;
	static float fov;
	static float distortionScale;

	static double fps;
	static double moveSpeed;

	static int captureW;
	static int captureH;
	static int camIdLeft;
	static int camIdRight;
	static bool rotRightCam180;
	//allows to translate camera images to achieve convergence:
	static int cameraOffset;
};

