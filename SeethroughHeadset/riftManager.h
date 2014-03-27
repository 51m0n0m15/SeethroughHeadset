#pragma once


#include <ovr.h>
#include "includes.h"
using namespace OVR;

class RiftManager
{
public:
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
	float getEyeHeight(){return eyeHeight;}

	void setFov(float _fov){fov = _fov; updateProjMatrices();}
	void setEyeHeight(float _eyeHeight){fov = eyeHeight;}


protected:
	float eyeHeight;
	float fov;

	OVR::Ptr<DeviceManager> pManager;
	OVR::Ptr<HMDDevice> pHMD;
	OVR::Ptr<SensorDevice> pSensor;
	HMDInfo *hmd;
	SensorFusion *SFusion;

	float EyeYaw;
	float EyePitch;
	float EyeRoll;
	float LastSensorYaw;

	mat4 viewLeft, viewRight,
			projLeft, projRight;

};

