#ifndef _engine_sound_h_
#define _engine_sound_h_

#include <al/al.h>
#include <al/alc.h>
#include <al/alext.h>

#include <al/waves.h>

class Sound
{

public:
   Sound();
   ~Sound() 
   {  alcMakeContextCurrent(nullptr);
      alcDestroyContext(pALcontext); //alcGetCurrentContext()
      alcCloseDevice(pALdevice);     //alcGetContextDevice(pALcotext)
   }

   ALuint add(const char* name); // create buffer from file data
   ALuint attach(ALuint buffer); // create source from buffer

   void play(const char* name);
   void play(GLuint id);

private:
   ALCcontext* pALcontext = nullptr;
   ALCdevice* pALdevice = nullptr;

};

#endif

   // todo : mwk (Jan2020)
   /*/ listener object
   ALfloat listenerPos[] = { 0.0f, 0.0f, 0.0f };
   ALfloat listenerVel[] = { 0.0f, 0.0f, 0.0f };
   ALfloat listenerOri[] = { 0.0f, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f }; // forward/up
   ALfloat listenerGain = 1.0f;
   alListenerf(AL_GAIN, listenerGain);
   alListenerfv(AL_POSITION, listenerPos); // error check
   alListenerfv(AL_VELOCITY, listenerVel); // error check
   alListenerfv(AL_ORIENTATION, listenerOri); // error check
   */

