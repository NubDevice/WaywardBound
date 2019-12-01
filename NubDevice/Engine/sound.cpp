#include <pch.h>
#include "sound.h"

#pragma comment(lib, "openal32.lib") 

Sound::Sound()
{  static bool once = true;
   if(once)
   {  once = false;
      // one time init at startup
      // http://www.voidcn.com/article/p-tgtzyivr-bmn.html
      pALdevice = alcOpenDevice(nullptr);

      if (pALdevice) {
         pALcontext = alcCreateContext(pALdevice, nullptr);
         if (pALcontext)
            alcMakeContextCurrent(pALcontext);
         else {
            alcCloseDevice(pALdevice);
            _log.begin("OpenAL create context failed", _log._error); _log.end();
         }
      }
      else {
         _log.begin("OpenAL create device failed", _log._error); _log.end();
      }

      ALboolean bEAX2_0 = alIsExtensionPresent("EAX2.0");
      alGetError(); // clear error code
   }
}


void Sound::play(const char* name)
{

}

void Sound::play(GLuint id)
{

}


ALuint Sound::add(const char* name)
{  ALuint buffer;
   alGenBuffers(1, &buffer);

   // load a wav
   CWaves* pSoundLoader = new CWaves();
   WAVEID  waveID;
   ALenum  eBufferFormat;
   ALchar* pData;
   ALint   iDataSize;
   ALint   iFrequency;

   WAVERESULT result;
   std::string filePath = "_data/Audio/"; filePath += name; filePath += ".wav";

   if (pSoundLoader)
   {
      //if ((result = pSoundLoader->LoadWaveFile("_data/Audio/buttonClick.wav", &waveID)) == WR_OK)
      if ((result = pSoundLoader->LoadWaveFile(filePath.c_str(), &waveID)) == WR_OK)
      {
         if ((result = pSoundLoader->GetWaveSize(waveID, (unsigned long*)&iDataSize)) == WR_OK)
         {
            if ((result = pSoundLoader->GetWaveData(waveID, (void**)&pData)) == WR_OK)
            {
               if ((result = pSoundLoader->GetWaveFrequency(waveID, (unsigned long*)&iFrequency)) == WR_OK)
               {
                  if ((result = pSoundLoader->GetWaveALBufferFormat(waveID, &alGetEnumValue, (unsigned long*)&eBufferFormat)) == WR_OK)
                  {
                     alGetError(); // clear the error buffer
                     alBufferData(buffer, eBufferFormat, pData, iDataSize, iFrequency);
                     _log.begin("Audio loaded: ", _log._info); std::cout<<name; _log.end();
                     if (alGetError() == AL_NO_ERROR)
                     {
                        pSoundLoader->DeleteWaveFile(waveID);
                        return buffer; 
                     }
                  }
               }
            }
         }
      }
   }
 
   _log.begin("Sound load failure: ", _log._error); std::cout<<name; _log.end();
   return 0;
}

ALuint Sound::attach(ALuint buffer)
{  ALuint source; alGenSources(1, &source);
   alSourcei (source, AL_BUFFER, buffer);
   alSourcef (source, AL_PITCH, 1.0f);
   alSourcef (source, AL_GAIN,  1.0f);
   alSource3f(source, AL_POSITION, 0, 0, 0);
   alSource3f(source, AL_VELOCITY, 0, 0, 0);
   alSourcei (source, AL_LOOPING, AL_FALSE);
   return source;
}


/*void Sound::loadWAV(const char* name)
{
   FILE* file = fopen("audio.wav", "fb");
   char xbuffer[5];
   xbuffer[4] = '\0';
   if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "RIFF") != 0)
      throw "Not a WAV file";

   file_read_int32_le(xbuffer, file);

   if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "WAVE") != 0)
      throw "Not a WAV file";

   if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "fmt ") != 0)
      throw "Invalid WAV file";

   file_read_int32_le(xbuffer, file);
   short audioFormat = file_read_int16_le(xbuffer, file);
   short channels = file_read_int16_le(xbuffer, file);
   int sampleRate = file_read_int32_le(xbuffer, file);
   int byteRate = file_read_int32_le(xbuffer, file);
   file_read_int16_le(xbuffer, file);
   short bitsPerSample = file_read_int16_le(xbuffer, file);

   if (audioFormat != 16) {
      short extraParams = file_read_int16_le(xbuffer, file);
      file_ignore_bytes(file, extraParams);
   }

   if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "data") != 0)
      throw "Invalid WAV file";

   int dataChunkSize = file_read_int32_le(xbuffer, file);
   unsigned char* bufferData = file_allocate_and_read_bytes(file, (size_t)dataChunkSize);

   float duration = float(dataChunkSize) / byteRate;
   alBufferData(buffer, GetFormatFromInfo(channels, bitsPerSample), bufferData, dataChunkSize, sampleRate);
   free(bufferData);
   fclose(file);
}*/