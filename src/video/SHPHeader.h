// SHPHeader.h
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

#ifndef SHPHEADER_H
#define SHPHEADER_H

#include "SDL/SDL_types.h"

/**
 * Header of SHP Image
 * 
 * @see SHPImage
 */
class SHPHeader
{
public:
	/** Number of images */
	Uint16  NumImages;
	/** Unknown */
	Uint16	A;
	/** Unknown */
	Uint16	B;
	/** Width of the images */
	Uint16  Width;
	/** Height of the images */
    Uint16  Height;
    /** Unknown */
    long int C;
    /** Offset of image in file */
    Uint32* Offset;
    /** Format of image in file */
    Uint8*  Format;
    /** Offset and format of image on which it is based */
    Uint32* RefOffs;
    Uint8*  RefFormat;
};

#endif //SHPHEADER_H
