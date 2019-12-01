#include <pch.h>
#include "state_manager.h"

#include "menu_state.h"
#include "game_state.h"

using namespace game_state;

Menu_state* menuState;
Game_state* gameState;



void game_state::StateManager::initialize(Display::sDims clientDimensions)
{
   
   menuState = new game_state::Menu_state(clientDimensions, &shaders);
   gameState = new game_state::Game_state(clientDimensions, &shaders);
   // characterCustomizationtate 
   // userPorfileState
   // loginState
   // graphicsOptionsState
   // userInputConfiguration

   states.push_back(menuState); //push root
   running = true;
}

void StateManager::shutdown()
{  running = false;
  _log.entry("begin state shutdown", _log._message, "\n");
   
   menuState->shutdown();
   delete menuState; menuState=nullptr;
  _log.entry("end menu shutdown", _log._message, "");
  
   gameState->shutdown();
   delete gameState; gameState=nullptr;
   _log.entry("end game shutdown", _log._message, "");

  
}

 
void StateManager::changeState(Base_state* _state) { }
void StateManager::pushState(Base_state* _state)   { }
void StateManager::popState()                        { states.pop_back(); }
void StateManager::updateInput(float delta_time)     { states.back()->updateInput(delta_time); }
void StateManager::updateWorldTick(float delta_time) { states.back()->updateWorld(delta_time); }
void StateManager::draw(float time_alpha)            { states.back()->draw(time_alpha); }


void StateManager::handleEvents()
{  
   eStateAction action = states.back()->handleEvents();
   if (action != eStateAction::none)
   {  switch (action)
      {
      
      case eStateAction::pause:
         if (states.back()->name == "game_state") { states.pop_back(); }
         break;

      case eStateAction::play:
         states.push_back(gameState);

          states.back()->profile_timer.start(); // fps temp
         
         break;

      case eStateAction::quit:
         if (states.back()->name == "game_state") { states.pop_back(); }
         break;            

      }
   }
 
}
   
