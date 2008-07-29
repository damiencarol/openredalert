// animinfo_t.h
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

#ifndef ANIMINFO_T_H
#define ANIMINFO_T_H

#include "SDL/SDL_types.h"

/**
 * Information about image animation (use to decode image in animation) 
 */
struct animinfo_t
{
	Uint32 animdelay;
	Uint8 loopend;
	Uint8 loopend2;
	Uint8 animspeed;
	Uint8 animtype;
	Uint8 sectype;
	Uint8 dmgoff;
	Uint8 dmgoff2;
	/** number of image during making anim */
	Uint16 makenum;
};

#endif //ANIMINFO_T_H
