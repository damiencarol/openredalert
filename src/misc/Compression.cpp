#include "Compression.h"

#include <algorithm>
#include <cstring>
#include <cctype>

#include "SDL/SDL_types.h"
#include "SDL/SDL_endian.h"

#include "include/Logger.h"


extern Logger * logger;

/** 
 * Decodes base64 data
 * 
 *  @param compressed data
 *  @param pointer to put uncompressed data
 *  @param size of compressed data
 *  @returns -1 if error
 */
int Compression::dec_base64(const unsigned char* src, unsigned char* target, size_t length)
{
    int i;
    unsigned char a, b, c, d;
    static unsigned char dtable[256];
    int bits_to_skip = 0;

    for( i = length-1; src[i] == '='; i-- ) {
        bits_to_skip += 2;
        length--;
    }
    if( bits_to_skip >= 6 ) {
        logger->warning("Error in base64 (too many '=')\n");
        return -1;
    }

    for(i= 0;i<255;i++) {
        dtable[i]= 0x80;
    }
    for(i= 'A';i<='Z';i++) {
        dtable[i]= i-'A';
    }
    for(i= 'a';i<='z';i++) {
        dtable[i]= 26+(i-'a');
    }
    for(i= '0';i<='9';i++) {
        dtable[i]= 52+(i-'0');
    }
    dtable[(Uint8)'+']= 62;
    dtable[(Uint8)'/']= 63;
    dtable[(Uint8)'=']= 0;


    while (length >= 4) {
        a = dtable[src[0]];
        b = dtable[src[1]];
        c = dtable[src[2]];
        d = dtable[src[3]];
        if( a == 0x80 || b == 0x80 ||
                c == 0x80 || d == 0x80 ) {
            logger->warning("Illegal character\n");
        }
        target[0] = a << 2 | b >> 4;
        target[1] = b << 4 | c >> 2;
        target[2] = c << 6 | d;
        target+=3;
        length-=4;
        src += 4;
    }
    if( length > 0 ) {
        if( bits_to_skip == 4 && length == 2 ) {
            a = dtable[src[0]];
            b = dtable[src[1]];

            target[0] = a << 2 | b >> 4;
        } else if( bits_to_skip == 2 && length == 3 ) {
            a = dtable[src[0]];
            b = dtable[src[1]];
            c = dtable[src[2]];

            target[0] = a << 2 | b >> 4;
            target[1] = b << 4 | c >> 2;
        } else {
            logger->warning("Error in base64. #bits to skip doesn't match length\n");
            logger->warning("skip %d bits, %d chars left\n\"%s\"\n", bits_to_skip, (Uint32)length, src);
            return -1;
        }
    }

    return 0;
}
/** 
 * Decompress format 80 compressed data.
 * 
 * @param compressed data.
 * @param pointer to output uncompressed data.
 * @returns size of uncompressed data.
 */
