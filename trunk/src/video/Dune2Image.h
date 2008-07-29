// Dune2Image.h
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

#ifndef DUNE2IMAGE_H
#define DUNE2IMAGE_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "Dune2Header.h"
#include "SHPBase.h"

class Dune2Image : SHPBase
{
public:
    Dune2Image(const char *fname, Sint8 scaleq);
    ~Dune2Image();

    SDL_Surface* getImage(Uint16 imgnum);

private:
    Uint32 getD2Header(Uint16 imgnum);
    Uint8* shpdata;

    /** @link aggregationByValue*/
    Dune2Header lnkHeader;
};

#endif
