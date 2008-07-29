// SoundUtils.h
// 0.4

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

#ifndef SOUNDUTILS_H
#define SOUNDUTILS_H

#include "SDL/SDL_types.h"

/**
 * 
 */
class SoundUtils {
public:

    static Uint8 Clip(int parameter0);

    static Uint8 Clip(int, int, int);

    static void IMADecode(Uint8 *output, Uint8 *input, Uint16 compressed_size, Sint32& sample, Sint32& index);

    static void WSADPCM_Decode(Uint8 *output, Uint8 *input, Uint16 compressed_size, Uint16 uncompressed_size);
};

#endif //SOUNDUTILS_H
