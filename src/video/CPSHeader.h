// CPSHeader.h
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

#ifndef CPSHEADER_H
#define CPSHEADER_H

#include "SDL/SDL_types.h"

/**
 * Header of .CPS image
 *
 * The .CPS files contain 320x200x256 images. The images are compressed with the
 * Format80 compression method. They may or may not contain a palette.
 *
 * The header has the following structure :
 *  Header : record
 *    Size    : word;  {File size - 2}
 *    Unknown : word;  {Always 0004h}
 *    ImSize  : word;  {Size of uncompressed image (always 0FA00h)}
 *    Palette : longint; {Is there a palette ?}
 *  end;
 *
 * If Palette is 03000000h then there's a palette after the header, otherwise
 * the image follows.
 * CPS file without palette can be found in the SETUP.MIX file, and they all use
 * the Palette that can be found inside the same .MIX.
 *
 * The image that follows the palette (or the Header) is in Format80.
 *
 * @see CPSImage
 */
class CPSHeader {
public:
    /** Size of the file - 2 */
    Uint16 size;
    /** Unknow (Always = 0004h) */
    Uint16 unknown;
    /** Size of uncompressed image (always 0FA00h) */
    Uint16 imsize;
    /** Number of the palette */
    Uint32 palette;
};

#endif //CPSHEADER_H
