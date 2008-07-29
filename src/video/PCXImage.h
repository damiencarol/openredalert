// PCXImage.h
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

#ifndef PCXIMAGE_H
#define PCXIMAGE_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "PCXHeader.h"
#include "include/imageproc.h"

class VFile;

/**
 * Image in .pcx format
 */
class PCXImage
{
public:
	PCXImage(const char * fname, int scaleq);
	~PCXImage();

	SDL_Surface* getImage();

private:
	void loadImage();
	void readPalette();

	/** Header of the file */
	PCXHeader header;
	Uint32 imgsize;
	Uint8 * pcxdata;
	SDL_Color palette[256];
	int scaleq;
	ImageProc scaler;
	SDL_Surface * image;
	bool Use16ColorPalette;
	bool HeaderError;
};

#endif //PCXIMAGE_H
