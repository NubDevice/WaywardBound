#ifndef _engine_mesh_h_
#define _engine_mesh_h_

//#define BUFFER_OFFSET(i) ((char*)NULL + (i))

struct sVertex
{  glm::vec3 pos;
   glm::vec2 uv;
};


class Mesh
{ 
   
public:
   Mesh()
   {
      name = "un-name-me";
   }
   
   Mesh(std::string _name) 
   {
      name = _name;
   }

   ~Mesh() { _log.entry(name.c_str(), _log._success, "~Mesh"); };


   void destroy()
   {  glDeleteVertexArrays(1, &vao);
      glDeleteBuffers(1, &vbo);
      glDeleteBuffers(1, &ebo);
      _log.entry(name.c_str(), _log._warning, "Mesh destroy: ");
   }

   
   void draw()
   {  glBindVertexArray(vao);
      glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
   }

   // expects vertices to be (position, uv) interleaved
   void initialize(float* vertices, unsigned int* indices, int countVertices, int countFaces, bool isBumpMapped=false)
   {  numVertices = countVertices;
      numIndices = countFaces * 3;
      if (isBumpMapped)
      {
         float* pV_out = new float[numVertices*14];

         int index = 0;
         for(int i=0; i<countFaces; i++)
         {  // generate per face tangent basis
            float* p1 = &vertices[indices[index+0]*8];
            float* p2 = &vertices[indices[index+1]*8];
            float* p3 = &vertices[indices[index+2]*8];
            glm::vec3 pos1 = glm::vec3(*p1, *(p1+1), *(p1+2));
            glm::vec3 pos2 = glm::vec3(*p2, *(p2+1), *(p2+2));
            glm::vec3 pos3 = glm::vec3(*p3, *(p3+1), *(p3+2));
            glm::vec2 uv1 = glm::vec2(*(p1+3), *(p1+4));
            glm::vec2 uv2 = glm::vec2(*(p2+3), *(p2+4));
            glm::vec2 uv3 = glm::vec2(*(p3+3), *(p3+4));

            glm::vec3 tangent;
            glm::vec3 bitangent;
            glm::vec3 edge1 = pos2 - pos1;
            glm::vec3 edge2 = pos3 - pos1;
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            //glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            glm::vec3 normal = glm::vec3(*(p1+5), *(p1+6), *(p1+7));  // file normals

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            tangent   = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
            bitangent = f * (deltaUV1.x * edge2 - deltaUV2.x * edge1);
            
            bitangent = glm::normalize(glm::cross(tangent, normal));
            tangent   = glm::normalize(glm::cross(bitangent, normal));

            // interleaved array     
            float* vp = &pV_out[(indices[index]*14)];
            int si = indices[index]*14;
            pV_out[si] = *p1;
            pV_out[si + 1] = *(p1+1);
            pV_out[si + 2] = *(p1+2); // position
            pV_out[si + 3] = *(p1+3);
            pV_out[si + 4] = *(p1+4); // uv
            pV_out[si + 5] = *(p1+5); 
            pV_out[si + 6] = *(p1+6); 
            pV_out[si + 7] = *(p1+7); // normal
            pV_out[si + 8] = tangent.x;
            pV_out[si + 9] = tangent.y;
            pV_out[si +10] = tangent.z;
            pV_out[si +11] = bitangent.x;
            pV_out[si +12] = bitangent.y;
            pV_out[si +13] = bitangent.z;
            si = indices[index+1]*14;
            pV_out[si] = *p2;
            pV_out[si + 1] = *(p2 + 1);
            pV_out[si + 2] = *(p2 + 2); // position
            pV_out[si + 3] = *(p2 + 3);
            pV_out[si + 4] = *(p2 + 4); // uv
            pV_out[si + 5] = *(p2 + 5); 
            pV_out[si + 6] = *(p2 + 6); 
            pV_out[si + 7] = *(p2 + 7); // normal
            pV_out[si + 8] = tangent.x;
            pV_out[si + 9] = tangent.y;
            pV_out[si + 10] = tangent.z;
            pV_out[si + 11] = bitangent.x;
            pV_out[si + 12] = bitangent.y;
            pV_out[si + 13] = bitangent.z;
            si = indices[index+2]*14;
            pV_out[si] = *p3;
            pV_out[si + 1] = *(p3 + 1);
            pV_out[si + 2] = *(p3 + 2); // position
            pV_out[si + 3] = *(p3 + 3);
            pV_out[si + 4] = *(p3 + 4); // uv
            pV_out[si + 5] = *(p3 + 5);  
            pV_out[si + 6] = *(p3 + 6); 
            pV_out[si + 7] = *(p3 + 7); // normal
            pV_out[si + 8] = tangent.x;
            pV_out[si + 9] = tangent.y;
            pV_out[si + 10] = tangent.z;
            pV_out[si + 11] = bitangent.x;
            pV_out[si + 12] = bitangent.y;
            pV_out[si + 13] = bitangent.z;

            index+=3;
         }

         glGenVertexArrays(1, &vao);
         glGenBuffers(1, &vbo);
         glGenBuffers(1, &ebo);

         glBindVertexArray(vao);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(float)*14, (void*)&pV_out[0], GL_STATIC_DRAW);
         glEnableVertexAttribArray(0);
         glEnableVertexAttribArray(1);
         glEnableVertexAttribArray(2);
         glEnableVertexAttribArray(3);
         glEnableVertexAttribArray(4);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*14, (void*)0);
         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*14, (void*)(3*sizeof(float)));
         glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)*14, (void*)(5*sizeof(float)));
         glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float)*14, (void*)(8*sizeof(float)));
         glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float)*14, (void*)(11*sizeof(float)));

         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint), indices, GL_STATIC_DRAW);
         
         glBindVertexArray(0);
         delete pV_out;
         _log.entry(name.c_str(), _log._info, "PBR Mesh Loaded: ");
         return;
      }

      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      glGenBuffers(1, &ebo);

      glBindVertexArray(vao);

      glBindBuffer(GL_ARRAY_BUFFER, vbo); // position, texcoord
      glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(sVertex), vertices, GL_STATIC_DRAW);
      
      glEnableVertexAttribArray(0); // position
      glEnableVertexAttribArray(1); // texcoord
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)0);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), ((char*)NULL + (12)));//BUFFER_OFFSET(12));
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint), indices, GL_STATIC_DRAW);

      glBindVertexArray(0);
      _log.entry(name.c_str(), _log._info, "Mesh Loaded: ");
   }


public:

   GLuint vao;
   GLuint vbo; // interleaved ? maybe switch back over. mwk:11-2019
   GLuint ebo;
   //GLuint ubo; :)

   int numVertices;
   int numIndices;
   std::string name;
   
};

#endif
