#ifndef _engine_pch_h_
#define _engine_pch_h_

#define VC_EXTRALEAN
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "gl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "log.h"
#include "timer.h"
#include "display.h"

#include "shader.h"
#include "texture.h"

#include "input.h"
#include "util.h"

#include <Xinput.h>
#include <winsock2.h>
#include <gainput/gainput.h>

#pragma comment(lib, "opengl32.lib")

#ifdef _DEBUG
#pragma comment(lib, "gainputstatic-x64d.lib")
#else
#pragma comment(lib, "gainputstatic-x64.lib")
#endif

#pragma comment(lib, "Xinput9_1_0.lib")
#pragma comment(lib, "Ws2_32.lib")

struct sBounds
{
   sBounds() = default;
   sBounds(float minx, float miny, float maxx, float maxy) 
   {        left=minx;bottom=miny; right=maxx;   top=maxy; }
   
   float left;
   float bottom;
   float right;
   float top;
};

#endif
