// TemplateImage.h
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

#ifndef TEMPLATEIMAGE_H
#define TEMPLATEIMAGE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "SHPBase.h"

class VFile;

/**
 * Image use to decode tile of terrain overlay pack 
 */
class TemplateImage : SHPBase
{
public:
	TemplateImage(const char *fname, Sint8 scaleq, bool ratemp = false);
	~TemplateImage();

	Uint16 getNumTiles();
	SDL_Surface* getImage(Uint16 imgnum);

private:
	bool ratemp;
	VFile* tmpfile;
};

#endif //TEMPLATEIMAGE_H
