#pragma once

#define _USE_MATH_DEFINES

//std
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>


using std::string;
using std::cout;
using std::endl;
using std::vector;


//gl&co
#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <ext.hpp>

using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

#define idMat mat4(1.0,0.0,0.0,0.0,   0.0,1.0,0.0,0.0,   0.0,0.0,1.0,0.0,   0.0,0.0,0.0,1.0)

//opencv
#include <opencv.hpp>


//config
#include "cfg.h"