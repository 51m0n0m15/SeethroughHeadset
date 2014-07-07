#pragma once


#include <ovr.h>
#include "includes.h"
using namespace OVR;





class RiftManager
{

public:
	enum Eye{LEFT_EYE, RIGHT_EYE};

	RiftManager(void);
	~RiftManager(void);

	void displayHMDInfo();
	void displaySensorData();
	void updateProjMatrices();
	void updateViewMatrices();

	glm::mat4 getProjLeft(){return projLeft;}
	glm::mat4 getProjRight(){return projRight;}
	glm::mat4 getViewLeft(){return viewLeft;}
	glm::mat4 getViewRight(){return viewRight;}
	glm::mat4 getViewCenter(){return viewCenter;}

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

