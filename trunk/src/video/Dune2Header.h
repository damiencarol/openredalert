// Dune2Header.h
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

#ifndef DUNE2HEADER_H
#define DUNE2HEADER_H

#include "SDL/SDL_types.h"

class Dune2Header {
public:
    Uint16 compression;
    Uint8  cy;
    Uint16 cx;
    Uint8  cy2;
    Uint16 size_in;
    Uint16 size_out;
};

#endif //DUNE2HEADER_H
