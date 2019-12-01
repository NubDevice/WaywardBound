#ifndef _engine_game_state_state_manager_h_
#define _engine_game_state_state_manager_h_

#include "base_state.h"

namespace game_state 
{

   class StateManager
   {

   public:
      void initialize(Display::sDims clientDimensions);
      void shutdown();

      void changeState(Base_state* _state);
      void pushState(Base_state* _state);
      void popState();

      void handleEvents();
      void pushEvent(eStateAction e) { states.back()->action_this_frame = e; }

      void updateInput(float delta_time);
      void updateWorldTick(float delta_time);
      void draw(float time_alpha);

      bool isRunning() { return running;  }
      void quit()      { running = false; }

      // GUI DRIVEN ----------------
      double dVolume = 0.15; // right and left?                // todo : panning
      double difficulty = 0.5;  // 0.0 --> 1.0 maps to 0.4 --> 2.5 time scale
      double dSlider01 = 0.0;
      double dSlider02 = 0.0;
      // END DRIVEN -----------------

      // core / share --------------------------------------------------------
      std::vector<Base_state*> states;    // library of game state controllers

      Shader shaders;


   private:
      bool running;

      
   };

}

#endif

