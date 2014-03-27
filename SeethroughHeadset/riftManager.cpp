#include "riftManager.h"


RiftManager::RiftManager(void)
{	
	System::Init(Log::ConfigureDefaultLog(LogMask_All));
	pManager = *DeviceManager::Create();
	pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();
	pSensor = *pHMD->GetSensor();
	hmd = new HMDInfo();
	pHMD->GetDeviceInfo(hmd);

	SFusion = new SensorFusion();
	if(pSensor) SFusion->AttachToSensor(pSensor);
	else cout << " no sensor found" << endl;
	updateProjMatrices();

	LastSensorYaw = 0;
	eyeHeight = 1.5f;
	fov = 0;
}


RiftManager::~RiftManager(void)
{
	pManager->Release();
	pHMD->Release();
	pSensor->Release();
	delete SFusion;
	System::Destroy();
}


void RiftManager::updateProjMatrices(){

	// Compute Aspect Ratio. Stereo mode cuts width in half.
	float aspectRatio = float(hmd->HResolution * 0.5f) / float(hmd->VResolution);
	
	
	if(fov == 0.0f){
		// Compute Vertical FOV based on distance.
		float halfScreenDistance = (hmd->VScreenSize / 2);
		fov = (2.0f * atan(halfScreenDistance/hmd->EyeToScreenDistance))*180.0f/(float)M_PI;		//vertical fov
	}
	// Projection matrix for the "center eye", which the left/right matrices are based on.
	mat4 projCenter = glm::perspective(fov, aspectRatio, 0.01f, 1000.0f);

	// Post-projection viewport coordinates range from (-1.0, 1.0), with the
	// center of the left viewport falling at (1/4) of horizontal screen size.
	// We need to shift this projection center to match with the lens center.
	// We compute this shift in physical units (meters) to correct
	// for different screen sizes and then rescale to viewport coordinates.
	float viewCenter = hmd->HScreenSize * 0.25f;
	float eyeProjectionShift = viewCenter - hmd->LensSeparationDistance*0.5f;
	float projectionCenterOffset = 4.0f * eyeProjectionShift / hmd->HScreenSize;

	
	
	
	projLeft = glm::translate(projectionCenterOffset, 0.0f, 0.0f) * projCenter;
	projRight = glm::translate(-projectionCenterOffset, 0.0f, 0.0f) * projCenter;
	
}

void RiftManager::updateViewMatrices(){
	Quatf hmdOrient = SFusion->GetOrientation();

	glm::vec3 eulerAngles;
    hmdOrient.GetEulerAngles<OVR::Axis_X, OVR::Axis_Y, OVR::Axis_Z, OVR::Rotate_CW, OVR::Handed_R>
    (&eulerAngles.x, &eulerAngles.y, &eulerAngles.z);

    glm::quat quaternion(eulerAngles); 
    glm::mat4 centerViewMat(glm::mat4_cast(quaternion));
	
	// View transformation translation in world units.
	float halfIPD = hmd->InterpupillaryDistance * 0.5f;

	viewLeft = glm::translate(halfIPD, -eyeHeight, 0.0f) * centerViewMat;
	viewRight= glm::translate(-halfIPD, -eyeHeight, 0.0f) * centerViewMat;
	
}



void RiftManager::displaySensorData(){
	// We extract Yaw, Pitch, Roll instead of directly using the orientation
	// to allow "additional" yaw manipulation with mouse/controller.
	Quatf hmdOrient = SFusion->GetOrientation();
	float yaw = 0.0f;
	
	hmdOrient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &EyePitch, &EyeRoll);
	EyeYaw += (yaw - LastSensorYaw);
	LastSensorYaw = yaw;
	// NOTE: We can get a matrix from orientation as follows:
	//Matrix4f hmdMat(hmdOrient);
	cout << EyeYaw << "   " << EyePitch << "   " << EyeRoll << endl;
}