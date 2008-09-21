// Compression.cpp
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

#include "Compression.hpp"

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
 *  @param src compressed data
 *  @param target pointer to put uncompressed data
 *  @param length size of compressed data
 *  @return -1 if error
 */
int Compression::dec_base64(const unsigned char* src, unsigned char* target, const unsigned int length)
{
    int i;
    unsigned char a, b, c, d;
    static unsigned char dtable[256];
    int bits_to_skip = 0;
    unsigned int varLength = length;

    for( i = varLength-1; src[i] == '='; i-- ) {
        bits_to_skip += 2;
        varLength--;
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
    dtable[(unsigned char)'+']= 62;
    dtable[(unsigned char)'/']= 63;
    dtable[(unsigned char)'=']= 0;


    while (varLength >= 4) {
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
        varLength-=4;
        src += 4;
    }

    if( varLength > 0 )
    {
        if( bits_to_skip == 4 && varLength == 2 ) {
            a = dtable[src[0]];
            b = dtable[src[1]];

            target[0] = a << 2 | b >> 4;
        } else if( bits_to_skip == 2 && varLength == 3 ) {
            a = dtable[src[0]];
            b = dtable[src[1]];
            c = dtable[src[2]];

            target[0] = a << 2 | b >> 4;
            target[1] = b << 4 | c >> 2;
        } else {
            logger->warning("Error in base64. #bits to skip doesn't match length\n");
            logger->warning("skip %d bits, %d chars left\n\"%s\"\n", bits_to_skip, varLength, src);
            return -1;
        }
    }

    return 0;
}

/**
 * Decompress format 80 compressed data.
 *
 * @param image_in compressed data.
 * @param image_out pointer to output uncompressed data.
 * @return size of uncompressed data.
 */
int Compression::decode80(const unsigned char image_in[], unsigned char image_out[])
{
	//----------
	// Format80
	//----------
	//
	//There are several different commands, with different sizes : form 1 to 5
	//bytes.
	//The positions mentioned below always refer to the destination buffer (i.e.
	//the uncompressed image). The relative positions are relative to the current
	//position in the destination buffer, which is one byte beyond the last written
	//byte.
	//
	//I will give some sample code at the end.
	//
	//(1) 1 byte
	//      +---+---+---+---+---+---+---+---+
	//      | 1 | 0 |   |   |   |   |   |   |
	//      +---+---+---+---+---+---+---+---+
	//              \_______________________/
	//                         |
	//                       Count
	//
	//      This one means : copy next Count bytes as is from Source to Dest.
	//
	//(2) 2 bytes
	//  +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
	//  | 0 |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
	//  +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
	//      \___________/\__________________________________________________/
	//            |                             |
	//         Count-3                    Relative Pos.
	//
	//  This means copy Count bytes from Dest at Current Pos.-Rel. Pos. to
	//  Current position.
	//  Note that you have to add 3 to the number you find in the bits 4-6 of the
	//  first byte to obtain the Count.
	//  Note that if the Rel. Pos. is 1, that means repeat Count times the previous
	//  byte.
	//
	//(3) 3 bytes
	//  +---+---+---+---+---+---+---+---+   +---------------+---------------+
	//  | 1 | 1 |   |   |   |   |   |   |   |               |               |
	//  +---+---+---+---+---+---+---+---+   +---------------+---------------+
	//          \_______________________/                  Pos
	//                     |
	//                 Count-3
	//
	//  Copy Count bytes from Pos, where Pos is absolute from the start of the
	//  destination buffer. (Pos is a word, that means that the images can't be
	//  larger than 64K)
	//
	//(4) 4 bytes
	//  +---+---+---+---+---+---+---+---+   +-------+-------+  +-------+
	//  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0 |   |       |       |  |       |
	//  +---+---+---+---+---+---+---+---+   +-------+-------+  +-------+
	//                                            Count          Color
	//
	//  Write Color Count times.
	//  (Count is a word, color is a byte)
	//
	//(5) 5 bytes
	//  +---+---+---+---+---+---+---+---+   +-------+-------+  +-------+-------+
	//  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |   |       |       |  |       |       |
	//  +---+---+---+---+---+---+---+---+   +-------+-------+  +-------+-------+
	//                                            Count               Pos
	//
	//  Copy Count bytes from Dest. starting at Pos. Pos is absolute from the start
	//  of the Destination buffer.
	//  Both Count and Pos are words.
	//
	//These are all the commands I found out. Maybe there are other ones, but I
	//haven't seen them yet.
	//
	//All the images end with a 80h command.
	//
	//To make things more clearer here's a piece of code that will uncompress the
	//image.
	//
	//  DP = destination pointer
	//  SP = source pointer
	//  Source and Dest are the two buffers
	//
	//
	//  SP:=0;
	//  DP:=0;
	//  repeat
	//    Com:=Source[SP];
	//    inc(SP);
	//    b7:=Com shr 7;  {b7 is bit 7 of Com}
	//    case b7 of
	//      0 : begin  {copy command (2)}
	//            {Count is bits 4-6 + 3}
	//            Count:=(Com and $7F) shr 4 + 3;
	//            {Position is bits 0-3, with bits 0-7 of next byte}
	//            Posit:=(Com and $0F) shl 8+Source[SP];
	//            Inc(SP);
	//            {Starting pos=Cur pos. - calculated value}
	//            Posit:=DP-Posit;
	//            for i:=Posit to Posit+Count-1 do
	//            begin
	//              Dest[DP]:=Dest[i];
	//              Inc(DP);
	//            end;
	//          end;
	//      1 : begin
	//            {Check bit 6 of Com}
	//            b6:=(Com and $40) shr 6;
	//            case b6 of
	//              0 : begin  {Copy as is command (1)}
	//                    Count:=Com and $3F;  {mask 2 topmost bits}
	//                    if Count=0 then break; {EOF marker}
	//                    for i:=1 to Count do
	//                    begin
	//                      Dest[DP]:=Source[SP];
	//                      Inc(DP);
	//                      Inc(SP);
	//                    end;
	//                  end;
	//              1 : begin  {large copy, very large copy and fill commands}
	//                    {Count = (bits 0-5 of Com) +3}
	//                    {if Com=FEh then fill, if Com=FFh then very large copy}
	//                    Count:=Com and $3F;
	//                    if Count<$3E then {large copy (3)}
	//                    begin
	//                      Inc(Count,3);
	//                      {Next word = pos. from start of image}
	//                      Posit:=Word(Source[SP]);
	//                      Inc(SP,2);
	//                      for i:=Posit to Posit+Count-1 do
	//                      begin
	//                        Dest[DP]:=Dest[i];
	//                        Inc(DP);
	//                      end;
	//                    end
	//                    else if Count=$3F then   {very large copy (5)}
	//                    begin
	//                      {next 2 words are Count and Pos}
	//                      Count:=Word(Source[SP]);
	//                      Posit:=Word(Source[SP+2]);
	//                      Inc(SP,4);
	//                      for i:=Posit to Posit+Count-1 do
	//                      begin
	//                        Dest[DP]:=Dest[i];
	//                        Inc(DP);
	//                      end;
	//                    end else
	//                    begin   {Count=$3E, fill (4)}
	//                      {Next word is count, the byte after is color}
	//                      Count:=Word(Source[SP]);
	//                      Inc(SP,2);
	//                      b:=Source[SP];
	//                      Inc(SP);
	//                      for i:=0 to Count-1 do
	//                      begin
	//                        Dest[DP]:=b;
	//                        inc(DP);
	//                      end;
	//                    end;
	//                  end;
	//            end;
	//          end;
	//    end;
	//  until false;
	//
	//Note that you won't be able to compile this code, because the typecasting
	//won't work. (But I'm sure you'll be able to fix it).


	// To resume :
	//    0 copy 0cccpppp p
	//    1 copy 10cccccc
	//    2 copy 11cccccc p p
	//    3 fill 11111110 c c v
	//    4 copy 11111111 c c p p


    const unsigned char* copyp;
    const unsigned char* readp = image_in;
    unsigned char* writep = image_out;
    Uint32 code;
    Uint32 count;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint16 bigend; // temporary big endian var
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
 * @param image_in compressed data.
 * @param image_out pointer to pu uncompressed data in.
 * @return size of uncompressed data.
 *
 *
----------
 Format40
----------

As I said before the images in Format40 must be xor-ed over a previous image,
or against a black screen (as in the .WSA format).
It is used when there are only minor changes between an image and a following
one.

Here I'll assume that the old image is in Dest, and that the Dest pointer is
set to the beginning of that buffer.

As for the Format80, there are many commands :


(1) 1 byte
               byte
  +---+---+---+---+---+---+---+---+
  | 1 |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
      \___________________________/
                   |
                 Count

  Skip count bytes in Dest (move the pointer forward).

(2) 3 bytes
              byte                           word
  +---+---+---+---+---+---+---+---+  +---+-----+-------+
  | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  | 0 | ... |       |
  +---+---+---+---+---+---+---+---+  +---+-----+-------+
                                         \_____________/
                                                |
                                              Count

  Skip count bytes.

(3) 3 bytes
                byte                              word
  +---+---+---+---+---+---+---+---+  +---+---+-----+-------+
  | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  | 1 | 0 | ... |       |
  +---+---+---+---+---+---+---+---+  +---+---+-----+-------+
                                             \_____________/
                                                   |
                                                 Count

 Xor next count bytes. That means xor count bytes from Source with bytes
 in Dest.

(4) 4 bytes
              byte                               word           byte
  +---+---+---+---+---+---+---+---+  +---+---+-----+-------+  +-------+
  | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  | 1 | 1 | ... |       |  |       |
  +---+---+---+---+---+---+---+---+  +---+---+-----+-------+  +-------+
                                             \_____________/    value
                                                   |
                                                 Count

  Xor next count bytes in Dest with value.

5) 1 byte
               byte
  +---+---+---+---+---+---+---+---+
  | 0 |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
      \___________________________/
                   |
                 Count

  Xor next count bytes from source with dest.

6) 3 bytes
              byte                     byte       byte
  +---+---+---+---+---+---+---+---+  +-------+  +-------+
  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  |       |  |       |
  +---+---+---+---+---+---+---+---+  +-------+  +-------+
                                       Count      Value

  Xor next count bytes with value.


