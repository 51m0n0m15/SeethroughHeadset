#pragma once

#include "includes.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
Reads parameters from an ini file and stores them as public static variables.
*/
class Cfg
{
public:
	Cfg(void);
	~Cfg(void);

	static void load(char* path);

	//display
	static int displayW;			/**< width of the whole HMD (left+right eye). Value will be overwritten by actual display size if fullscreen is on.*/
	static int displayH;			/**< height of the HMD  Value will be overwritten by actual display size if fullscreen is on.*/
	static bool fullscreen;			/**< start application in fullscreen mode */

	//models
	static string modelPath;		/**< path to the folder that contains the models that should be loaded */
	static string modelFileExt;		/**< e.g. '.dae' */

	//viewing parameters
	static float eyeHeight;			/**< y-coordinate of the eyes/camera in relation to the global coordinate system */
	static float fov;				/**< field of view of the camera model used for rendering virtual objects;
											should be set such that it matches the FOV of the cameras.*/
	static int cameraOffset;		/**< defines an offset of the right and left camera frame 
											towards the center in pixels. is used to achieve the right convergence.*/
	static float camScale;			/**< scales the camera image in relation to the virtual objects */
	static float distortionScale;	/**< constant that scales the wholeimage in the distortion correction shader pass. */
	

	//animation
	static double fps;				/**< animation speed for animated models */
	static double moveSpeed;		/**< animation speed for user-controlled movements */

	//cameras
	static int camIdLeft;			/**< id of left camera (can range from 0 to [number of connected cameras]-1)*/
	static int camIdRight;			/**< id of right camera (can range from 0 to [number of connected cameras]-1)*/
};

