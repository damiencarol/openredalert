// SHPImage.cpp
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

#include "SHPImage.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

#include "SDL/SDL_video.h"

#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "include/imageproc.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "include/config.h"
#include "headerformats.h"
#include "ImageNotFound.h"
#include "SHPHeader.h"

using std::runtime_error;
using std::string;

namespace pc {
    extern ConfigType Config;
}
extern Logger * logger;


/**
 * Constructor, loads a shpfile.
 *
 * @param fname the filename
 * @param scaleq scaling option (-1 is disabled)
 */
SHPImage::SHPImage(const char *fname, Sint8 scaleq) : SHPBase(fname, scaleq)
{
    int i; // variable use for loop
    int j; // variable use for loop
    VFile *imgfile; // link to the file in mix archives

    // Create the header
    lnkHeader = new SHPHeader();

    // Set to 0 before reading of the header
    lnkHeader->NumImages = 0;

	// Open the file in archive
    imgfile = VFSUtils::VFS_Open(fname);
    // Check that file is loaded
    if (imgfile == NULL) {
    	// Log it
    	logger->error("failed to load %s (SHPImage)\n", fname);
    	// Throw an Exception
        throw ImageNotFound("failed to load (SHPImage) " + string(fname));
    }

    // Allocate data for the data
    shpdata = new Uint8[imgfile->fileSize()];
    // Read all the file and store it in 'shpdata'
    imgfile->readByte(shpdata, imgfile->fileSize());

    // lnkHeader
    lnkHeader->NumImages = shpdata[0] + (shpdata[0+1] << 8);
    lnkHeader->Width = shpdata[6] + (shpdata[6+1] << 8);

    // Read unknow variables
    lnkHeader->A = shpdata[2] + (shpdata[2+1] << 8);
    lnkHeader->B = shpdata[4] + (shpdata[4+1] << 8);

    lnkHeader->Height = shpdata[8] + (shpdata[8+1] << 8);
    lnkHeader->Offset = new Uint32[lnkHeader->NumImages + 2];
    lnkHeader->Format = new Uint8[lnkHeader->NumImages + 2];
    lnkHeader->RefOffs = new Uint32[lnkHeader->NumImages + 2];
    lnkHeader->RefFormat = new Uint8[lnkHeader->NumImages + 2];

    // "Offsets"
    j = 14;
    for (i = 0; i < lnkHeader->NumImages + 2; i++)
    {
        lnkHeader->Offset[i] = shpdata[j] + (shpdata[j+1] << 8) + (shpdata[j+2] << 16) + (0 << 24);
        j += 3;
        lnkHeader->Format[i] = shpdata[j];
        j += 1;
        lnkHeader->RefOffs[i] = shpdata[j] + (shpdata[j+1] << 8) + (shpdata[j+2] << 16) + (0 << 24);
        j += 3;
        lnkHeader->RefFormat[i] = shpdata[j];
        j += 1;
    }

    // Close the file
    VFSUtils::VFS_Close(imgfile);
}

/**
 * Destructor, freas the memory used by the shpimage.
 */
SHPImage::~SHPImage()
{
	// Free data from the file
    delete[] shpdata;
    // free headers vars
    delete[] lnkHeader->Offset;
    delete[] lnkHeader->Format;
    delete[] lnkHeader->RefOffs;
    delete[] lnkHeader->RefFormat;

    // free the header
    delete lnkHeader;
}

/**
 * Extract a frame from a SHP into two SDL_Surface* (shadow is separate)
 *
 * @param imgnum the index of the frame to decode.
 * @param img pointer to the SDL_Surface* into which the frame is decoded.
 * @param shadow pointer to the SDL_Surface* into which the shadow frame is decoded. This can be 0 if you don't need the shadow.
 * @param palnum Number of the palette to use
 */
