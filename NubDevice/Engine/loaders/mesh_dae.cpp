#include <pch.h>
#include "mesh_dae.h"

#include <stdio.h>
#include <iostream>

#include <sstream>
#include <iterator>

//https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#SS-type learn your shit
//http://www.wazim.com/Collada_Tutorial_1.htm c++ opengl collada skinning

#include "util.h" // string parsing utilities
#include <glm/gtc/type_ptr.hpp>

using namespace rapidxml;

static inline void ParseToFloatArray(const std::string token, glm::mat4& matrix)
{
   std::istringstream ss(token);
   std::istream_iterator<float> iter(ss);
   float transform[16];
   for (int i = 0; i < 16; i++) transform[i] = *iter++;
   memcpy(glm::value_ptr(matrix), transform, sizeof(transform));
}


void PrintSkeleton(Joint* joint, int depth = 0)
{
   for (int i = 0; i < depth; i++)
   {
      printf(".");
   }
   printf("%s\n", joint->name.c_str());

   printf("%.6f ", joint->bindLocalTransform[0][0]);
   printf("%.6f ", joint->bindLocalTransform[0][1]);
   printf("%.6f ", joint->bindLocalTransform[0][2]);
   printf("%.6f\n", joint->bindLocalTransform[0][3]);

   printf("%.6f ", joint->bindLocalTransform[1][0]);
   printf("%.6f ", joint->bindLocalTransform[1][1]);
   printf("%.6f ", joint->bindLocalTransform[1][2]);
   printf("%.6f\n", joint->bindLocalTransform[1][3]);

   printf("%.6f ", joint->bindLocalTransform[2][0]);
   printf("%.6f ", joint->bindLocalTransform[2][1]);
   printf("%.6f ", joint->bindLocalTransform[2][2]);
   printf("%.6f\n", joint->bindLocalTransform[2][3]);

   printf("%.6f ", joint->bindLocalTransform[3][0]);
   printf("%.6f ", joint->bindLocalTransform[3][1]);
   printf("%.6f ", joint->bindLocalTransform[3][2]);
   printf("%.6f\n", joint->bindLocalTransform[3][3]);

   if (joint->child != nullptr)
   {
      PrintSkeleton(joint->child, depth + 1);
   }
   if (joint->sibling != nullptr)
   {
      PrintSkeleton(joint->sibling, depth);
   }
}


void DAEModel::Initialize(const std::string& filePath)
{
   xml_document<> doc;
   std::ifstream theFile(filePath);
   std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)),
      std::istreambuf_iterator<char>());
   buffer.push_back('\0');
   doc.parse<0>(&buffer[0]);

   xml_node<>* root_node = doc.first_node();          // COLLADA
   xml_node<>* asset_node = root_node->first_node();  // asset

   //xml_node<>* images_node = root_node->first_node("library_images");
   //char* textureFilePath = images_node->first_node()->first_node()->value(); // mwk: skip this for now...scene controls texture assignment, not DCC

   // mesh
   DAEModel::LoadGeometries(root_node->first_node("library_geometries"));
   return;

   // skeleton
   xml_node<>* joints_node = root_node->first_node("library_visual_scenes");
   xml_node<>* armature = joints_node->first_node("visual_scene")->first_node("node");
   xml_node<>* armature_root = armature->first_node("node");
   DAEModel::BuildSkeletonHeirarchy(&skeleton_root, armature_root);
   // PrintSkeleton(&skeleton_root);

   // skin info
   DAEModel::LoadController(root_node->first_node("library_controllers"));

}


