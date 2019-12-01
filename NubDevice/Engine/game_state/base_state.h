#ifndef _engine_game_state_base_game_state_h_
#define _engine_game_state_base_game_state_h_

namespace game_state 
{
   enum eStateAction
   {
      pause,
      play,
      levelUp,
      win,
      loose,
      quit,
      moveWindow,
      none
   };

   // abstract
   class Base_state
   {

   public:
      Base_state(std::string _name, Display::sDims _clientDimensions)
      {
         name = _name;
         clientDimensions = _clientDimensions;
      }

      virtual void initialize() = 0;
      virtual void shutdown() = 0;

      virtual void pause() = 0;
      virtual void resume() = 0;

      virtual eStateAction handleEvents() = 0;
      virtual void updateInput(float delta_time) = 0;
      virtual void updateWorld(float delta_time) = 0;
      virtual void draw(float time_alpha) = 0;

      std::string name;
      Display::sDims clientDimensions;

      eStateAction action_this_frame = eStateAction::none;

      virtual float getTimerInfo() = 0; // profiler
      Timer profile_timer;

   protected:
      Base_state() { /* empty constructor */ }
           
   };

}


#endif


