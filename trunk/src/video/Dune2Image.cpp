// Dune2Image.cpp
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

#include "Dune2Image.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "include/imageproc.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "video/ImageNotFound.h"

using std::string;
using std::runtime_error;

extern Logger * logger;

/**
 * Constructor loads a dune2 shpfile.
 *
 * @param fname the name of the dune2 shpfile.
 * @param scaleq do scalling or not
 */
Dune2Image::Dune2Image(const char *fname, Sint8 scaleq) : SHPBase(fname, scaleq)
{
    VFile* imgfile = 0;
    
    // Open the file in .mix archivefile
    imgfile = VFSUtils::VFS_Open(fname);
    if (imgfile == 0) 
    {
        logger->error(" File \"%s\" not found.\n", fname);
        shpdata = 0;
        throw ImageNotFound("File \"" + string(fname) + "\" not found.");
    }
    
    // Create the buffer with correct size
    shpdata = new Uint8[imgfile->fileSize()];
    
    // Fill the buffer
    imgfile->readByte(shpdata, imgfile->fileSize());
        
    // Close the archive
    VFSUtils::VFS_Close(imgfile);
}

/** 
 * Destructor, frees up the memory used by a dune2 shp.
 */
Dune2Image::~Dune2Image()
{	
    delete[] shpdata;
}

/** 
 * Decode a image in the dune2 shp.
 * 
 * @param imgnum the number of the image to decode.
 * @return a SDL_Surface containing the image.
 */
SDL_Surface* Dune2Image::getImage(Uint16 imgnum)
{
    SDL_Surface* image = 0;
    SDL_Surface* optimage = 0;
    Uint32 startpos;
    Uint8* d = 0;
    Uint8* data = 0;

    startpos = getD2Header(imgnum);

    data = new Uint8[lnkHeader.cx * lnkHeader.cy];

    if( ~lnkHeader.compression & 2 ) 
    {
        d = new Uint8[lnkHeader.size_out];

        memset(d, 0, lnkHeader.size_out);

        Compression::decode20( d, data, Compression::decode80( shpdata+startpos, d ) );

        delete[] d;
    } else {
        Compression::decode20( shpdata+startpos, data, lnkHeader.size_out );
    }
    
    image = SDL_CreateRGBSurfaceFrom(data, lnkHeader.cx, lnkHeader.cy,
                                     8, lnkHeader.cx, 0, 0, 0, 0);

    /// @bug TEMPORARY HACK
    // The index 0x0c is used to give some cursors shadows, this is defined in
    // the palette as (0,0,0), which is also the colour of the index 0, which
    // has to be set transparent.  Not sure why it kills 0x0c as well as 0x0.
    // 0x0c does not get killed in 8 bit mode.

#if 0
    for (Sint32 pos = 0; pos < header.cx*header.cy; ++pos) {
        if (data[pos] == 0x0c)
            data[pos] = 0x9a;
    }
#endif

    SDL_SetColors(image, palette[0], 0, 256);
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0);

    if (scaleq >= 0) {
        optimage = scale(image, scaleq);
        SDL_SetColorKey(optimage, SDL_SRCCOLORKEY, 0);
    } else {
        optimage = SDL_DisplayFormat(image);
    }
    SDL_FreeSurface(image);
    delete[] data;

    return optimage;
}

/** 
 * Read the header of a specified dune2 shp.
 * 
 * @param imgnum the number of the image to read the header from.
 * @return the offset of the image.
 */
Uint32 Dune2Image::getD2Header(Uint16 imgnum)
{
    Uint16 imgs;
    Uint32 curpos;

    imgs = shpdata[0] + (shpdata[0+1] << 8);

    if (imgnum >= imgs) {
        logger->error("%s: getD2Header called with invalid param: %i (>= %i)\n",
                name.c_str(), imgnum, imgs);
        return 0;
    }

    if( shpdata[4] + (shpdata[4+1] << 8) ) {
        curpos = shpdata[imgnum*2 + 2] + (shpdata[imgnum*2 + 2+1] << 8);
    } else {
        curpos = shpdata[imgnum*4 + 2] + (shpdata[imgnum*4 + 2+1] << 8) + (shpdata[imgnum*4 + 2+2] << 16) + (shpdata[imgnum*4 + 2+3] << 24) + 2;
    }

    lnkHeader.compression = shpdata[curpos] + (shpdata[curpos+1] << 8);
    curpos+=2;
    lnkHeader.cy = shpdata[curpos];
    curpos++;
    lnkHeader.cx = shpdata[curpos] + (shpdata[curpos+1] << 8);
    curpos += 2;
    lnkHeader.cy2 = shpdata[curpos];
    curpos++;
    lnkHeader.size_in = shpdata[curpos] + (shpdata[curpos+1] << 8);
    curpos += 2;
    lnkHeader.size_out = shpdata[curpos] + (shpdata[curpos+1] << 8);
    curpos += 2;

    if( lnkHeader.compression & 1 ){
        curpos += 16;
    }

    return curpos;
}
