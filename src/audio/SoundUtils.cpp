// SoundUtils.cpp
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

#include "SoundUtils.h"

#include <algorithm>

using std::copy;


#include "SoundCommon.h"

Uint8 SoundUtils::Clip (int value)
{
    if (value > 255)
	    return 255;
	if (value < 0)
	    return 0;

    return (Uint8) value;
}


Uint8 SoundUtils::Clip (int value, int min, int max)
{
    if (value > max)
	    return (Uint8)max;
	if (value < min)
	    return (Uint8)min;

    return (Uint8) value;
}

/**
 */
void SoundUtils::IMADecode(Uint8 *output, Uint8 *input, Uint16 compressed_size, Sint32& sample, Sint32& index)
{
    int  Samples;
    Uint8 Code;
    int  Sign;
    int  Delta;
    Uint8 *InP;
    Sint16 *OutP;
    Uint16 uncompressed_size;

    // Data used in IMA Decoding algorithm
    static int Steps[89] = {
        7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,
        34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,
        157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,
        724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,
        2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,
        9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,
        27086,29794,32767
    };

    uncompressed_size = compressed_size * 2;

    if (compressed_size==0)
        return;

    InP=(Uint8 *)input;
    OutP=(Sint16 *)output;

    for (Samples=0; Samples<uncompressed_size; Samples++) {
        if (Samples&1)          // If Samples is odd
            Code=(*InP++)>>4;    // Extract upper 4 bits
        else                         // Samples is even
            Code=(*InP) & 0x0F;  // Extract lower 4 bits

        Sign=(Code & 0x08)!=0;  // If topmost bit is set, Sign=true
        Code&=0x07;             // Keep lower 3 bits

        Delta=0;
        if ((Code & 0x04)!=0)
            Delta += Steps[index];
        if ((Code & 0x02)!=0)
            Delta += Steps[index]>>1;
        if ((Code & 0x01)!=0)
            Delta += Steps[index]>>2;
        Delta += Steps[index]>>3;

        if (Sign)
            Delta=-Delta;

        sample+=Delta;

        // When sample is too high it should wrap
//        if (sample > 0x7FFF)
//            sample = Delta;

        if (sample > 32767)
	        sample = 32767;
	    if (sample < -32768)
	        sample = -32768;

        // @todo I'm not happy with this, if we do sample = 0 or sample -= 0x7FFF we get
        // popping noises. sample = Delta seems to have the smallest pop
        // If we do nothing, then audio after X seconds gets stuck

        *OutP++ = sample; //Clip<Sint16>(sample);

        index+=Sound::Indexes[Code];
        index = Clip(index, 0, 88);
    }
}

/**
 * Decode Westwood's ADPCM format.  Original code from ws-aud.txt
 * by Asatur V. Nazarian
 */
void SoundUtils::WSADPCM_Decode(Uint8 *output, Uint8 *input, Uint16 compressed_size, Uint16 uncompressed_size)
{
    Sint16 CurSample;
    Uint8  code;
    Sint8  count;
    Uint16 i;
    Uint16 shifted_input;

    // This data are used in this decoding algorithm
    static int WSTable2bit[4] = {-2,-1,0,1};
    static int WSTable4bit[16] = {
        -9, -8, -6, -5, -4, -3, -2, -1,
         0,  1,  2,  3,  4,  5,  6,  8
    };


    if (compressed_size==uncompressed_size)
    {
    	copy(input, input+uncompressed_size, output);
    	return;
    }

    CurSample=0x80;
    i=0;

    Uint16 bytes_left = uncompressed_size;
    while (bytes_left>0) { // expecting more output
        shifted_input=input[i++];
        shifted_input<<=2;
        code=HIBYTE(shifted_input);
        count=LOBYTE(shifted_input)>>2;
        switch (code) // parse code
        {
        case 2: // no compression...
            if (count & 0x20) {
                count<<=3;  // here it's significant that (count) is signed:
                CurSample+=count>>3; // the sign bit will be copied by these shifts!
                *output++ = Clip(CurSample);
                bytes_left--; // one byte added to output
            } else {
                // copy (count+1) bytes from input to output

                /// @todo This version doesn't produce the same result as the loop below
                //std::copy(input+i, input+i+count+1, output);
                //bytes_left -= (count + 1);
                //i += (count + 1);

                // Old version for reference:
                for (++count; count > 0; --count) {
                    --bytes_left;
                    *output++ = input[i++];
                }

                CurSample=input[i-1]; // set (CurSample) to the last byte sent to output
            }
            break;
        case 1: // ADPCM 8-bit -> 4-bit
            for (count++;count>0;count--) { // decode (count+1) bytes
                code=input[i++];
                CurSample+= WSTable4bit[(code & 0x0F)]; // lower nibble
                *output++ = Clip(CurSample);
                CurSample+= WSTable4bit[(code >> 4)]; // higher nibble
                *output++ = Clip(CurSample);
                bytes_left-=2; // two bytes added to output
            }
            break;
        case 0: // ADPCM 8-bit -> 2-bit
            for (count++;count>0;count--) { // decode (count+1) bytes
                code=input[i++];
                CurSample+= WSTable2bit[(code & 0x03)]; // lower 2 bits
                *output++ = Clip(CurSample);
                CurSample+= WSTable2bit[((code>>2) & 0x03)]; // lower middle 2 bits
                *output++ = Clip(CurSample);
                CurSample+= WSTable2bit[((code>>4) & 0x03)]; // higher middle 2 bits
                *output++ = Clip(CurSample);
                CurSample+= WSTable2bit[((code>>6) & 0x03)]; // higher 2 bits
                *output++ = Clip(CurSample);
                bytes_left-=4; // 4 bytes sent to output
            }
            break;
        default: // just copy (CurSample) (count+1) times to output
            /// @todo This version doesn't produce the same result as the loop below
            //std::fill(output, output+count+2, Clip<Uint8>(CurSample));
            //bytes_left -= count+1;

            // Old version for reference:
            for (count++;count>0;count--,bytes_left--)
                *output++ = Clip(CurSample);
        }
    }
}
