#ifndef SOUNDUTILS_H
#define SOUNDUTILS_H

#include "SDL/SDL_types.h"

class SoundUtils {
public:

    static Uint8 Clip(int parameter0);

    static Uint8 Clip(int, int, int);

    static void IMADecode(Uint8 *output, Uint8 *input, Uint16 compressed_size, Sint32& sample, Sint32& index);

    static void WSADPCM_Decode(Uint8 *output, Uint8 *input, Uint16 compressed_size, Uint16 uncompressed_size);
};
#endif //SOUNDUTILS_H