void DAEModel::LoadController(xml_node<>* library_node)
{
   xml_node<>* controller_node = library_node->first_node("controller");
   std::string armatureName = controller_node->first_attribute("name")->value();

   // bone names in index order ----------------------------------------
   xml_node<>* skin_node = controller_node->first_node("skin");
   xml_node<>* boneNameArray_node = skin_node->first_node("source")->first_node("Name_array");
   strtk::parse(boneNameArray_node->first_attribute("count")->value(), " ", numBones); // number of bones
   //std::cout << armatureName << ": " << numBones << " bones\n";
   strtk::parse(boneNameArray_node->value(), " ", boneMap);
   //std::cout << "root: " << boneMap[0] << "\n";

   // bind pose matrices -----------------------------------------------
   xml_node<>* skinBindPoseArray_node = skin_node->first_node("source")->next_sibling("source");
   std::vector<float> vf;
   strtk::parse(skinBindPoseArray_node->first_node("float_array")->value(), " ", vf);

   bindPoseMatrices.resize(numBones);
   for (size_t i = 0, index = 0; index < (numBones - 1); i += 16, index++)
   {
      bindPoseMatrices[index] = glm::mat4(vf[i], vf[i + 1], vf[i + 2], vf[i + 3],
         vf[i + 4], vf[i + 5], vf[i + 6], vf[i + 7],
         vf[i + 8], vf[i + 9], vf[i + 10], vf[i + 11],
         vf[i + 12], vf[i + 13], vf[i + 14], vf[i + 15]);
   }

   // weights -----------------------------------------------------------
   xml_node<>* skinWeightArray_node = skinBindPoseArray_node->next_sibling("source");
   strtk::parse<float>(skinWeightArray_node->first_node("float_array")->value(), " ", skinWeightArray);
   //std::cout << "Proceesing: " << skinWeightArray_node->first_attribute("id")->value() << "\n";
   //for (auto val : skinWeightArray) {  std::cout << val << " "; }  std::cout << "\n";
   //std::cout << "count weights: " << skinWeightArray.size() << "\n";

   // weight mapping ----------------------------------------------------
   xml_node<>* weightMapping_node = skin_node->first_node("vertex_weights");
   strtk::parse<unsigned int>(weightMapping_node->first_node("vcount")->value(), " ", weightMapCounter);
   strtk::parse<unsigned int>(weightMapping_node->first_node("v")->value(), " ", weightMapIndices);
   //for(auto val : weightMapCounter) { std::cout << val << " "; } std::cout << "\n";
   //for(auto val : weightMapIndices) { std::cout << val << " "; } std::cout << "\n";

   // process weight mapping to array 
   unsigned int bone[4];
   float weight[4];
   unsigned int mapIndex = 0;
   for (auto numInfluences : weightMapCounter)
   {
      bone[0] = bone[1] = bone[2] = bone[3] = 0;
      weight[0] = weight[1] = weight[2] = weight[3] = 0.f;
      for (unsigned int i = 0; i < numInfluences; i++)
      {
         if (i < 4) // todo : handle distrubuted weight scaling per influence
         {
            bone[i] = weightMapIndices[mapIndex]; mapIndex++;
            weight[i] = (float)skinWeightArray[weightMapIndices[mapIndex]]; mapIndex++;
         }
      }
      weights_boneID.push_back(glm::ivec4(bone[0], bone[1], bone[2], bone[3]));
      weights.push_back(glm::vec4(weight[0], weight[1], weight[2], weight[3]));
   }

   //unsigned int tID = 0;
   //std::cout << weights_boneID[tID].x << " " << weights_boneID[tID].y << " " << weights_boneID[tID].z << " " << weights_boneID[tID].w << "\n";
   //std::cout << weights[tID].x << " " << weights[tID].y << " " << weights[tID].z << " " << weights[tID].w << "\n";


   // animation ----------------------------------------------------------
}

void DAEModel::BuildSkeletonHeirarchy(Joint* joint, xml_node<>* node)
{
   joint->index = skeletonIndexing++;
   joint->name = node->first_attribute("sid")->value();                               // ******************************
   ParseToFloatArray(node->first_node("matrix")->value(), joint->bindLocalTransform); // todo: to quaternion **********

   if (node->first_node("node") != 0)
   {  // child present
      joint->child = new Joint();
      BuildSkeletonHeirarchy(joint->child, node->first_node("node"));
   }

   if (node->next_sibling("node") != 0)
   {  // sibling present
      joint->sibling = new Joint();
      BuildSkeletonHeirarchy(joint->sibling, node->next_sibling("node"));
   }

   return;
}


