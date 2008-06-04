// CPSImage.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
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
 */
class CPSImage
{
public:
	CPSImage(const char* fname, int scaleq);
	~CPSImage();
	
	/** Get a SDL_Surface of the image */
	SDL_Surface* getImage();

private:
	void loadImage();
	/** Read the palette in data */
	void readPalette();

	Uint32 imgsize;
	Uint32 offset;
	Uint8* cpsdata;
	/** Internal palette of the image */
	SDL_Color palette[256];

	/**
	 * @link aggregationByValue 
	 */
	CPSHeader lnkHeader;
	int scaleq;
	ImageProc lnkScaler;
	SDL_Surface* image;
};

#endif //CPSIMAGE_H
