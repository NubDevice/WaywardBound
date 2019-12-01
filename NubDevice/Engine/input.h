#ifndef _engine_input_h_
#define _engine_input_h_

#include <gainput/gainput.h>

enum eInputAction
{
   mouseX,
   mouseY,
   cancel,
   confirm,
   selecting,
   gizmo_translation,
   increment,
   decrement,
   num_actions
}; // actions extended per class need resuming from num_actions

class Input
{

public:
   Input() = default;

   void setClientDimensions(Display::sDims client_dimensions)
   {
      manager.SetDisplaySize(client_dimensions.width, client_dimensions.height);
      keyboard = manager.CreateDevice<gainput::InputDeviceKeyboard>();
      mouse    = manager.CreateDevice<gainput::InputDeviceMouse>();
      gamepad  = manager.CreateDevice<gainput::InputDevicePad>();

      static gainput::InputMap inputMap(manager);
      inputMap.MapFloat(mouseX, mouse, gainput::MouseAxisX);
      inputMap.MapFloat(mouseY, mouse, gainput::MouseAxisY);
      
      inputMap.MapBool(confirm, keyboard, gainput::KeyReturn);
      inputMap.MapBool(confirm, gamepad,  gainput::PadButtonA);
      inputMap.MapBool(confirm, mouse,    gainput::MouseButtonLeft);

      inputMap.MapBool(cancel, keyboard, gainput::KeyEscape);
      inputMap.MapBool(cancel, gamepad,  gainput::PadButtonY);
      
      inputMap.MapBool(selecting, mouse, gainput::MouseButtonRight);
      
      pAction_map = &inputMap;
      // gainput sample directory for 'how to' goodness.
   }

   gainput::DeviceId keyboard;
   gainput::DeviceId mouse;
   gainput::DeviceId gamepad;
   
   gainput::InputManager manager;
   gainput::InputMap* pAction_map;

private:   

};

#endif
