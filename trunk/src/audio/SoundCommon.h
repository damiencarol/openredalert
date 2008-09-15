// SoundCommon.h
// 0.4

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SOUNDCOMMON_H
#define SOUNDCOMMON_H

#include <map>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_audio.h"
#include "SDL/SDL_mixer.h"

#include "SoundCache.h"
#include "SoundBuffer.h"
#include "SoundFile.h"

#define SOUND_FORMAT    AUDIO_S16SYS
#define SOUND_CHANNELS  2
#define SOUND_FREQUENCY 22050

#define SOUND_MAX_CHUNK_SIZE        16384
#define SOUND_MAX_UNCOMPRESSED_SIZE (SOUND_MAX_CHUNK_SIZE << 2)
#define SOUND_MAX_COMPRESSED_SIZE   SOUND_MAX_CHUNK_SIZE

//using p::second;

//namespace p {
 //   extern

//class SoundFile;


namespace {
    SDL_AudioCVT monoconv;
    SDL_AudioCVT eightbitconv;

    bool initconv = false;

    Uint8 chunk[SOUND_MAX_CHUNK_SIZE];
    Uint8 tmpbuff[SOUND_MAX_UNCOMPRESSED_SIZE * 4];
}

namespace {

struct SoundCacheCleaner : public std::unary_function<SoundCache::value_type, void>
{
    void operator()(const SoundCache::value_type& p) {
        Mix_FreeChunk(p.second->chunk);
        delete p.second;
    }
};


    //SoundDecoder decoder;
    SoundFile soundDecoder;
    SoundFile musicDecoder;
}

namespace Sound {

const int Steps[89] = {
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,
    34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,
    157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,
    724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,
    2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,
    9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,
    27086,29794,32767
};

//const int Indexes[8] = {-1,-1,-1,-1,2,4,6,8};
const int Indexes[16] = {
		-1, -1, -1, -1, 2, 4, 6, 8,
		-1, -1, -1, -1, 2, 4, 6, 8
};

// Decode Westwood's ADPCM format.  Original code from ws-aud.txt by Asatur V. Nazarian
#define HIBYTE(word) ((word) >> 8)
#define LOBYTE(word) ((word) & 0xFF)

const int WSTable2bit[4] = {-2,-1,0,1};
const int WSTable4bit[16] = {
    -9, -8, -6, -5, -4, -3, -2, -1,
     0,  1,  2,  3,  4,  5,  6,  8
};



}

#endif /* SOUNDCOMMON_H */