All images end with a 80h 00h 00h command.

I think these are all the commands, but there might be some other.
If you find anything new, please e-mail me.

As before here's some code :

  DP = destination pointer
  SP = source pointer
  Source is buffer containing the Format40 data
  Dest   is the buffer containing the image over which the second has
         to be xor-ed


  SP:=0;
  DP:=0;
  repeat
    Com:=Source[SP];
    Inc(SP);

    if (Com and $80)<>0 then {if bit 7 set}
    begin
      if Com<>$80 then  {small skip command (1)}
      begin
        Count:=Com and $7F;
        Inc(DP,Count);
      end
      else  {Big commands}
      begin
        Count:=Word(Source[SP]);
        if Count=0 then break;
        Inc(SP,2);

        Tc:=(Count and $C000) shr 14;  {Tc=two topmost bits of count}

        case Tc of
          0,1 : begin  {Big skip (2)}
                  Inc(DP,Count);
                end;
          2 : begin {big xor (3)}
                Count:=Count and $3FFF;
                for i:=1 to Count do
                begin
                  Dest[DP]:=Dest[DP] xor Source[SP];
                  Inc(DP);
                  Inc(SP);
                end;
              end;
          3 : begin  {big repeated xor (4)}
                Count:=Count and $3FFF;
                b:=Source[SP];
                Inc(SP);
                for i:=1 to Count do
                begin
                  Dest[DP]:=Dest[DP] xor b;
                  Inc(DP);
                end;
              end;
        end;
      end;
    end else  {xor command}
    begin
      Count:=Com;
      if Count=0 then
      begin {repeated xor (6)}
        Count:=Source[SP];
        Inc(SP);
        b:=Source[SP];
        Inc(SP);
        for i:=1 to Count do
        begin
          Dest[DP]:=Dest[DP] xor b;
          Inc(DP);
        end;
      end else  {copy xor (5)}
        for i:=1 to Count do
        begin
          Dest[DP]:=Dest[DP] xor Source[SP];
          Inc(DP);
          Inc(SP);
        end;
    end;
  until false;

 *
 */
int Compression::decode40(const unsigned char image_in[], unsigned char image_out[])
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
 * @param s compressed data.
 * @param d pointer to pu uncompressed data in.
 * @param cb_s size of compressed data?
 * @return size of uncompressed data?
 */
int Compression::decode20(const unsigned char* s, unsigned char* d, int cb_s)
{
    const unsigned char* r = s;
    const unsigned char* r_end = s + cb_s;
    unsigned char* w = d;

    while (r < r_end)
    {
        int v = *r++;
        if (v)
        {
            *w++ = v;
        }
        else
        {
            v = *r++;
            memset(w, 0, v);
            w += v;
        }
    }

    return w - d;
}
