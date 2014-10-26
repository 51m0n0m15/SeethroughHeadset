#include "cfg.h"

Cfg::Cfg(void)
{
}


Cfg::~Cfg(void)
{
}

int Cfg::displayW;
int Cfg::displayH;
bool Cfg::fullscreen;

string Cfg::modelPath;
string Cfg::modelFileExt;

float Cfg::eyeHeight;
float Cfg::fov;
int Cfg::cameraOffset;
float Cfg::camScale;
float Cfg::distortionScale;


double Cfg::fps;
double Cfg::moveSpeed;

int Cfg::camIdLeft;
int Cfg::camIdRight;


/**
Loads parameters from the specified ini file.
If a key is not found in the ini file, sets a default value.
If the specified ini file is not found, writes a message to the console and returns; note that the parameters remain undefined in that case.
*/
void Cfg::load(char *path){

	if(!boost::filesystem::exists(path)){
		cout << "ERROR: " << path << " not found." << endl;
		throw;
		return;
	}

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(path, pt);
	
	displayW = pt.get<int>("displayW", 1280);
	displayH = pt.get<int>("displayH", 800);
	fullscreen = pt.get<bool>("fullscreen", true);

	modelPath = pt.get<string>("modelPath", "models");
	modelFileExt = pt.get<string>("modelFileExt", ".dae");

	eyeHeight = pt.get<float>("eyeHeight", 0.0f);
	fov = pt.get<float>("fov", 75.0f);
	cameraOffset = pt.get<int>("cameraOffset", 60);
	camScale = pt.get<float>("camScale", 1.0f);
	distortionScale = pt.get<float>("distortionScale", 0.48f);

	fps = pt.get<double>("fps", 24);
	moveSpeed = pt.get<double>("moveSpeed", 1);

	camIdLeft = pt.get<int>("camIdLeft", 0);
	camIdRight = pt.get<int>("camIdRight", 1);
}