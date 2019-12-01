#include <pch.h>

Timer::Timer()
{  t_begin = std::chrono::steady_clock::now();
   deltaTime = std::chrono::duration<double>::zero();
   t_last = t_begin;
}

void Timer::start()
{  t_begin = std::chrono::steady_clock::now();
   t_last = t_begin;
}

double Timer::end()
{  t_end = std::chrono::steady_clock::now();

   deltaTime = t_end - t_last;
   t_last = t_end;
   return deltaTime.count();
}

void Timer::log()
{  _log.begin("cycle time: ", _log._message);
   
   std::cout << "(" << end() << ") " << timestamp();
}

