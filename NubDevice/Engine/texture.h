#ifndef _engine_texture_h_
#define _engine_texture_h_

#include <unordered_map>

class Texture
{
   
public:
   Texture() = default;
   void initialize();

   GLuint add(const char* name);

   void   use(GLuint id);
   GLuint use(const char* name);

private:   
   
   std::unordered_map<std::string, GLuint> name_id_map;
   GLuint default_id;

};

#endif
