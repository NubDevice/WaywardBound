#ifndef _engine_dae_loader_h_
#define _engine_dae_loader_h_

#include "indexedModel.h"
#include <rapidxml/rapidxml.hpp>

#include <deque>
#define strtk_no_tr1_or_boost
#include <strtk/strtk.hpp>

struct Joint
{
   int index;
   std::string name;
   glm::mat4 bindLocalTransform;
   Joint* child;
   Joint* sibling;

   Joint()
   {
      index = 0;
      name = "";
      bindLocalTransform = glm::mat4();
      child = nullptr;
      sibling = nullptr;
   }

   ~Joint()
   {
      if (child) { delete child; child = nullptr; }
      if (sibling) { delete sibling; sibling = nullptr; }
   }
};


class DAEModel
{
public:

   struct VertexWeight
   {  // max four weights per vertex
      unsigned int boneID[4];// = {0, 0, 0, 0};
      float weight[4];// = {0.f, 0.f, 0.f, 0.f};
   };

   DAEModel() {}

   void Initialize(const std::string& filePath);
   IndexedAnimModel ToIndexedModel();

   // file representation
   std::vector<glm::vec3> vertices;
   std::vector<glm::vec2> uvs;
   std::vector<glm::vec3> normals;
   std::vector<glm::ivec3> indices;
   std::vector<glm::ivec4> weights_boneID;
   std::vector<glm::vec4> weights;

   Joint skeleton_root;
   std::deque<std::string> boneMap;           // linear name list
   std::vector<glm::mat4>  bindPoseMatrices;

   std::deque<float>        skinWeightArray;
   std::deque<unsigned int> weightMapCounter;
   std::deque<unsigned int> weightMapIndices;

private:
   void LoadGeometries(rapidxml::xml_node<>* node);
   void LoadController(rapidxml::xml_node<>* node);

   void BuildSkeletonHeirarchy(Joint* joint, rapidxml::xml_node<>* node); // recursive

   size_t numBones = 0;
   int numWeights = 0;

private:
   int skeletonIndexing = 0;
   glm::mat4 matrixCorrection; // file rotation to world up
};

#endif

