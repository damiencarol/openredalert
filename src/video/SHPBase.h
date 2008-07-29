// SHPBase.h
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

#ifndef SHPBASE_H
#define SHPBASE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class VFile;
class ImageProc;

using std::string;

/**
 * Base class for all image based on shp file in archives
 */
class SHPBase 
{
public:
    SHPBase(const string& fname, Sint8 scaleq = -1);
    virtual ~SHPBase();

    static void setPalette(SDL_Color *pal);
    static void calculatePalettes();
    static SDL_Color* getPalette(Uint8 palnum) ;
    static Uint32 getColour(SDL_PixelFormat* fmt, Uint8 palnum, Uint16 index);
    static Uint8 numPalettes() ;

    SDL_Surface* scale(SDL_Surface *input, int quality);
    const string& getFileName() const ;

protected:
    static SDL_Color palette[32][256];
    static const Uint8 numpals;
    string name;
    Sint8 scaleq;
    ImageProc* scaler;
};

#endif //SHPBASE_H
