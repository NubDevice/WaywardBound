#ifndef _engine_game_state_game_config_input_h_
#define _engine_game_state_game_config_input_h_

#include "../game_state.h"

namespace game
{
   enum eInputActionEx
   {
      forward = num_actions,
      backward,
      left,
      right,
      up,
      down,
      jump,
      save,
      num_actionsEx,
   };

   void config_input(Input* input);
   game_state::eStateAction update_input(float delta_time, game_state::eStateAction current_action,
                                         Input* input, Display* display, 
                                         Loaders::Scene* scene, Camera* camera, gui::Gizmo* gizmo);
}

#endif

