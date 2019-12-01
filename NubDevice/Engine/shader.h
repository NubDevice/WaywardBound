#ifndef _engine_shader_h_
#define _engine_shader_h_

// seven ways to insert to unordered map
// https://thispointer.com/different-ways-to-insert-elements-in-an-unordered_map/

#include <unordered_map>

class Shader
{

public:
   Shader();

   GLuint add(const char* name);

   GLuint use(const char* name);

   void setBool(  const char* name, bool value);  
   void setInt(   const char* name, int value);    
   void setFloat( const char* name, float value); 
   void setMatrix(const char* name, glm::mat4 value);
   void setVector(const char* name, glm::vec3 value);
   void setColor (const char* name, glm::vec4 value);

private:

   std::unordered_map<std::string, GLuint> name_id_map;
   GLuint current_id;

};

#endif 
