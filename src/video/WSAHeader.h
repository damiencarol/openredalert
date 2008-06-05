// WSAHeader.h
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

#ifndef WSAHEADER_H
#define WSAHEADER_H

#include "SDL/SDL_types.h"

/**
 * Header of the WSA Movie
 * 
 * @see WSAMovie
 */
class WSAHeader
{
public:
	/** Number of frames in the movie */
	Uint16 NumFrames;
	/** X position ??? of frames in the movie */
	Uint16 xpos;
	/** Y position ??? of frames in the movie */
	Uint16 ypos;
	/** Width of frames in the movie */
	Uint16 width;
	/** Height of frames in the movie */
	Uint16 height;
	/** ??? */
	Uint32 delta;
	/** ??? */
	Uint32* offsets;
};

#endif //WSAHEADER_H
