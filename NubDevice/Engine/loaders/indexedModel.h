#ifndef _engine_indexed_model_h_
#define _engine_indexed_model_h_


class IndexedModel
{
public:
   IndexedModel() = default;
   std::vector<glm::vec3> positions;
   std::vector<glm::vec2> texCoords;
   std::vector<glm::vec3> normals;
   std::vector<unsigned int> indices;

   void CalcNormals();

};

class IndexedAnimModel : public IndexedModel
{
public:
   IndexedAnimModel() = default;
   std::vector<glm::ivec4> boneIndices; // bone influence id (per vertex)
   std::vector<glm::vec4> weights;      // weight influence  (per vertex)

};

#endif

