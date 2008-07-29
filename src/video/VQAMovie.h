// VQAMovie.h
// 1.0

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

#ifndef VQAMOVIE_H
#define VQAMOVIE_H

#include "SDL/SDL_audio.h"
#include "SDL/SDL_mutex.h"
#include "SDL/SDL_video.h"

#include "VQAHeader.h"

class VFile;
class VQAHeader;

namespace VQA {

/**
 * Movie in VQA format
 */
class VQAMovie
{
public:
	/** Build a movie */
    VQAMovie(const char* filename);
    ~VQAMovie();
    
    /** Play the movie */
    void play();
    
private:
    VQAMovie();

    bool ReadChunk ();
    bool DecodeFORMChunk(); // Decodes FORM Chunk - This has to return true to continue
    bool DecodeFINFChunk(); // This has to return true to continue
    Uint32 DecodeSNDChunk(Uint8* outbuf);
    bool DecodeVQFRChunk(SDL_Surface* frame);
    inline void DecodeCBPChunk();
    inline void DecodeVPTChunk(Uint8 Compressed);
    inline void DecodeCBFChunk(Uint8 Compressed);
    inline void DecodeCPLChunk(SDL_Color* palette);
    inline void DecodeUnknownChunk();

    static void AudioHook(void* userdata, Uint8* stream, int len);

    /** File in mix archives */
    VFile* vqafile;
    /** Header of the movide */
    VQAHeader header;
    
    // VQA Video Related Variables
    Uint32 CBPOffset;
    Uint16 CBPChunks;
    Uint8* CBF_LookUp;
    Uint8* CBP_LookUp;
    Uint8* VPT_Table;
    Uint32* offsets;
    Uint8 modifier;
    Uint32 lowoffset;
    // VQA Sound Related Variables
    Sint32 sndindex;
    Sint32 sndsample;

    int scaleVideo; 
    int videoScaleQuality;

    // Buffer to hold ~15 audio frames
    Uint8* sndbuf; // The whole buffer
    Uint8* sndbufMaxEnd; // The max end of the malloced size of the buffer
    Uint8* sndbufStart; // The current start into the buffer
    Uint8* sndbufEnd; // The current end in the buffer

	Uint8 BlockS;		// Block size w * h

    // Semaphores to allow sync
    SDL_sem* empty;
    SDL_sem* full;
    SDL_mutex* sndBufLock;

    // Used to convert VQA audio to correct format
    SDL_AudioCVT cvt;
};

}

#endif //VQAMOVIE_H
