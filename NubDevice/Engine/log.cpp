#include <pch.h>

Log::Log() 
{  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Log::begin(const char* msg, eLogState _state)
{  SetConsoleTextAttribute(hConsole, WORD(_state - 0x08));
   std::cout << msg << " ";
   SetConsoleTextAttribute(hConsole, WORD(_state));
   state = _state;
}

void Log::end(const char* msg)
{  SetConsoleTextAttribute(hConsole, WORD(state - 0x08));
   std::cout << " " << msg << "\n";
   SetConsoleTextAttribute(hConsole, 0x08);
   state = eLogState::_default;
}

void Log::entry(const char* msg, eLogState _state = eLogState::_default, const char* header = " ")
{  if (state != _state)
   {  switch (state)
      {
      case _default: SetConsoleTextAttribute(hConsole, 0x0f); break;
      case _array:   SetConsoleTextAttribute(hConsole, 0x09); break; // blue
      case _success: SetConsoleTextAttribute(hConsole, 0x0a); break;
      case _info:    SetConsoleTextAttribute(hConsole, 0x0b); break;
      case _error:   SetConsoleTextAttribute(hConsole, 0x0c); break; // hi intensity += 8
      case _message: SetConsoleTextAttribute(hConsole, 0x0d); break;
      case _warning: SetConsoleTextAttribute(hConsole, 0x0e); break;
      }
   }

   begin(header, _state);
   std::cout << msg << "\n";
   SetConsoleTextAttribute(hConsole, 0x08);
}