void SHPImage::getImage(Uint16 imgnum, SDL_Surface **img, SDL_Surface **shadow, Uint8 palnum)
{
	// If the image is NULL
	if (0 == img)
	{
		string s = name + ": can't decode to a NULL surface";
		throw runtime_error(s);
	}

	// The palette index 0 is the transparent pixel. In the palette this pixel originally is black
	// However using black as a colorkey doesn't work as black is also used in other parts of some pictures
	// So we set index 0 to a (hopefully) unused color and set the colorkey to that :)
	palette[palnum][0].r = 22;
	palette[palnum][0].g = 255;
	palette[palnum][0].b = 22;

	if (imgnum >= lnkHeader->NumImages)
	{
		logger->error("%s line %i: Error want imgnum %i but only %i images availeble, image name = %s\n", __FILE__, __LINE__, imgnum, lnkHeader->NumImages, name.c_str());
		*img = 0;
		*shadow = 0;
		return;
	}

	Uint8* imgdata = new Uint8[lnkHeader->Width * lnkHeader->Height];
	DecodeSprite(imgdata, imgnum);

	if (shadow != 0)
	{
		Uint8* shadowdata = new Uint8[lnkHeader->Width * lnkHeader->Height];
		memset(shadowdata, 0, lnkHeader->Width * lnkHeader->Height);
		for (int i = 0; i<lnkHeader->Width * lnkHeader->Height; ++i)
		{
			if (imgdata[i] == 4)
			{
				imgdata[i] = 0;
				shadowdata[i] = 1;
			}
		}
		SDL_Surface* shadowimg = SDL_CreateRGBSurfaceFrom(shadowdata,
			lnkHeader->Width, lnkHeader->Height, 8, lnkHeader->Width, 0, 0, 0, 0);
		SDL_SetColors(shadowimg, shadowpal, 0, 2);
		SDL_SetColorKey(shadowimg, SDL_SRCCOLORKEY, 0);
		SDL_SetAlpha(shadowimg, SDL_SRCALPHA|SDL_RLEACCEL, 128);

		if (scaleq >= 0) {
		*shadow = scale(shadowimg, scaleq);
		SDL_SetColorKey(*shadow, SDL_SRCCOLORKEY, 0);
		} else {
		*shadow = SDL_DisplayFormat(shadowimg);
		}
		SDL_FreeSurface(shadowimg);
		delete[] shadowdata;
	} else {
		for (int i = 0; i<lnkHeader->Width * lnkHeader->Height; ++i)
		{
			if (imgdata[i] == 4)
			{
				imgdata[i] = 0;
			}
		}
	}
	/*
	// DEBUGGING
	for (int i = 0; i<lnkHeader.Width * lnkHeader.Height; ++i) {
		if (name == (char*)"dd-edge.shp")
			printf ("Palette index = %i\n", imgdata[i]);
	}
	*/

	SDL_Surface* imageimg = SDL_CreateRGBSurfaceFrom(imgdata, lnkHeader->Width, lnkHeader->Height, 8, lnkHeader->Width, 0, 0, 0, 0);
	SDL_SetColors(imageimg, palette[palnum], 0, 256);

//	SDL_SetColorKey(imageimg, SDL_SRCCOLORKEY, 0);
//	SDL_SetColorKey(imageimg, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(imageimg->format, 1, 255, 1 ));

	if (scaleq >= 0) {
		*img = scale(imageimg, scaleq);
		SDL_SetColorKey(*img, SDL_SRCCOLORKEY, 0);
	} else {
		*img = SDL_DisplayFormat(imageimg);
	}

	// Setup the colorkey
	SDL_Surface *imgp = *img;
	//Uint32 KeyColor = SDL_MapRGB(imgp->format, 1, 10, 1 );
	SDL_SetColorKey(*img, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(imgp->format, 22, 255, 22 ));
	// Free
	SDL_FreeSurface(imageimg);

	delete[] imgdata;
}

/**
 * Extracts a SHP into a SDL_Surface* with the values mapped to different
 * levels of transparency.
 *
 * Might be a bit of a hack, since the only valid palette values allowed
 * are 0, 12-16. However only shadows.shp seems to use this function, thus
 * its ok.
 *
 * @param imgnum Image number to get
 * @param img Destination image
 */
