// TemplateImage.cpp
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

#include "TemplateImage.h"

#include <string>
#include <stdexcept>

#include "SDL/SDL_types.h"

#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "include/imageproc.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "video/ImageNotFound.h"
#include "include/config.h"
#include "video/headerformats.h"
#include "SHPImage.h"
#include "vfs/VFile.h"
#include "vfs/vfs.h"

using std::string;
using std::runtime_error;

TemplateImage::TemplateImage(const char *fname, Sint8 scaleq, bool ratemp)
    : SHPBase(fname, scaleq), ratemp(ratemp)
{
    tmpfile = VFSUtils::VFS_Open(fname);
    if (tmpfile == NULL){
        throw ImageNotFound("Can't load TemplateImage file " + string(fname));
    }
}

TemplateImage::~TemplateImage()
{
	VFSUtils::VFS_Close(tmpfile);
}

Uint16 TemplateImage::getNumTiles()
{
    Uint16 data;
    tmpfile->seekSet(4);
    tmpfile->readWord(&data, 1);
    return data;
}

SDL_Surface* TemplateImage::getImage(Uint16 imgnum)
{
    // Read width, hight and number of tiles in template
    Uint16 imgwidth;
    Uint16 imgheight;
    Uint16 numtil;

    tmpfile->seekSet(0);
    tmpfile->readWord(&imgwidth, 1);
    tmpfile->readWord(&imgheight, 1);
    tmpfile->readWord(&numtil, 1);

    if (imgnum >= numtil)
        return NULL;

    // Skip some constants
    if (ratemp)
        tmpfile->seekCur(10);
    else
        tmpfile->seekCur(6);

    // Load the offset to the image
    Uint32 imgStart;
    tmpfile->readDWord(&imgStart, 1);

    // Skip some constants
    if (ratemp){
        tmpfile->seekCur(16);
    }else{
        tmpfile->seekCur(12);
    }
    
    // Load address of index1
    Uint32 index1;
    tmpfile->readDWord(&index1, 1);

    // Read the index1 value of the tile
    Uint8 index1val;
    tmpfile->seekSet(index1+imgnum);
    tmpfile->readByte(&index1val, 1);

    if (index1val == 0xff){
        return NULL;
    }

    // Seek the start of the image
    tmpfile->seekSet(imgStart+imgwidth*imgheight*index1val);

    // allocate space for the imagedata and load it
    Uint8* imgdata = new Uint8[imgwidth*imgheight];
    tmpfile->readByte(imgdata, imgwidth*imgheight);

    // The image is made up from the data
    SDL_Surface* sdlimage = SDL_CreateRGBSurfaceFrom(imgdata, imgwidth, imgheight, 8, imgwidth, 0, 0, 0, 0);
    SDL_Surface* retimage;

    // Set the palette to be the map's palette
    SDL_SetColors(sdlimage, palette[0], 0, 256);
    SDL_SetColorKey(sdlimage, SDL_SRCCOLORKEY, 0);

    if (scaleq >= 0) {
        retimage = scale(sdlimage, scaleq);
        SDL_SetColorKey(sdlimage, SDL_SRCCOLORKEY, 0);
    } else {
        retimage = SDL_DisplayFormat(sdlimage);
    }

    SDL_FreeSurface(sdlimage);
    delete[] imgdata;

    return retimage;
}
