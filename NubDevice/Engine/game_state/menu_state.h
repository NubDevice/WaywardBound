#ifndef _engine_game_state_menu_state_h_
#define _engine_game_state_menu_state_h_

#include "base_state.h"

#include "../mesh.h"
#include "../gui/gui.h"


namespace game_state {

   class Menu_state : public Base_state
   {
   public:
      Menu_state(Display::sDims clientDimensions, Shader* psh) 
         : Base_state("menu_state", clientDimensions) 
         { 
            pShaders = psh;
            initialize(); 
         }
      
      void initialize();
      void shutdown();

      void pause();
      void resume();

      eStateAction handleEvents();
      void updateInput(float delta_time);
      void updateWorld(float delta_time);
      void draw(float time_alpha);
      float getTimerInfo();
      

   private:
    
    // game objects
    Shader* pShaders;

   };
}

#endif


