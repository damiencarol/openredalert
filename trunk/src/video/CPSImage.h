// CPSImage.h
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

#ifndef CPSIMAGE_H
#define CPSIMAGE_H

#include "SDL/SDL_video.h"

#include "include/imageproc.h"
#include "CPSHeader.h"

/**
 * Image in .CPS format
 *
 * The .CPS files contain 320x200x256 images. The images are compressed with the
 * Format80 compression method. They may or may not contain a palette.
 *
 * The header has the following structure :
 * Header : record
 *     Size    : word;  {File size - 2}
 *     Unknown : word;  {Always 0004h}
 *     ImSize  : word;  {Size of uncompressed image (always 0FA00h)}
 *     Palette : longint; {Is there a palette ?}
 * end;
 *
 * If Palette is 03000000h then there's a palette after the header, otherwise
 * the image follows.
 * CPS file without palette can be found in the SETUP.MIX file, and they all use
 * the Palette that can be found inside the same .MIX.
 *
 * The image that follows the palette (or the Header) is in Format80 which is
 * explained above.
 */
class CPSImage
{
public:
    CPSImage(const char* fname, int scaleq);
    ~CPSImage();
    
    /** Get a SDL_Surface of the image */
    SDL_Surface* getImage();
    
private:
    /** Read the image and store in a surface */
    void loadImage();
    /** Read the palette in data */
    void readPalette();
    
    /** Size of the image */
    Uint32 imgsize;
    /** Offset in file to the image */
    Uint32 offset;
    /** Raw data of the file loaded */
    Uint8* cpsdata;
    /** Internal palette of the image */
    SDL_Color palette[256];
    
    /** Header of the file */
    CPSHeader lnkHeader;
    int scaleq;
    ImageProc lnkScaler;
    SDL_Surface* image;
};

#endif //CPSIMAGE_H
