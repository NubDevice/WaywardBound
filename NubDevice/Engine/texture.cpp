#include <pch.h>
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>


void Texture::initialize() 
{  
   default_id = add("default.png");
}

void Texture::use(GLuint id)
{
   glBindTexture(GL_TEXTURE_2D, id);
}

GLuint Texture::use(const char* name) 
{  for (auto item : name_id_map)
   {  if (item.first == name) 
      {  glBindTexture(GL_TEXTURE_2D, item.second);
         return item.second;
      }
   }
   glBindTexture(GL_TEXTURE_2D, default_id);
   return default_id;
}


GLuint Texture::add(const char* name) 
{  for (auto item : name_id_map) 
   {  if (item.first == name) 
      {  return item.second; // already exists
      }
   }
 
   std::string path = "_data/Texture/"; path += name;
   stbi_set_flip_vertically_on_load(true);
   
   int width, height, numChannels;
   auto data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
   
   if(numChannels != 4) 
   { 
      _log.entry(" requries alpha channel", _log._error, name); 
      return default_id;
   }

   GLuint id;
   if(data) 
   {  glGenTextures(1, &id);
      glBindTexture(GL_TEXTURE_2D, id);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(data);

      name_id_map.insert({name, id}); // add to mapped list
      _log.begin("Texture loaded: ", _log._info); std::cout<<name; _log.end();
      return id;
   }
   else
   {  _log.begin("Texture load failed: ", _log._error); std::cout << name; _log.end();
      return default_id;
   }
}

