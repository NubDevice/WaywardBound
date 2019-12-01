#ifndef _engine_loaders_scene_h_
#define _engine_loaders_scene_h_

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
using namespace rapidxml;

#include <deque>
#define strtk_no_tr1_or_boost
#include <strtk/strtk.hpp>

#include <glm/gtx/string_cast.hpp>

#include "../loaders/mesh_dae.h"
#include "../mesh.h"

namespace Loaders
{
   class Scene
   {
      public:

         // https://sqlite.org/src/doc/trunk/README.md sqlite
         // geometry containers
         std::vector<Mesh> meshes;
         std::vector<GLint> mesh_ids;

         struct Draw_item
         {
            std::string tex_albedo_name = "";
            std::string tex_normal_name = "";
            std::vector<std::string> mesh_names;
            std::vector<glm::mat4> transforms;
            std::vector<size_t> mesh_id;
         };

         struct Renderable
         {
            std::string shader_name;
            std::vector<Draw_item> items;
         };

         // struct Lights
         // struct Triggers 
         // struct 
         // struct Whatever

         
         std::vector<Renderable> renderables;

         void printScene()
         {
            _log.entry(scene_name.c_str(), _log._default, "\nLoading Scene: ");
            for (auto renderable : renderables)
            {
               _log.begin("Shader : ", _log._default); std::cout << renderable.shader_name; _log.end();
               for(auto item : renderable.items)
               {
                  _log.begin("Texture: ", _log._default); std::cout << item.tex_albedo_name << ", " << item.tex_normal_name; _log.end();
                  for(auto mesh : item.mesh_names)
                  {
                     _log.begin("Mesh: ", _log._default); std::cout << mesh; _log.end(); // mesh name
                     // _log transform
                  }
               }
            }
            std::cout << "End Scene\n\n";
         }

         
         void load(const char* name)
         {
            scene_name = name;
            xml_document<> doc;
            std::string filePath = "_data/Scenes/"; filePath += name;
            std::ifstream theFile(filePath+".xml");
            std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)),std::istreambuf_iterator<char>());
            buffer.push_back('\0');
            doc.parse<0>(&buffer[0]);

            root_node = doc.first_node();
            if (root_node != 0)
            {  
               shader_node = root_node->first_node();
               loadNextShader();
            }
         }

         void uploadVFX(const char* name) {
            DAEModel fileModel;
            std::string path = "_data_raw/"; path += name; path += ".dae";
            fileModel.Initialize(path);

            IndexedAnimModel iam = fileModel.ToIndexedModel();
            float* verts = new float[8 * iam.positions.size()];
            for (unsigned int index = 0; index < iam.positions.size(); index++)
            {
               int i = index * 8;
               verts[i + 0] = iam.positions[index].x;
               verts[i + 1] = iam.positions[index].y;
               verts[i + 2] = iam.positions[index].z;
               verts[i + 3] = iam.texCoords[index].x;
               verts[i + 4] = iam.texCoords[index].y;
               verts[i + 5] = iam.normals[index].x;
               verts[i + 6] = iam.normals[index].y;
               verts[i + 7] = iam.normals[index].z;
            }

            meshes.push_back(Mesh(name));

            meshes.back().initialize(&verts[0], &iam.indices[0],
               int(iam.positions.size()),
               int(iam.indices.size() / 3), true); // todo: size concept is not symetical here
            delete verts;                          //       change to member get
            _log.begin("DAEObject : ", _log._info); std::cout << iam.positions.size(); _log.end();

            
            //todo: if you allow public access, you have to assign to texture group

         }

         void stage(Scene* scene, Shader* shaders, Texture* textures) 
         {
            for (auto renderable : scene->renderables)
            {
               shaders->add(renderable.shader_name.c_str());
               for (auto item : renderable.items)
               {
                  textures->add(item.tex_albedo_name.c_str());
                  textures->add(item.tex_normal_name.c_str());

                  for (auto name : item.mesh_names)
                  {
                     item.mesh_id.push_back(meshes.size());
                     uploadVFX(name.c_str());
                  }
               }
            }
         }

         void save()
         {
            _log.entry(scene_name.c_str(), _log._default, "\nSaving Scene: ");
            xml_document<> doc;
            xml_node<> *node = doc.allocate_node(node_element, "scene", "");
            
            for (auto renderable : renderables)
            {
               xml_node<> *shader_node = doc.allocate_node(node_element, "shader", "");
               xml_attribute<> *scene_name_attrib = doc.allocate_attribute("name", doc.allocate_string(renderable.shader_name.c_str()));
               shader_node->append_attribute(scene_name_attrib);
               node->append_node(shader_node);

               for (auto item : renderable.items)
               {
                  xml_node<> *texture_node = doc.allocate_node(node_element, "texture", "");
                  xml_attribute<> *albedo_attrib = doc.allocate_attribute("albedo", doc.allocate_string(item.tex_albedo_name.c_str()));
                  xml_attribute<> *normal_attrib = doc.allocate_attribute("normal", doc.allocate_string(item.tex_normal_name.c_str()));
                  texture_node->append_attribute(albedo_attrib);
                  texture_node->append_attribute(normal_attrib);
                  shader_node->append_node(texture_node);

                  int i=0;
                  for (auto mesh_name : item.mesh_names)
                  {   
                     xml_node<> *mesh_node = doc.allocate_node(node_element, "mesh", doc.allocate_string(glm::to_string(item.transforms[i]).c_str()) );
                     xml_attribute<> *name_attrib = doc.allocate_attribute("name", doc.allocate_string(mesh_name.c_str()));
                     mesh_node->append_attribute(name_attrib);
                     texture_node->append_node(mesh_node);
                  }
               }
            }

            doc.append_node(node);
            
            std::string path = "_data/Scenes/"; path += scene_name + ".xml";
            std::ofstream file(path, std::ofstream::trunc);
            file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
            file << doc;

            file.close();
         }



      private:

         void loadNextShader()
         {
            if (shader_node != 0)
            {
               renderables.push_back(Renderable());
               Renderable* ptr = &renderables.back();
               ptr->shader_name = shader_node->first_attribute("name")->value(); // shader name

               texture_node = shader_node->first_node("texture");
               loadNextMaterial(ptr);

               shader_node = shader_node->next_sibling("shader");
               loadNextShader(); // unverified...mwk
            }
         }

         void loadNextMesh(Draw_item* ptd)
         {
            if (mesh_node != 0)
            {
               ptd->mesh_names.push_back(mesh_node->first_attribute("name")->value());

               std::vector<float> vf;
               strtk::parse(mesh_node->value(), " ", vf);
               ptd->transforms.push_back(glm::mat4(vf[0], vf[1], vf[2], vf[3],
                  vf[4], vf[5], vf[6], vf[7],
                  vf[8], vf[9], vf[10], vf[11],
                  vf[12], vf[13], vf[14], vf[15]));

               // next mesh
               mesh_node = mesh_node->next_sibling("mesh");
               loadNextMesh(ptd);
            }
         }


         void loadNextMaterial(Renderable* ptr)
         {
            if (texture_node != 0)
            {
               ptr->items.push_back(Draw_item());
               Draw_item* ptd = &ptr->items.back();
               ptd->tex_albedo_name = texture_node->first_attribute("albedo")->value();
               ptd->tex_normal_name = texture_node->first_attribute("normal")->value();

               mesh_node = texture_node->first_node("mesh");
               loadNextMesh(ptd); 

               // next material
               texture_node = texture_node->next_sibling("texture");
               loadNextMaterial(ptr);
            }
         }


         private:
         
         // data load markers
         xml_node<>* shader_node;
         xml_node<>* texture_node;
         xml_node<>* mesh_node;
         xml_node<>* root_node;

         std::string scene_name;

   };
}


#endif
