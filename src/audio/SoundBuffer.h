#ifndef SOUNDBUFFER_H
#define SOUNDBUFFER_H

#include <vector>

#include "SDL/SDL_types.h"

#ifdef RA_SOUND_ENGINE
#include "SDL/SDL_mixer.h"
#endif

typedef std::vector<Uint8> SampleBuffer;

struct SoundBuffer {
    SampleBuffer data;
    #ifdef RA_SOUND_ENGINE
    Mix_Chunk * chunk;
    #endif
};

#endif
