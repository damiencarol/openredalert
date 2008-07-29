// WSAHeader.h
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

#ifndef WSAHEADER_H
#define WSAHEADER_H

#include "SDL/SDL_types.h"

/**
 * Header of the WSA Movie
 * 
 * WSA files contain short animations and can be found in the GENERAL.MIX files.
 * They are basically a series of Format40 images, that are then compressed with
 * Format80.
 * 
 * The header is :
 *  Header : record
 *             NumFrames : word;  {Number of frames}
 *             X,Y       : word;  {Position on screen of the upper left corner}
 *             W,H       : word;  {Width and height of the images}
 *             Delta     : longint; {Frames/Sec = Delta/(2^10)}
 *           end;
 * 
 * Following that there's an array of offsets :
 * 
 *   Offsets : array [0..NumFrames+1] of longint;
 * 
 * The obtain the actual offset, you have to add 300h. 
 * That is the size of the palette that follows the Offsets array.
 * As for .SHP files the two last offsets have a special meaning.
 * If the last offset is 0 then the one before it points to the end of file
 * (after you added 300h of course).
 * If the last one is <>0 then it points to the end of the file, and the
 * one before it points to a special frame that gives you the difference
 * between the last and the first frame. This is used when you have to loop
 * the animation.
 * 
 * @see WSAMovie
 */
class WSAHeader
{
public:
	/** Number of frames in the movie */
	Uint16 NumFrames;
	/** Position on screen of the upper left corner (absciss X) */
	Uint16 xpos;
	/** Position on screen of the upper left corner (absciss Y) */
	Uint16 ypos;
	/** Width of frames in the movie */
	Uint16 width;
	/** Height of frames in the movie */
	Uint16 height;
	/** Frames/Sec = Delta/(2^10) */
	Uint32 delta;
	/** Offsets of images in the file */
	Uint32* offsets;
};

#endif //WSAHEADER_H