void SHPImage::getImageAsAlpha(Uint16 imgnum, SDL_Surface **img)
{
    Uint8* imgdata = new Uint8[lnkHeader->Width * lnkHeader->Height];

    DecodeSprite(imgdata, imgnum);

    for (Uint16 i = 0; i < lnkHeader->Width * lnkHeader->Height; ++i)  {
        // The shadows.shp only uses 0, 12-16
        // So we map them to 0-5
        if (imgdata[i] > 11) {
            imgdata[i] = 17 - imgdata[i];
        }
    }

    SDL_Surface* imageimg = SDL_CreateRGBSurfaceFrom(imgdata, lnkHeader->Width,
        lnkHeader->Height, 8, lnkHeader->Width, 0, 0, 0, 0);
    SDL_SetColors(imageimg, alphapal, 0, 7);

    SDL_PixelFormat fmt = {NULL, 32, 4, 0, 0, 0, 0, 8, 16, 24, 32, 0x000000ff,
        0x0000ff00, 0x00ff0000, 0xff000000, 0, 0};

    SDL_Surface* alphaimg = SDL_ConvertSurface(imageimg, &fmt, SDL_SWSURFACE);
    SDL_LockSurface(alphaimg);

    // Use the Red value as the alpha value for each pixel
    Uint32 *p = (Uint32 *)alphaimg->pixels;
    for (Uint16 i = 0; i < lnkHeader->Width * lnkHeader->Height; ++i) {
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        *p = SDL_Swap32(*p);
        #endif
        *p = *p<<fmt.Rshift;
        *p &= fmt.Amask;
        ++p;
    }

    SDL_UnlockSurface(alphaimg);

    if (scaleq >= 0) {
        *img = scale(imageimg, scaleq);
        SDL_SetColorKey(*img, SDL_SRCCOLORKEY, 0);
    } else {
        *img = SDL_DisplayFormatAlpha(alphaimg);
    }

    SDL_FreeSurface(imageimg);
    SDL_FreeSurface(alphaimg);
    delete[] imgdata;
}

/**
 * Get the Width of the image
 * @return Width of the image
 */
Uint32 SHPImage::getWidth() const
{
	return lnkHeader->Width;
}

/**
 * Get the Height of the image
 * @return Height of the image
 */
Uint32 SHPImage::getHeight() const
{
	return lnkHeader->Height;
}

/**
 * Get number of image in the SHPImage file
 * @return Number of image in the SHPImage file
 */
Uint16 SHPImage::getNumImg() const
{
	return lnkHeader->NumImages;
}

/**
 * Get the name of the file
 * @return Name of the file
 */
string SHPImage::getFileName() const
{
	return name;
}

SDL_Color SHPImage::shadowpal[2] =
{
	{0xff,0xff,0xff,0},
	{0x00,0x00,0x00,0}
};

SDL_Color SHPImage::alphapal[6] =
{
	{0x00,0x00,0x00,0x00},
	{0x33,0x33,0x33,0x33},
	{0x66,0x66,0x66,0x66},
	{0x99,0x99,0x99,0x99},
	{0xCC,0xCC,0xCC,0xCC},
	{0xFF,0xFF,0xFF,0xFF}
};

/**
 * Method to decompress a format xx compressed image.
 *
 * @param imgdst The buffer in which to put the image (must contain XOR image).
 * @param imgnum The index of the frame to decompress.
 */
void SHPImage::DecodeSprite(Uint8 *imgdst, Uint16 imgnum)
{
	// Check if imgnum to decompress is <= images in SHP
    if (imgnum >= lnkHeader->NumImages)
    {
        logger->error("%s: Invalid SHP imagenumber (%i >= %i)\n", name.c_str(), imgnum, lnkHeader->NumImages);
        return;
    }

    Uint32 len;
    Uint8* imgsrc;
    switch (lnkHeader->Format[imgnum]) {
        case FORMAT_80:
            len = lnkHeader->Offset[imgnum + 1] - lnkHeader->Offset[imgnum];
            imgsrc = new Uint8[len];
            memcpy(imgsrc, shpdata + lnkHeader->Offset[imgnum], len);
            memset(imgdst, 0, sizeof(imgdst));
            Compression::decode80(imgsrc, imgdst);
            break;
        case FORMAT_40:{
            Uint32 i;
            for (i = 0; i < lnkHeader->NumImages; i++ ) {
                if (lnkHeader->Offset[i] == lnkHeader->RefOffs[imgnum])
                    break;
            }
            DecodeSprite(imgdst, i);
            len = lnkHeader->Offset[imgnum + 1] - lnkHeader->Offset[imgnum];
            imgsrc = new Uint8[len];
            memcpy(imgsrc, shpdata + lnkHeader->Offset[imgnum], len);
            Compression::decode40(imgsrc, imgdst);
            break;
        }
        case FORMAT_20:
            DecodeSprite(imgdst, imgnum - 1);
            len = lnkHeader->Offset[imgnum + 1] - lnkHeader->Offset[imgnum];
            imgsrc = new Uint8[len];
            memcpy(imgsrc, shpdata + lnkHeader->Offset[imgnum], len);
            Compression::decode40(imgsrc, imgdst);
            break;
        default:
            logger->error("Possible memory corruption detected: unknown lnkHeader format in %s at frame %i/%i.\n", name.c_str(), imgnum, lnkHeader->NumImages);
            return;
    }
    delete[] imgsrc;
}



