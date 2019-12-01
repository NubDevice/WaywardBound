#include <pch.h>

#include "../../input.h"
#include "../../loaders/scene.h"
#include "../../camera.h"
#include "../../gui/gizmo.h"

#include "config_input.h"
#include "../game_state.h"

void game::config_input(Input* input)
{
   input->pAction_map->MapBool(gizmo_translation, input->keyboard, gainput::KeyT);
   input->pAction_map->MapBool(forward, input->keyboard, gainput::KeyW);
   input->pAction_map->MapBool(backward, input->keyboard, gainput::KeyS);
   input->pAction_map->MapBool(left, input->keyboard, gainput::KeyA);
   input->pAction_map->MapBool(right, input->keyboard, gainput::KeyD);
   input->pAction_map->MapBool(up, input->keyboard, gainput::KeyQ);
   input->pAction_map->MapBool(down, input->keyboard, gainput::KeyE);
   input->pAction_map->MapBool(jump, input->keyboard, gainput::KeySpace);
   input->pAction_map->MapBool(save, input->keyboard, gainput::KeyF11);
   input->pAction_map->MapBool(increment, input->keyboard, gainput::KeyUp);
   input->pAction_map->MapBool(decrement, input->keyboard, gainput::KeyDown);
}

//int selected_item_id = 0;

#define imp_bool(input, x) if(input->pAction_map->GetBool(x))
#define imp_float(input, x) if(input->pAction_map->GetFloatDelta(x) != 0.0f)
#define imp_bool_new(input, x) if(input->pAction_map->GetBoolIsNew(x))
POINT click_screen_pos;
game_state::eStateAction game::update_input(float delta_time, game_state::eStateAction current_action, 
                                          Input* input, Display* display,
                                          Loaders::Scene* scene, Camera* camera, gui::Gizmo* gizmo)
{
   game_state::eStateAction result_action = current_action;
   
   // user input pump --------------------------------------
   imp_bool(input, cancel) { result_action = game_state::quit; }

   // camera -----------------------------------------------
   imp_bool(input, game::forward) { camera->move(camera->FORWARD, delta_time); }
   imp_bool(input, game::backward) { camera->move(camera->BACKWARD, delta_time); }
   imp_bool(input, game::left) { camera->move(camera->LEFT, delta_time); }
   imp_bool(input, game::right) { camera->move(camera->RIGHT, delta_time); }
   imp_bool(input, game::up) { camera->move(camera->UP, delta_time); }
   imp_bool(input, game::down) { camera->move(camera->DOWN, delta_time); }
   imp_bool(input, game::jump) { }

   imp_bool_new(input, increment) { //selected_item_id++; 
      if (++gizmo->selected_item_id >= (int)scene->renderables[0].items.size())
         gizmo->selected_item_id = 0;
   }
   imp_bool_new(input, decrement) { //selected_item_id--; 
      if (--gizmo->selected_item_id < 0)
         gizmo->selected_item_id = (int)scene->renderables[0].items.size() - 1;
   }

   imp_bool_new(input, game::save) { scene->save(); }

   float x = input->pAction_map->GetFloat(mouseX);
   float y = input->pAction_map->GetFloatDelta(mouseY);

   // screen edge wrapping ---------------------------------
   POINT pt; ::GetCursorPos(&pt); // keep y component
   if (x < 0) { // left and right screen edge cursor wrapping
      POINT pt_desired = { display->dims.width, 0 }; ::ClientToScreen(display->hwnd, &pt_desired);
      ::SetCursorPos(pt_desired.x - 2, pt.y); // todo: refactor out the magic number
   }
   if (x > 0.9985f) { // partial cause of refactor above (but it's decent smooth)
      POINT pt_desired = { 0, 0 }; ::ClientToScreen(display->hwnd, &pt_desired);
      ::SetCursorPos(pt_desired.x, pt.y);
   }

   // picking ----------------------------------------------
   if (input->pAction_map->GetBoolIsNew(confirm))
   {  // click select toggle
      ::GetCursorPos(&click_screen_pos);
      if (!gizmo->bObj_selected)
      {
         gizmo->bObj_selected = true;     // activate widget
         gizmo->is_axis_hover = glm::ivec3(0); // clear state
         gizmo->is_axis_active = 0;       //
      }
      else {
         // maintain id of current state
         if (gizmo->is_axis_hover.x == 1)      gizmo->is_axis_active = 1;
         else if (gizmo->is_axis_hover.y == 1) gizmo->is_axis_active = 2;
         else if (gizmo->is_axis_hover.z == 1) gizmo->is_axis_active = 3;
         else {
            gizmo->bObj_selected = false; // deactivate widget if not being manipulated
         }
      }
   }

   // pffftttt....all this choking garbage needs to go. 
   // toss a litte dirt on it (Nov11,2019)
   static bool was_dragging = false;
   static bool let_blank = true;
   static float xdiff = 0.f;
   static float ydiff = 0.f;
   if (gizmo->is_dragging) {
      was_dragging = true;
      ydiff = y;// track last
   }
   else
   {
      if (was_dragging)
      {
         was_dragging = false;
         POINT temp; ::GetCursorPos(&temp);
         xdiff = float(click_screen_pos.x - temp.x);
         //ydiff = (click_screen_pos.y-temp.y)/display.dims.height;
         ::SetCursorPos(click_screen_pos.x, temp.y);//click_screen_pos.y);
         let_blank = true;
      }
      else
      {
         if (let_blank)
            let_blank = false;
         else
            camera->ProcessMouseMovement(x * 360.0f, y, true);
      }
   }


   //
   // todo: take into account the physical velocity of the mouse movement
   // high velocity user movement skips a beat at edge transition
   // solution: track previous movement and apply
   // priority: barely notice-able
   //

   return result_action;
}