// VQAHeader.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef VQAHEADER_H
#define VQAHEADER_H

#include "SDL/SDL_types.h"

namespace VQA {

// Copied from XCC Mixer (xcc.ra2.mods) by Olaf van der Spek

  /// @todo Fix/verify this for Big Endian
  inline Uint8 VQA_HI_BYTE(Uint32 x) {
      return (x & 0xff000000) >> 24;
  }

  const Uint32 vqa_t_mask = 0x00ffffff;

  // Each of these constants is derived from e.g. *(Uint32*)"CBF\0" etc.
  // We can't have these in the above form if we want to use them as switch
  // labels, even though they're consts.....
  const Uint32 vqa_cbf_id = 0x464243;
  const Uint32 vqa_cbp_id = 0x504243;
  const Uint32 vqa_cpl_id = 0x4C5043;
  const Uint32 vqa_snd_id = 0x444E53;
  const Uint32 vqa_vpt_id = 0x545056;
  const Uint32 vqa_vpr_id = 0x525056;
  const Uint32 vqa_vqfl_id = 0x4C465156;
  const Uint32 vqa_vqfr_id = 0x52465156;

/** size of vqa header - so i know how many bytes to read in the file */
const size_t header_size = sizeof(Uint8) * 28 + sizeof(Uint16) * 9 + sizeof(Uint32) * 2;


class VQAHeader
{
public:
    Uint8 Signature[8];	// Always "WVQAVQHD"
    Uint32 RStartPos;	// Size of header minus Signature (always 42 bytes) big endian
    Uint16 Version;	// VQA Version. 2 = C&C TD and C&C RA, 3 = C&C TS
    Uint16 Flags;	// VQA Flags. If bit 1 is set, VQA has sound
    Uint16 NumFrames;	// Number of frames
    Uint16 Width;	// Width of each frame
    Uint16 Height;	// Height of each frame
    Uint8 BlockW;	// Width of each image block (usually 4)
    Uint8 BlockH;	// Height of each image block (usually 2)
    Uint8 FrameRate;	// Number of frames per second?
    Uint8 CBParts;	// Number of frames that use the same lookup table (always 8 in TD and RA)
    Uint16 Colors;	// Number of colors used in Palette
    Uint16 MaxBlocks;	// Maximum number of image blocks??
    Uint16 Unknown1;
    Uint32 Unknown2;
    Uint16 Freq;	// Sound frequency
    Uint8 Channels;	// 1 = mono; 2 = stereo (TD and RA always 1) (TS is always 2)
    Uint8 Bits;		// 8 or 16 bit sound
    Uint8 Unknown3[14]; 
};

}

#endif //VQAHEADER_H
