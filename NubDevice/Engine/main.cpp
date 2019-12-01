#include <pch.h>
#include "input.h"
#include "game_state/state_manager.h"

#include "sound.h"   // wip: external interface (access by name is shit)

// library dependencies included
// http://galogen.gpfault.net/galogen-web.html online ogl function pointer generator
// https://github.com/jkuhlmann/gainput gainput library
// reference material
// https://learnopengl.com/Getting-started/Hello-Triangle
// https://www.khronos.org/opengl/wiki/Detecting_the_Shader_Model // detect shading language
//
// http://vodacek.zvb.cz/archiv/681.html fix your time step
// https://www.youtube.com/watch?v=8ZiB-YqZWpY&list=PL7wAPgl1JVvVJabcmRyEwg8k9rFxCRU9x&index=5 coding math (tween 3)
//

Display display(800, 600);

game_state::StateManager game_state_manager; 
Timer timer; // Application delta timer

Input input; 
Sound sound;


bool isAppInitialized = false;
int main(int argc, char** argv) // -------------------------------------- app entry -----------------------
{  _log.entry(timestamp(), _log._message, "Application Startup - ");
   
   ALuint buffer_menuMood = sound.add("spaceship_cockpit");
   ALuint source_menuMood = sound.attach(buffer_menuMood);//1.0f, 1.0f, glm::vec3(0.0f), glm::vec3(0.0f));
   alSourcePlay(source_menuMood);

   display.Create("NubDevice");
   input.setClientDimensions(display.dims);
   
   game_state_manager.initialize(display.dims);
 
 
   isAppInitialized = true; // allow system pings on local objects and go... 
   _log.begin("\nEntering main loop", _log._message); std::cout<<timestamp(); _log.end();
   MSG msg = { 0 };

   double target_tick_time = 0.333; // ~30fps
   double tick_accumulator = 0.0;
   double delta_time;
   timer.start();
   while (1)
   {  delta_time = timer.end(); // end repeater
      tick_accumulator += delta_time;
      // user input and os messages
      input.manager.Update();   
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
         if(msg.message == WM_QUIT) { break; } 
         else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            input.manager.HandleMessage(msg);
         }
      } // end system message processing
      game_state_manager.handleEvents();
      game_state_manager.updateInput((float)delta_time);
     
      // fixed update
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      while(tick_accumulator >= target_tick_time)
      {  if(game_state_manager.isRunning()) // integrate ticks
         {  game_state_manager.updateWorldTick((float)target_tick_time);
            tick_accumulator -= target_tick_time;
            //_log.entry("update", _log._warning, "... ");
         }
      }  // http://vodacek.zvb.cz/archiv/681.html Fixed your time step
      
      // application sliding render state  
      const double time_alpha = tick_accumulator / target_tick_time;
      if (game_state_manager.isRunning())
      {  game_state_manager.draw((float)time_alpha); // tween state. prepare for multi-playgazm     
         SwapBuffers(display.hdc);
         //_log.begin("... draw ", _log._warning); printf_s("%.3f", time_alpha); _log.end();
      }
   }


   // --------------------------- clean up -----------------------------------
   game_state_manager.shutdown();

   alDeleteBuffers(1, &buffer_menuMood); // user created audio memory
   alDeleteSources(1, &source_menuMood);

   // todo : check for message leaks
   return (int)msg.wParam; 
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_CREATE:
      break;

   case WM_CLOSE:
     _log.entry(timestamp(), _log._default, "Application Close: ");
      PostQuitMessage(0);
      break;

   case WM_QUIT:
   case WM_DESTROY:
     _log.begin("End Log ", _log._default); std::cout << timestamp(); _log.end("");
      break;

   case WM_SIZE:
      // todo : notify display of size change  // mwk: locking to initialized dimension.  
     _log.begin("Viewport Size : ", _log._info); std::cout << display.dims.width << ", " << display.dims.height; _log.end();
      glViewport(0, 0, (GLsizei)display.dims.width, (GLsizei)display.dims.height);
      return 0;

   case WM_MOVE:
      if(isAppInitialized) { game_state_manager.pushEvent(game_state::eStateAction::moveWindow); }
      break;

   default:
      return DefWindowProc(hWnd, message, wParam, lParam);

   }
   return 0;
}