IndexedAnimModel DAEModel::ToIndexedModel()
{
   IndexedAnimModel im; // hardware format container
   // track found position-uv pairs from indices
   std::vector<std::pair<int, int>> vuv;
   for (auto ind = indices.begin(); ind != indices.end(); ind++)
   {
      int i = 0;
      bool found = false;
      for (auto ip : vuv)

      //
      // todo: uv <--> position pairing is still incorrect
      //

      {  // comparing index (int) pairs instead of vec(float, float, float) values
         if ((ip.first == ind->x) && (int(uvs[ip.second].x*1000) == int(uvs[ind->z].x*1000)) )// && (ip.second == ind->z)) // mwk: hack fix
         {
            im.indices.push_back(i); // reuse this index
            found = true;
            break;
         }
         i++;
      }
      
      if (!found)
      {  // make new (position-uv) and index it.
         vuv.push_back(std::make_pair(ind->x, ind->z));
         im.indices.push_back(i); // i is flow thru, use it //
         im.positions.push_back(vertices[ind->x]);
         im.normals.push_back(glm::vec3(normals[ind->y].x, normals[ind->y].y, normals[ind->y].z)); // recalculated in mesh
         im.texCoords.push_back(uvs[ind->z]);

        // im.weights.push_back(weights[ind->x]); todo: re-add me
        // im.boneIndices.push_back(weights_boneID[ind->x]); todo: re-add me
      }
   }
  
   //std::cout << "num pos: " << im.positions.size() << "\n";
   //std::cout << "num weights: " << im.weights.size() << "\n";

   // flip winding order
   /*for (size_t i = 0; i < im.indices.size(); i += 3)
   {
      short temp = im.indices[i];
      im.indices[i] = im.indices[i + 2];
      im.indices[i + 2] = temp;
   }*/

   // recalculate normals (hard surface-ish)
   //im.normals.resize(sizeof(im.positions));
   //im.CalcNormals(); // comment me out to retain file normals
   return im;
}


void DAEModel::LoadGeometries(xml_node<>* geometries_node)
{
   xml_node<>* mesh_node = geometries_node->first_node("geometry")->first_node("mesh");
   xml_node<>* source_iterator = mesh_node->first_node("source");
   xml_node<>* positions_node = source_iterator->first_node("float_array");
   xml_node<>* normals_node = source_iterator->next_sibling("source")->first_node("float_array");
   xml_node<>* uvs_node = source_iterator->next_sibling("source")->next_sibling("source")->first_node("float_array");
   // triangles ------------------
   xml_node<>* triangles_node = mesh_node->first_node("triangles");
   xml_node<>* triangles_data = triangles_node->first_node("p");

   std::istringstream ss(positions_node->value());
   std::istream_iterator<float> iter(ss);
   while (iter != std::istream_iterator<float>())
   {
      float x = *iter; iter++;
      float z = *iter; iter++;
      float y = *iter; iter++;
      vertices.push_back(glm::vec3(x, y, -z));
   }

   ss = std::istringstream(normals_node->value());
   iter = std::istream_iterator<float>(ss);
   while (iter != std::istream_iterator<float>())
   {
      float x = *iter; iter++;
      float z = *iter; iter++; // rotate 90 deg.
      float y = *iter; iter++; // file up does not populate thru? mwk (fixed?)
                               // returned: negated blend z axis
      normals.push_back(glm::vec3(x, y, -z));
   }

   ss = std::istringstream(uvs_node->value());
   iter = std::istream_iterator<float>(ss);
   while (iter != std::istream_iterator<float>())
   {
      float u = *iter; iter++;
      float v = *iter; iter++;
      uvs.push_back(glm::vec2(u, v));
   }

   ss = std::istringstream(triangles_data->value());
   std::istream_iterator<int> iIter(ss);
   while (iIter != std::istream_iterator<int>())
   {
      int vi = *iIter; iIter++;
      int vn = *iIter; iIter++;
      int vt = *iIter; iIter++;
      indices.push_back(glm::ivec3(vi, vn, vt));
   }
}
