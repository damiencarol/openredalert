// FontCache.h
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

#ifndef FONTCACHE_H
#define FONTCACHE_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "TFontImage.h"

using std::string;
using std::vector;

class FontCache {
public:
    FontCache(void);
    ~FontCache(void);
    void Add(TFontImage Image);
    void Add(std::string fontname, SDL_Surface * img, std::vector < SDL_Rect > chrdest);
    bool Get(std::string fontname, SDL_Surface * * fontimg, std::vector < SDL_Rect > & chrdest);

private:
    std::vector < TFontImage > font_cache;
};

#endif //FONTCACHE_H
