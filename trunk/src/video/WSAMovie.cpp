// WSAMovie.cpp
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

#include "WSAMovie.h"

#include <stdexcept>
#include <string>

#include "SDL/SDL_timer.h"
#include "SDL/SDL_video.h"

#include "Renderer.h"
#include "include/config.h"
#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "video/GraphicsEngine.h"
#include "include/imageproc.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "audio/SoundEngine.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "WSAError.h"
#include "WSAHeader.h"
#include "include/Logger.h"
#include "GraphicsEngine.h"

using std::string;
using std::runtime_error;

namespace pc {
	/** SoundEngine of the game */
	extern SoundEngine* sfxeng;
}
extern Logger * logger;

/**
 */
WSAMovie::~WSAMovie()
{
    delete[] wsadata;
    delete[] header.offsets;
    delete[] framedata;
    delete[] sndfile;
}

/**
 */
void WSAMovie::animate(GraphicsEngine* grafEngine)
{
    float fps, delay;
    SDL_Rect dest;
    int i;
    SDL_Surface* frame;
    ImageProc scaler;
    
    


    frame = 0;
    dest.w = header.width<<1;
    dest.h = header.height<<1;
    dest.x = (grafEngine->getWidth()-(header.width<<1))>>1;
    dest.y = (grafEngine->getHeight()-(header.height<<1))>>1;
    fps = static_cast<float>((1024.0 / (float) header.delta) * 1024.0);
    delay = static_cast<float>((1.0 / fps) * 1000.0);

    grafEngine->clearScreen();
    // queue sound first, regardless of whats in the buffer already
    if (header.NumFrames == 0) {
        return;
    }
    
    // Play sound if present
    if (sndfile != 0){
        pc::sfxeng->PlaySound(sndfile);
    }
    
    for (i = 0; i < header.NumFrames; i++) {
        /* FIXME: fill buffer a little before zero to prevent
         * slight audio pause? what value to use?
         */
        // if (pc::sfxeng->getBufferLen() == 0 && sndfile != NULL) {
            pc::sfxeng->PlaySound(sndfile);
        // }
        frame = decodeFrame(i);
        grafEngine->drawVQAFrame(scaler.scale(frame,1));
        SDL_Delay((unsigned int)delay);
    }
    SDL_FreeSurface(frame);

    /* Perhaps an emptyBuffer function to empty any left over sound
     * in the buffer after the wsa finishes?
     */
}

SDL_Surface* WSAMovie::decodeFrame(Uint16 framenum)
{
    Uint32 frameLength;
    Uint8 *image40;
    Uint8 *image80;
    SDL_Surface *tempframe;
    SDL_Surface *frame;

    // Get length of specific frame
    frameLength = header.offsets[framenum+1] - header.offsets[framenum];
    image80 = new Uint8[frameLength];
    image40 = new Uint8[64000]; /* Max space. We dont know how big
                                               decompressed image will be */

    memcpy(image80, wsadata + header.offsets[framenum], frameLength);
    Compression::decode80(image80, image40);
    Compression::decode40(image40, framedata);

    tempframe = SDL_CreateRGBSurfaceFrom(framedata, header.width, header.height, 8, header.width, 0, 0, 0, 0);
    SDL_SetColors(tempframe, palette, 0, 256);

    delete[] image40;
    delete[] image80;

    frame = SDL_DisplayFormat(tempframe);

    SDL_FreeSurface(tempframe);

    return frame;
}

WSAMovie::WSAMovie(string fname)
{
    int i;
    int j;
    INIFile* wsa_ini = 0;
    VFile* wfile = 0;

    // Load the animation file from mix archives
    wfile = VFSUtils::VFS_Open(fname.c_str());
    // Test if their no file
    if( wfile == 0 ) {
    	// Throw a WSA error
        throw WSAError("WSA Animation file not found");
    }
    // copy data of the file
    wsadata = new Uint8[wfile->fileSize()];
    wfile->readByte(wsadata, wfile->fileSize());
    
    // And close
    VFSUtils::VFS_Close(wfile);


    // Read which sound needs to be played
    try {
        wsa_ini = GetConfig("wsa.ini");
    } catch(runtime_error&) {
        throw WSAError("wsa.ini not found.");
    }

    sndfile = wsa_ini->readString(fname.c_str(), "sound");

    // If there are data in this file
    if (wsadata != 0) {
        // Lets get the header
        header.NumFrames = wsadata[0] + (wsadata[0+1] << 8);
        header.xpos = wsadata[2] + (wsadata[2+1] << 8);
        header.ypos = wsadata[4] + (wsadata[4+1] << 8);
        header.width = wsadata[6] + (wsadata[6+1] << 8);
        header.height = wsadata[8] + (wsadata[8+1] << 8);
        header.delta = wsadata[10] + (wsadata[10+1] << 8) + (wsadata[10+2] << 16) + (wsadata[10+3] << 24);
        header.offsets = new Uint32[header.NumFrames + 2];
        memset(header.offsets, 0, (header.NumFrames + 2) * sizeof(Uint32));
        j = 14; // start of offsets
        for (i = 0; i < header.NumFrames + 2; i++) {
            header.offsets[i] = wsadata[j] + (wsadata[j+1] << 8) + (wsadata[j+2] << 16) + (wsadata[j+3] << 24) + 0x300;
            j += 4;
        }

        // Read the palette
        for (i = 0; i < 256; i++) {
            palette[i].r = wsadata[j];
            palette[i].g = wsadata[j+1];
            palette[i].b = wsadata[j+2];
            palette[i].r <<= 2;
            palette[i].g <<= 2;
            palette[i].b <<= 2;
            j += 3;
        }

        // framedata contains the raw frame data
        // the first frame has to be decoded over a zeroed frame
        // and then each subsequent frame, decoded over the previous one
        framedata = new Uint8[header.height * header.width];
        memset(framedata, 0, header.height * header.width);

        if (header.offsets[header.NumFrames + 1] - 0x300) {
            loop = 1;
            header.NumFrames += 1; // Add loop frame
        } else {
            loop = 0;
        }
    } else {
    	// There are no data to load in this file
        throw WSAError("No data in WSA file " + fname);
    }
}
