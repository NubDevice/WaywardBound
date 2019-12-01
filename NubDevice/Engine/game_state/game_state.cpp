// dwtfyw v0.start here
#include <pch.h>
#include "game_state.h"

#include "../input.h"
#include "../camera.h"
#include "../shader.h"
#include "../texture.h"
#include "../mesh.h"
#include "../sound.h"

#include "../effects/shadow_directional.h"
Effects::cShadow_directional shadow_directional;

#include "../loaders/scene.h"

using namespace game_state;
using namespace glm;
extern Texture textures;

extern Input input;
extern Display display;
extern Sound sound;

// sound buffers
// sound sources

#include "../gui/line.h"
#include "../gui/gizmo.h"
gui::Line line;
gui::Gizmo gizmo;


Camera camera(vec3(10.0f, 1.6f, -5.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
glm::mat4 projection;


// https://www.gamedev.net/forums/topic/671562-spherical-harmonics-cubemap/?tab=comments#comment-5250908 mjp
// https://www.ppsloan.org/publications/shvis_jgt.pdf
//
// http://cseweb.ucsd.edu/~ravir/papers/invlamb/josa.pdf spherical harmonics
// http://www.ppsloan.org/publications/StupidSH36.pdf stupid tricks
//
// https://github.com/KhronosGroup
// https://github.com/syoyo/tinygltf glTF 
//

#include "game/config_input.h"


void Game_state::initialize()
{ 
   _log.entry("initialize", _log._warning, "Game");
  
   glDisable(GL_BLEND);
   gizmo.initialize();
 
   scene.load("mechsylvania");
   scene.stage(&scene, pShaders, &textures);
  
   
   pShaders->add("shadow_directional");
   pShaders->add("gizmo");
   pShaders->add("bkg");
   
          // for shits and giggles later(screen space reflections)
          // https://lettier.github.io/3d-game-shaders-for-beginners/screen-space-reflection.html 
   shadow_directional.initialize(1024, 1024);

   game::config_input(&input);

   projection = glm::perspective(30.04f, 1.33f, 0.01f, 1000.0f);



   // background wall test prop
   float vrt[] = {-28.5f, 60.0f, 50.5f, 0.0f, 1.0f,
                  -28.5f,-40.0f, 50.5f, 0.0f, 0.0f,
                  -28.5f,-40.0f,-50.5f, 1.0f, 0.0f,
                  -28.5f, 60.0f,-50.5f, 1.0f, 1.0f
                 };

   unsigned int ind[] = { 0,1,2, 0,2,3 };

   scene.meshes.push_back(Mesh("background")); 
   scene.meshes.back().initialize(vrt, ind, 4, 2, false);
   //bkg_id = int(scene.meshes.size()-1); // come back :)
}


void Game_state::shutdown()
{ _log.entry("shutdown", _log._warning, "Game"); 
   for(int i=int(scene.meshes.size())-1; i> -1; i--)
   {
      scene.meshes[i].destroy();
   }
}



void Game_state::pause()  {}
void Game_state::resume() {}



void Game_state::updateInput(float delta_time)
{
   action_this_frame = game::update_input(delta_time, game_state::none, &input, &display, &scene, &camera, &gizmo);

}

void Game_state::updateWorld(float delta_time)
{


}



int tick = 0;
float Game_state::getTimerInfo() {float result=float(tick / profile_timer.end()); tick=0; return result; }


void Game_state::draw(float time_alpha)
{  

   static bool once = true; 
   if (once) 
   {  once = false; _log.entry("Begin", _log._warning, "Game"); 
   }  tick++; // todo : remove me...profile timer data


   // 
   // todo: add camera bobbing
   //

   //
   // todo: add debug tear away camera
   //

   gizmo.update(&display, &input, &scene, pShaders, projection * camera.getViewMatrix());



   // shadow pass.....................
   glCullFace(GL_FRONT);
   shadow_directional.enableBuffer(pShaders->use("shadow_directional"), camera.position);
   pShaders->setMatrix("model", scene.renderables[0].items[0].transforms[0]);
   
   scene.meshes[0].draw();
   // todo : run shadow caster list
   //meshes[4].draw(); // todo: re design mesh layout to support just vertex positions here. 
                       // decrease cache pressure on shadow buffer update

   shadow_directional.disableBuffer();
   glViewport(0, 0, 800, 600);
   glCullFace(GL_BACK);



   // render pass.....................
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   for (auto renderable : scene.renderables)
   { 
      GLint currentShader_id = pShaders->use(renderable.shader_name.c_str());
      pShaders->setInt("bSpecular", 0); // refactor me out?
      pShaders->setMatrix("transform", projection* camera.getViewMatrix());
      pShaders->setMatrix("lightSpace", shadow_directional.lightSpaceMatrix);
      pShaders->setVector("lightPos", vec3(-9.0f, 14.0f, -12.0f));
      pShaders->setVector("viewPos", camera.position);

      glActiveTexture(GL_TEXTURE0);
      glUniform1i(glGetUniformLocation(currentShader_id, "albedoMap"), 0);
      glActiveTexture(GL_TEXTURE1);
      glUniform1i(glGetUniformLocation(currentShader_id, "normalMap"), 1);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, shadow_directional.tex_depthMap);
      glUniform1i(glGetUniformLocation(currentShader_id, "shadowMap"), 2);
      
      int i=0;
      for (auto item : renderable.items)
      {
         glActiveTexture(GL_TEXTURE0);
         textures.use(item.tex_albedo_name.c_str());
         glActiveTexture(GL_TEXTURE1);
         textures.use(item.tex_normal_name.c_str());

         for(auto transform : item.transforms)
         {
            pShaders->setMatrix("model", transform);
            scene.meshes[i].draw();
         
            i++;
         }
      }
   }

   //pShaders->use("bkg"); // experimental back drop
   //pShaders->setMatrix("transform", projection * camera.getViewMatrix());
   //meshes[bkg_id].draw();


   if(gizmo.bObj_selected)
   {
      glm::mat4 mvp = projection * camera.getViewMatrix() * scene.renderables[0].items[gizmo.selected_item_id].transforms[0];
      gizmo.draw(display.hwnd, pShaders, mvp);
   }

   // shadow texture overlay draw
   glViewport(0, 0, 300, 200);
   pShaders->use("default");
   glm::mat4 identity(1.0); pShaders->setMatrix("transform", identity);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, shadow_directional.tex_depthMap);
   nubfx::viewportFill();
   glViewport(0, 0, 800, 600);   
}


eStateAction Game_state::handleEvents()
{ 
   eStateAction result = eStateAction::none;
   switch (action_this_frame)
   {

   case eStateAction::moveWindow:
   {  RECT rcClient; ::GetClientRect(display.hwnd, &rcClient); // client space
      RECT rcWindow; ::GetWindowRect(display.hwnd, &rcWindow); // screen space
      int frame = ((rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left)) / 2; //(loose fractional??)
      RECT rcClip = { rcWindow.left + frame - 2, rcWindow.top,
                      rcWindow.right - frame + 5,// pixels extra
                      rcWindow.bottom - frame }; // LTRB
      ::ClipCursor(&rcClip); // screen space
   }  break;
    
   case eStateAction::quit:
      result = action_this_frame; // pass through
      _log.begin("Avg FPS: ", _log._warning); std::cout << getTimerInfo(); _log.end();
      ::ShowCursor(true);
      ::ClipCursor(nullptr);
      break;

   }

   action_this_frame = eStateAction::none; // reset trigger
   return result; // allow last chance handling at caller

}
