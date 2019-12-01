#include <pch.h>

namespace nubio
{

char* loadFile(const char* fname, GLint& fSize)
{  std::ifstream::pos_type size;
   char* memblock;
   std::string text;

   // file read based on example in cplusplus.com tutorial
   std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
   if (file.is_open())
   {  size = file.tellg();
      fSize = (GLuint)size;
      memblock = new char[(unsigned int)size];

      file.seekg(0, std::ios::beg);
      file.read(memblock, size);
      file.close();
      text.assign(memblock);
   }
   else
   {  _log.entry("Unable to open file", _log._error, fname);
      exit(1);
   }
   return memblock;
}



void printShaderInfoLog(GLint shader)
{  int infoLogLen = 0;
   int charsWritten = 0;
   GLchar* infoLog;

   glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

   // should additionally check for OpenGL errors here

   if (infoLogLen > 0)
   {  infoLog = new GLchar[infoLogLen];
      // error check for fail to allocate memory omitted
      glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
      _log.entry(infoLog, _log._error, "InfoLog:");
      delete[] infoLog;
   }

   // should additionally check for OpenGL errors here
}

} // end namespace nubio


namespace nubfx
{
   unsigned int VAO_fill = -1;
   unsigned int VBO_fill;
   void viewportFill()
   {
      if (VAO_fill == -1)
      {
         float vertices[] = {
           -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
           -1.0f,-1.0f, 0.0f,  0.0f, 0.0f,
            1.0f,-1.0f, 0.0f,  1.0f, 0.0f,

           -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            1.0f,-1.0f, 0.0f,  1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         };

         glGenVertexArrays(1, &VAO_fill);
         glGenBuffers(1, &VBO_fill);
         glBindVertexArray(VAO_fill);
         glBindBuffer(GL_ARRAY_BUFFER, VBO_fill);
         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
         glEnableVertexAttribArray(0);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(1);
         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
         glBindVertexArray(0);
      }

      glBindVertexArray(VAO_fill);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
   }
} // end namespace nubfx
