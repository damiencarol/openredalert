// CursorInfo.h
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

#ifndef CURSORINFO_H
#define CURSORINFO_H

#include "SDL/SDL_types.h"

/**
 * All Data of a cursor 
 */
class CursorInfo
{
public:
    Uint16 getAnStart();
    void setAnStart(Uint16 anStart);
    Uint16 getAnEnd();
    void setAnEnd(Uint16 anEnd);

private:
    Uint16 anStart;
    Uint16 anEnd;
};

#endif //CURSORINFO_H
