#include "cfg.h"

Cfg::Cfg(void)
{
}


Cfg::~Cfg(void)
{
}

int Cfg::displayW;
int Cfg::displayH;

string Cfg::modelPath;
string Cfg::modelFileExt;

float Cfg::eyeHeight;
float Cfg::fov;
float Cfg::distortionScale;

double Cfg::fps;
double Cfg::moveSpeed;

int Cfg::captureW;
int Cfg::captureH;
int Cfg::camIdLeft;
int Cfg::camIdRight;


void Cfg::load(char *path){
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(path, pt);
	

	displayW = pt.get<int>("displayW");
	displayH = pt.get<int>("displayH");

	modelPath = pt.get<string>("modelPath");
	modelFileExt = pt.get<string>("modelFileExt");

	eyeHeight = pt.get<float>("eyeHeight");
	fov = pt.get<float>("fov");
	distortionScale = pt.get<float>("distortionScale");

	fps = pt.get<double>("fps");
	moveSpeed = pt.get<double>("moveSpeed");

	captureW = pt.get<int>("captureW");
	captureH = pt.get<int>("captureH");
	camIdLeft = pt.get<int>("camIdLeft");
	camIdRight = pt.get<int>("camIdRight");
}