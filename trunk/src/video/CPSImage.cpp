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

#include "CPSImage.h"

#include <string>

#include "misc/Compression.hpp"
#include "vfs/vfs.h" // to use VFS_Open()
#include "vfs/VFile.h" // to use VFile
#include "video/ImageNotFound.h" // to use ImageNotFound

using std::string;

/**
 * @param fname Name of the file to load
 * @param scaleq Scale factor, if caleq = -1 there are no scale factor
 */
CPSImage::CPSImage(const char* fname, int scaleq) :
cpsdata(0), image(0) {
    
    // Copy the scaler factor
    this->scaleq = scaleq;
    
    // Open the image file
    VFile* imgfile = VFSUtils::VFS_Open(fname);
    // If no file loaded throw an error
    if (imgfile == 0) {
        throw ImageNotFound("CPSImage: Image [" + string(fname) + "] not found.");
    }
    
    // Get the size
    imgsize = imgfile->fileSize();
    // Set the SDL surface to NULL
    image = 0;
    
    // Copy all data from the VFile
    cpsdata = new Uint8[imgsize];
    imgfile->readByte(cpsdata, imgsize);
    
    // Read data of the header
    lnkHeader.size = cpsdata[0] + (cpsdata[0+1] << 8);
    lnkHeader.unknown = cpsdata[2] + (cpsdata[2+1] << 8);
    lnkHeader.imsize = cpsdata[4] + (cpsdata[4+1] << 8);
    lnkHeader.palette = cpsdata[6] + (cpsdata[6+1] << 8) + (cpsdata[6+2] << 16)
            + (cpsdata[6+3] << 24);
    
    // If Palette is 03000000h then there's a palette after the header,
    // otherwise the image follows.
    if (lnkHeader.palette == 0x3000000) {
        readPalette();
    } else {
        // Offset = 10 because their are no palette and size of the header is 10
        offset = 10;
    }
    
    // Close the VFile
    VFSUtils::VFS_Close(imgfile);
}

/**
 */
CPSImage::~CPSImage() {
    delete[] cpsdata;
    
    if (image != 0) {
        SDL_FreeSurface(image);
    }
    image = 0;
}

/**
 */
SDL_Surface* CPSImage::getImage() {
    if (image == 0) {
        loadImage();
    }
    return image;
}

/**
 */
void CPSImage::loadImage() {
    Uint32 len;
    Uint8* imgsrc;
    Uint8 *imgdst;
    SDL_Surface* imgtmp;
    
    len = imgsize-offset;
    imgsrc = new Uint8[len];
    imgdst = new Uint8[lnkHeader.imsize];
    memcpy(imgsrc, cpsdata + offset, len);
    memset(imgdst, 0, lnkHeader.imsize);
    // Decode the image in format 80h
    Compression::decode80(imgsrc, imgdst);
    
    // 320x200x256 images. -> 320 , 200 , 8bit palette 
    imgtmp = SDL_CreateRGBSurfaceFrom(imgdst, 320, 200, 8, 320, 0, 0, 0, 0);
    SDL_SetColors(imgtmp, palette, 0, 256);
    SDL_SetColorKey(imgtmp, SDL_SRCCOLORKEY, 0);
    delete[] imgsrc;
    delete[] cpsdata;
    cpsdata = NULL;
    if (scaleq >= 0) {
        image = lnkScaler.scale(imgtmp, scaleq);
        SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0);
    }
    else {
        image = SDL_DisplayFormat(imgtmp);
    }
    SDL_FreeSurface(imgtmp);
    delete[] imgdst;
}

/**
 */
void CPSImage::readPalette() {
    Uint16 i;
    
    offset = 10;
    for (i = 0; i < 256; i++) {
        palette[i].r = cpsdata[offset];
        palette[i].g = cpsdata[offset+1];
        palette[i].b = cpsdata[offset+2];
        palette[i].r <<= 2;
        palette[i].g <<= 2;
        palette[i].b <<= 2;
#if 0
        //        palette[i].r = (palette[i].r*255)/63;
        //        palette[i].g = (palette[i].g*255)/63;
        //        palette[i].b = (palette[i].b*255)/63;
        printf("CPS ReadPalette r,g,b = %u,%u,%u\n", (unsigned char)palette[i].r, (unsigned char)palette[i].g, (unsigned char)palette[i].b);
#endif
        offset += 3;
    }
}
