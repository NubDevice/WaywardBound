#include <pch.h>
#include "shader.h"

Shader::Shader() {}

void Shader::setBool(const char* name, bool value)
{  glUniform1i(glGetUniformLocation(current_id, name), (int)value); 
}

void Shader::setInt(const char* name, int value)
{  glUniform1i(glGetUniformLocation(current_id, name), value); 
}

void Shader::setFloat(const char* name, float value)
{  glUniform1f(glGetUniformLocation(current_id, name), value); 
}

void Shader::setMatrix(const char* name, glm::mat4 value)
{  glUniformMatrix4fv(glGetUniformLocation(current_id, name), 1, GL_FALSE, glm::value_ptr(value)); 
}

void Shader::setVector(const char* name, glm::vec3 value)
{  glUniform3f(glGetUniformLocation(current_id, name), value.x, value.y, value.z);
}

void Shader::setColor(const char* name, glm::vec4 value)
{  glUniform4f(glGetUniformLocation(current_id, name), value.r, value.g, value.b, value.a); 
}

GLuint Shader::use(const char* name)
{  for (auto item : name_id_map)
   {  if (std::strcmp(item.first.c_str(), name) == 0)
      {  glUseProgram(item.second);
         current_id = item.second;
         return item.second; // shader program id
      }
   }
   return -1; // todo : have a default ready to piss off...
}


GLuint Shader::add(const char* name)
{
   for (auto item : name_id_map)
   {  if (std::strcmp(item.first.c_str(), name) == 0)
      {  
         return item.second; // already exists
      }
   }
   std::string path = "_data/Shader/"; path += name;
   GLint vsBufferSize;
   GLint fsBufferSize;
   char* vs_source = nubio::loadFile((path + ".vs").c_str(), vsBufferSize);
   char* fs_source = nubio::loadFile((path + ".fs").c_str(), fsBufferSize);

   auto assert_is = [&](GLuint id, const char* ext) {
      GLint compiled;
      glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
      if (!compiled) {
         _log.begin(name, _log._error); std::cout<<" "<<ext<<" shader not compiled\n";
         nubio::printShaderInfoLog(id);
         _log.end("");
      }
   };

   GLint vs_id = glCreateShader(GL_VERTEX_SHADER);
   GLint fs_id = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(vs_id, 1, &vs_source, &vsBufferSize);
   glShaderSource(fs_id, 1, &fs_source, &fsBufferSize);
   glCompileShader(vs_id); assert_is(vs_id, "vs");
   glCompileShader(fs_id); assert_is(fs_id, "fs");

   GLuint id = glCreateProgram();
   glBindAttribLocation(id, 0, "in_pos");
   glBindAttribLocation(id, 1, "in_uv");
   glBindAttribLocation(id, 2, "in_norm");
   glBindAttribLocation(id, 3, "in_tan");
   glBindAttribLocation(id, 4, "in_bitan"); // don;t really need

   glAttachShader(id, vs_id);
   glAttachShader(id, fs_id);
   glLinkProgram(id); // todo : assert link

   name_id_map.insert({name, id}); // add to mapped list
   _log.begin("Shader loaded: ", _log._success); std::cout << name; _log.end("");
   delete[] vs_source;
   delete[] fs_source;

      glUniform1i(glGetUniformLocation(id, "albedoMap"), 0);
      glUniform1i(glGetUniformLocation(id, "normalMap"), 1);
      glUniform1i(glGetUniformLocation(id, "shadowMap"), 2);
      glUniform1i(glGetUniformLocation(id, "bSpecular"), 0); // false;

   return id; // todo : default id on failure ? probably not :) // lost note(2020)
}
