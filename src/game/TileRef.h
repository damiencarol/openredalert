// TileRef.h
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

#ifndef TILEREF_H
#define TILEREF_H

#include "SDL/SDL_types.h"

/**
 * Reference to a tile
 */
class TileRef 
{
public:    
    Uint16 crx;
    Uint16 cry;
    Uint32 g;
    Uint32 h;
};

#endif //TILEREF_H
