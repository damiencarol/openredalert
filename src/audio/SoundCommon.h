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


namespace {
    SDL_AudioCVT monoconv;
    SDL_AudioCVT eightbitconv;

    bool initconv = false;

    Uint8 chunk[SOUND_MAX_CHUNK_SIZE];
    Uint8 tmpbuff[SOUND_MAX_UNCOMPRESSED_SIZE * 4];
}

namespace {

    //SoundDecoder decoder;
    SoundFile soundDecoder;
    SoundFile musicDecoder;
}

namespace Sound {



//const int Indexes[8] = {-1,-1,-1,-1,2,4,6,8};
const int Indexes[16] = {
		-1, -1, -1, -1, 2, 4, 6, 8,
		-1, -1, -1, -1, 2, 4, 6, 8
};

// Decode Westwood's ADPCM format.  Original code from ws-aud.txt by Asatur V. Nazarian
#define HIBYTE(word) ((word) >> 8)
#define LOBYTE(word) ((word) & 0xFF)

}

#endif /* SOUNDCOMMON_H */
