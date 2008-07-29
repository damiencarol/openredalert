// SHPImage.h
// 1.1

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

#ifndef SHPIMAGE_H
#define SHPIMAGE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "SHPBase.h"

class SHPHeader;

using std::string;

/**
 * shpimage - code to load/decode shp files
 */
class SHPImage : SHPBase
{
public:
	SHPImage(const char * fname, Sint8 scaleq);
	~SHPImage();

	void getImage(Uint16 imgnum, SDL_Surface * * img, SDL_Surface * * shadow,
			Uint8 palnum);
	void getImageAsAlpha(Uint16 imgnum, SDL_Surface * * img);

	Uint32 getWidth() const;
	Uint32 getHeight() const;
	Uint16 getNumImg() const;
	string getFileName() const;

private:
	static SDL_Color shadowpal[2];
	static SDL_Color alphapal[6];

	void DecodeSprite(Uint8 * imgdst, Uint16 imgnum);
	Uint8* shpdata;
	SHPHeader* lnkHeader;
};

#endif //SHPIMAGE_H