int Compression::decode80(const Uint8 image_in[], Uint8 image_out[])
{
    /*
    0 copy 0cccpppp p
    1 copy 10cccccc
    2 copy 11cccccc p p
    3 fill 11111110 c c v
    4 copy 11111111 c c p p
    */

    const Uint8* copyp;
    const Uint8* readp = image_in;
    Uint8* writep = image_out;
    Uint32 code;
    Uint32 count;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint16 bigend; /* temporary big endian var */
#endif

    while (1) {
        code = *readp++;
        if (~code & 0x80) {
            //bit 7 = 0
            //command 0 (0cccpppp p): copy
            count = (code >> 4) + 3;
            copyp = writep - (((code & 0xf) << 8) + *readp++);
            while (count--)
                *writep++ = *copyp++;
        } else {
            //bit 7 = 1
            count = code & 0x3f;
            if (~code & 0x40) {
                //bit 6 = 0
                if (!count)
                    //end of image
                    break;
                //command 1 (10cccccc): copy
                while (count--)
                    *writep++ = *readp++;
            } else {
                //bit 6 = 1
                if (count < 0x3e) {
                    //command 2 (11cccccc p p): copy
                    count += 3;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

                    memcpy(&bigend, readp, 2);
                    copyp = &image_out[SDL_Swap16(bigend)];
#else

                    copyp = &image_out[*(Uint16*)readp];
#endif

                    readp += 2;
                    while (count--)
                        *writep++ = *copyp++;
                } else if (count == 0x3e) {
                    //command 3 (11111110 c c v): fill
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    memset(&count, 0, sizeof(Uint32));
                    memcpy(&count, readp, 2);
                    count = SDL_Swap32(count);
#else

                    count = *(Uint16*)readp;
#endif

                    readp += 2;
                    code = *readp++;
                    while (count--)
                        *writep++ = code;
                } else {
                    //command 4 (copy 11111111 c c p p): copy
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    memset(&count, 0, sizeof(Uint32));
                    memcpy(&count, readp, 2);
                    count = SDL_Swap32(count);
#else

                    count = *(Uint16*)readp;
#endif

                    readp += 2;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

                    memcpy(&bigend, readp, 2);
                    copyp = &image_out[SDL_Swap16(bigend)];
#else

                    copyp = &image_out[*(Uint16*)readp];
#endif

                    readp += 2;
                    while (count--)
                        *writep++ = *copyp++;
                }
            }
        }
    }

    return (writep - image_out);
}
/** 
 * Decompress format 40 compressed data.
 * 
 * @param compressed data.
 * @param pointer to pu uncompressed data in.
 * @returns size of uncompressed data.
 */
int Compression::decode40(const Uint8 image_in[], Uint8 image_out[])
{
    /*
    0 fill 00000000 c v
    1 copy 0ccccccc
    2 skip 10000000 c 0ccccccc
    3 copy 10000000 c 10cccccc
    4 fill 10000000 c 11cccccc v
    5 skip 1ccccccc
    */

    const Uint8* readp = image_in;
    Uint8* writep = image_out;
    Uint32 code;
    Uint32 count;

    while (1) {
        code = *readp++;
        if (~code & 0x80) {
            //bit 7 = 0
            if (!code) {
                //command 0 (00000000 c v): fill
                count = *readp++;
                code = *readp++;
                while (count--)
                    *writep++ ^= code;
            } else {
                //command 1 (0ccccccc): copy
                count = code;
                while (count--)
                    *writep++ ^= *readp++;
            }

        } else {
            //bit 7 = 1
            if (!(count = code & 0x7f)) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                memset(&count, 0, sizeof(Uint32));
                memcpy(&count, readp, 2);
                count = SDL_Swap32(count);
#else

                count = *(Uint16*)readp;
#endif

                readp += 2;
                code = count >> 8;
                if (~code & 0x80) {
                    //bit 7 = 0
                    //command 2 (10000000 c 0ccccccc): skip
                    if (!count)
                        // end of image
                        break;
                    writep += count;
                } else {
                    //bit 7 = 1
                    count &= 0x3fff;
                    if (~code & 0x40) {
                        //bit 6 = 0
                        //command 3 (10000000 c 10cccccc): copy
                        while (count--)
                            *writep++ ^= *readp++;
                    } else {
                        //bit 6 = 1
                        //command 4 (10000000 c 11cccccc v): fill
                        code = *readp++;
                        while (count--)
                            *writep++ ^= code;
                    }
                }
            } else {
                //command 5 (1ccccccc): skip
                writep += count;
            }
        }
    }
    return (writep - image_out);
}
/** 
 * Decompress format 20 compressed data.
 * 
 * @param compressed data.
 * @param pointer to pu uncompressed data in.
 * @param size of compressed data?
 * @returns size of uncompressed data?
 */
int Compression::decode20(const Uint8* s, Uint8* d, int cb_s)
{
    const Uint8* r = s;
    const Uint8* r_end = s + cb_s;
    Uint8* w = d;
    while (r < r_end) {
        int v = *r++;
        if (v)
            *w++ = v;
        else {
            v = *r++;
            memset(w, 0, v);
            w += v;
        }
    }
    return w - d;

}
