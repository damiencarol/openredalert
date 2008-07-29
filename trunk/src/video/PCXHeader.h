// PCXHeader.h
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

#ifndef PCXHEADER_H
#define PCXHEADER_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

/**
 * Header of a .pcx file
 */
class PCXHeader
{
public:
    Uint8	Signature;
    Uint8	Version;
    Uint8	Encoding;
    Uint8	BitsPixel;
    Uint16	XMin;
    Uint16	YMin;
    Uint16	XMax;
    Uint16	YMax;
    Uint16	HRes;
    Uint16	VRes;
    SDL_Color palette[16];
    Uint8	Reserved;
    Uint8	NumPlanes;
    Uint16	BytesLine;
    /** 1 = Color or s/w, 2 = Black and white */
    Uint16	PalType;
    Uint16	HscreenSize;
    Uint16	VscreenSize;
    Uint8	Dummy[58];
};

#endif //PCXHEADER_H
