#ifndef _engine_timer_h_
#define _engine_timer_h_

#include <chrono>
#include <iomanip> // std::put_time

class Timer 
{

public:
   Timer();

   void start();
   double end();

   void log();

private:
   std::chrono::time_point<std::chrono::steady_clock> t_begin;
   std::chrono::time_point<std::chrono::steady_clock> t_last;
   std::chrono::time_point<std::chrono::steady_clock> t_end;
   std::chrono::duration<double> deltaTime;

};


static char* timestamp()
{  std::time_t timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   static char tbuf[32];
   ctime_s(tbuf, sizeof(char) * 32, &timeNow);
   return tbuf;
}

#endif
