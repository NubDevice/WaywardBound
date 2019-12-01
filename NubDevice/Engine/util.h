#ifndef _engine_util_h_
#define _engine_util_h_

#include <vector>
#include <sstream>

namespace nubio 
{
   char* loadFile(const char* fname, GLint& fSize);
   void  printShaderInfoLog(GLint shader);
}

namespace nubfx
{
   void viewportFill();
}

#endif
