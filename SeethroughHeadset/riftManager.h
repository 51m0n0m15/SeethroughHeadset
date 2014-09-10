#pragma once


#include <ovr.h>
using namespace OVR;

#include "includes.h"
#include "cfg.h"




/**
Handles the connection to the Oculus Rift and 
delivers view and projection matrices for stereo rendering of virtual objects.
*/
class RiftManager
{

public:
	enum Eye{LEFT_EYE, RIGHT_EYE};

	RiftManager(void);
	~RiftManager(void);

	void displaySensorData();
	void updateProjMatrices();
	void updateViewMatrices();

	glm::mat4 getProjLeft(){return projLeft;}		/**< Returns projection matrix for left eye. */
	glm::mat4 getProjRight(){return projRight;}		/**< Returns projection matrix for right eye. */
	glm::mat4 getViewLeft(){return viewLeft;}		/**< Returns view matrix for left eye. */
	glm::mat4 getViewRight(){return viewRight;}		/**< Returns view matrix for left eye. */
	glm::mat4 getViewCenter(){return viewCenter;}	/**< Returns central view matrix. */

	float getAspectRatio();
	glm::vec2 getLensCenter(Eye eye);
	glm::vec4 getWarpParameters();
	glm::vec4 getChromAbParameters();


protected:

	OVR::Ptr<DeviceManager> pManager;
	OVR::Ptr<HMDDevice> pHMD;
	OVR::Ptr<SensorDevice> pSensor;
	HMDInfo *hmd;
	SensorFusion *SFusion;

	float EyeYaw;
	float EyePitch;
	float EyeRoll;
	float LastSensorYaw;

	mat4 viewLeft, viewRight, viewCenter,
			projLeft, projRight;
};

