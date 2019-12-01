#ifndef _engine_game_state_game_state_h_
#define _engine_game_state_game_state_h_

#include "base_state.h"
#include "../loaders/scene.h"

namespace game_state {

   class Game_state : public Base_state
   {
   public:
      Game_state(Display::sDims clientDimensions, Shader* psh)
         : Base_state("game_state", clientDimensions) 
         { 
            pShaders = psh;
            initialize(); 
         }
      
      void initialize();
      void shutdown();

      void pause();
      void resume();

      void updateInput(float delta_time);
      void updateWorld(float delta_time);
      void draw(float time_alpha);
      
      eStateAction handleEvents();

      float getTimerInfo();
      
   private:

      Loaders::Scene scene;

      void uploadVFX(const char* name);

      // game objects silver spoones ------------------------------------
      Shader* pShaders;


   };
}

#endif

