#ifndef _engine_log_h_
#define _engine_log_h_

class Log 
{

public:
   enum eLogState 
   {  _erase = 0x00,  // black 0, 7, 8, f
      _array = 0x09,  // blue 1, 9
      _success,       // green 2, a
      _info,          // teal 3, b
      _error,         // red 4, c
      _message,       // purple 5, d
      _warning,       // yellow 6, e
      _default,       // dark grey 7, f
      //_countStates
   };

   Log();

   void begin(const char* msg, eLogState _state);
   void end(const char* msg = "");

   void entry(const char* msg, eLogState _state, const char* header);

   // formatted vector print
   // void vec2(glm::vec2 v);
   // void vec3(glm::vec3 v);
   // void vec4(glm::vec4 v);
   // void mat4(glm::mat4 m);
  
private:

   HANDLE hConsole = nullptr;
   eLogState state = eLogState::_default;
};

static Log _log;

#endif
