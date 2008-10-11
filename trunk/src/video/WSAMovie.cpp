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
#include "misc/config.h"
#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "video/GraphicsEngine.h"
#include "include/imageproc.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "WSAError.h"
#include "WSAHeader.h"
#include "include/Logger.h"
#include "GraphicsEngine.h"

using std::string;
using std::runtime_error;

extern Logger * logger;

/**
 */
WSAMovie::~WSAMovie()
{
    delete[] wsadata;
    delete[] header.offsets;
    delete[] framedata;
}

/**
 * @param grafEngine Graphic engine to show WSA animation
 */
void WSAMovie::animate(GraphicsEngine& grafEngine)
{
    float fps, delay;
    SDL_Rect dest;
    int i;
    SDL_Surface* frame;
    ImageProc scaler;
    
    


    frame = 0;
    dest.w = header.width<<1;
    dest.h = header.height<<1;
    dest.x = (grafEngine.getWidth()-(header.width<<1))>>1;
    dest.y = (grafEngine.getHeight()-(header.height<<1))>>1;
    fps = static_cast<float>((1024.0 / (float) header.delta) * 1024.0);
    delay = static_cast<float>((1.0 / fps) * 1000.0);

    // Clear the screen
    grafEngine.clearScreen();
    
    // check 
    if (header.NumFrames == 0) {
        return;
    }
    
   
    // For every frame: decode it, draw it and wait the delay 
    for (i = 0; i < header.NumFrames; i++)
    {
        // Decode the frame
        frame = decodeFrame(i);
        // Draw it to the screen
        grafEngine.drawVQAFrame(scaler.scale(frame,1));
        // Wait a delay
        SDL_Delay((unsigned int)delay);
    }
    // Release the surface
    SDL_FreeSurface(frame);
}

/**
 * @param framenum Number of the frame to decode in surface
 * @return Surface decoded
 */
SDL_Surface* WSAMovie::decodeFrame(unsigned int framenum)
{
    // Get length of specific frame
    Uint32 frameLength = header.offsets[framenum+1] - header.offsets[framenum];
    Uint8* image80 = new Uint8[frameLength];
    Uint8* image40 = new Uint8[64000]; // Max space. We dont know how big decompressed image will be

    memcpy(image80, wsadata + header.offsets[framenum], frameLength);
    Compression::decode80(image80, image40);
    Compression::decode40(image40, framedata);

    SDL_Surface* tempframe = SDL_CreateRGBSurfaceFrom(framedata, header.width, header.height, 8, header.width, 0, 0, 0, 0);
    SDL_SetColors(tempframe, palette, 0, 256);

    delete[] image40;
    delete[] image80;

    SDL_Surface* frame = SDL_DisplayFormat(tempframe);

    SDL_FreeSurface(tempframe);

    // Return the frame decoded
    return frame;
}

/**
 * @param movieFileName File name of the WSA movie
 */
WSAMovie::WSAMovie(const string& movieFileName)
{
    string fname = movieFileName;
    
    //transform(movieFileName.begin(), movieFileName.end(), )
    
    // Load the animation file from mix archives
    VFile* wfile = VFSUtils::VFS_Open(fname.c_str());
    // Test if their no file
    if (wfile == 0)
    {
    	// Throw a WSA error
        throw WSAError("WSA Animation file not found");
    }
    // copy data of the file
    wsadata = new Uint8[wfile->fileSize()];
    wfile->readByte(wsadata, wfile->fileSize());
    
    // And close
    VFSUtils::VFS_Close(wfile);


    // If there are data in this file
    if (wsadata != 0) 
    {
        // Lets get the header
        header.NumFrames = wsadata[0] + (wsadata[0+1] << 8);
        header.xpos = wsadata[2] + (wsadata[2+1] << 8);
        header.ypos = wsadata[4] + (wsadata[4+1] << 8);
        header.width = wsadata[6] + (wsadata[6+1] << 8);
        header.height = wsadata[8] + (wsadata[8+1] << 8);
        header.delta = wsadata[10] + (wsadata[10+1] << 8) + (wsadata[10+2] << 16) + (wsadata[10+3] << 24);
        header.offsets = new Uint32[header.NumFrames + 2];
        memset(header.offsets, 0, (header.NumFrames + 2) * sizeof(Uint32));
        int j = 14; // start of offsets
        for (int i = 0; i < header.NumFrames + 2; i++) {
            header.offsets[i] = wsadata[j] + (wsadata[j+1] << 8) + (wsadata[j+2] << 16) + (wsadata[j+3] << 24) + 0x300;
            j += 4;
        }

        // Read the palette
        for (int k = 0; k < 256; k++)
        {
            palette[k].r = wsadata[j];
            palette[k].g = wsadata[j + 1];
            palette[k].b = wsadata[j + 2];
            palette[k].r <<= 2;
            palette[k].g <<= 2;
            palette[k].b <<= 2;
            j += 3;
        }

        // framedata contains the raw frame data
        // the first frame has to be decoded over a zeroed frame
        // and then each subsequent frame, decoded over the previous one
        framedata = new Uint8[header.height * header.width];
        memset(framedata, 0, header.height * header.width);

        if (header.offsets[header.NumFrames + 1] - 0x300) 
        {
            loop = true;
            header.NumFrames += 1; // Add loop frame
        } else {
            loop = false;
        }
    } else {
    	// There are no data to load in this file
        throw WSAError("No data in WSA file " + fname);
    }
}
