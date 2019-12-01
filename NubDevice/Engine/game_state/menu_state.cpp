#include <pch.h>
#include "menu_state.h"
#include "../input.h"
#include "../display.h"
#include "../sound.h"

using namespace game_state;

Texture textures;
//Shader shaders;
extern Input input;     // todo:
extern Display display; // todo: global hell?
extern Sound sound;     // todo:


float fullscreenSquare_vertices[] = {
     0.75f,  1.0f, 0.0f,  1.0f, 1.0f,  // top right
     0.75f, -1.0f, 0.0f,  1.0f, 0.0f,  // bottom right
    -0.75f, -1.0f, 0.0f,  0.0f, 0.0f,  // bottom left
    -0.75f,  1.0f, 0.0f,  0.0f, 1.0f   // top left 
};

unsigned int quad_indices[] = { 
    0, 3, 1,   // first triangle
    1, 3, 2    // second triangle
};


// todo : 'slice' up image (gui) into 9x9 grid 
// to expand (any frame size/keep rounded corners)

gui::Button button_play;
gui::Button button_quit;

Mesh quad_mesh("background");

// sound
ALuint buffer_click;
ALuint source_click;

void Menu_state::initialize()
{  _log.entry("initialize", _log._warning, "Menu"); 
   
   //
   // todo: load scripted vertex data
   //

   //
   // make me spript-able. 

   buffer_click = sound.add("buttonClick");
   source_click = sound.attach(buffer_click); // set buffered sound to source ready

   glm::mat4 world = glm::mat4(1.0f);
   button_play.initialize("Play", world);
   
   world = glm::translate(world, glm::vec3(0.0f, -0.17f, 0.0f));
   button_quit.initialize("Quit", world);
   
   quad_mesh.initialize(fullscreenSquare_vertices, quad_indices, 4, 2);

   pShaders->add("default");

   textures.initialize();

   textures.add("gui/interface.png");
   textures.add("screens/helix.png");
   textures.add("brickwall.png");
   textures.add("brickwall_n.png");

   // end load script block
   //

}


void Menu_state::shutdown()
{  _log.entry("shutdown", _log._warning, "Menu");
   quad_mesh.destroy(); // getting myself in trouble with c++ again (research: stack/heap destruction) 
}


void Menu_state::pause()  {} // no op
void Menu_state::resume() {} // no op

eStateAction Menu_state::handleEvents()
{  eStateAction result = eStateAction::none;
   switch(action_this_frame)
   {
   
   case eStateAction::play:
      result = action_this_frame; // allow pass through
   {  RECT rcClient; ::GetClientRect(display.hwnd, &rcClient); // client space
      RECT rcWindow; ::GetWindowRect(display.hwnd, &rcWindow); // screen space
      int frame = ((rcWindow.right-rcWindow.left) - (rcClient.right-rcClient.left)) / 2; //(loose fractional??)
      RECT rcClip = { rcWindow.left + frame - 2, rcWindow.top,
                      rcWindow.right - frame + 5,// left + frame + rcClient.right - rcClient.left,
                      rcWindow.bottom - frame }; // LTRB
      ::ClipCursor(&rcClip); // screen space
   }  break;
   
   case eStateAction::quit:
      PostQuitMessage(0);
      break;
   }

   action_this_frame = eStateAction::none; // reset trigger
   return result; // allow last chance handling at caller
}


void Menu_state::updateInput(float delta_time)
{
}

void Menu_state::updateWorld(float delta_time)
{
}

float Menu_state::getTimerInfo() {return 0.0f;}


void Menu_state::draw(float time_alpha)
{  static bool once = true; if(once){once=false; _log.entry("Begin", _log._warning, "Menu");} 
     
   //
   // Draw Scripted Content here...
   //

   glEnable(GL_BLEND);
      GLuint current_shader_id = pShaders->use("default");
      glm::mat4 mtx_world(1.0f); // draw elements transform (identity)
      
      
      //glm::mat4 scale_mtx; scale_mtx = glm::scale(scale_mtx, glm::vec3(0.4f));
      //scale_mtx[3][0] = -0.8f;


      pShaders->setMatrix("transform", mtx_world);
      glActiveTexture(GL_TEXTURE0);
      textures.use("screens/helix.png");

      quad_mesh.draw(); // background image

      // major todo: paged gui. (grouped display calls)
      // add a shader in : fade_alpha
      // to control menu fakes

      textures.use("gui/interface.png");
      
      pShaders->setMatrix("transform", button_play.world);     
      if(button_play.draw()) { // if clicked
         alSourcePlay(source_click);
         RECT clientRect;
         ::GetClientRect(display.hwnd, &clientRect);
         ::ClipCursor(&clientRect);
         action_this_frame = eStateAction::play; 
      }
      
      pShaders->setMatrix("transform", button_quit.world);
      if(button_quit.draw()) { // if clicked
         alSourcePlay(source_click); 
         action_this_frame = eStateAction::quit;
      }
   glDisable(GL_BLEND);
}

