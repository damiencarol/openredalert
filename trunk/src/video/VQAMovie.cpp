// VQAMovie.cpp
// 1.5

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

#include "VQAMovie.h"

#include <cassert>
#include <cctype>
#include <stdexcept>
#include <string>

#include "SDL/SDL_audio.h"
#include "SDL/SDL_events.h"
#include "SDL/SDL_mutex.h"

#include "include/config.h"
#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "video/GraphicsEngine.h"
#include "include/imageproc.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "audio/SoundCommon.h"
#include "audio/SoundEngine.h"
#include "audio/SoundUtils.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"

using std::runtime_error;
using std::string;

namespace pc {
	extern GraphicsEngine * gfxeng;
	extern SoundEngine * sfxeng;
}
extern Logger * logger;

/**
 * @param filename the name of the vqamovie.
 */
VQA::VQAMovie::VQAMovie(const char* filename) : vqafile(0), CBF_LookUp(0),
        CBP_LookUp(0), VPT_Table(0), offsets(0), sndbuf(0)
{
    string fname(filename);


    if (toupper(filename[0]) != 'X') {
        fname += ".VQA";
        vqafile = VFSUtils::VFS_Open(fname.c_str());
    }

    if (vqafile == 0) {
		logger->error ("Failed to load vqa movie %s\n", fname.c_str());
        throw runtime_error("No VQA file");
    }
    // Get header information for the vqa.  If the header is corrupt, we can die now.
    vqafile->seekSet(0);
    if (!DecodeFORMChunk()) {
        logger->error("VQA: Invalid FORM chunk\n");
        throw runtime_error("VQA: Invalid FORM chunk\n");
    }

#ifdef DEBUG_VQA
	logger->note ("%s line %i: VQA header for file\t\t\t\t= %s\n", __FILE__, __LINE__, filename);
	logger->note ("%s line %i: Header signature\t\t\t\t= %c%c%c%c%c%c%c%c\n", __FILE__, __LINE__, header.Signature[0], header.Signature[1], header.Signature[2], header.Signature[3], header.Signature[4], header.Signature[5], header.Signature[6], header.Signature[7]);
	logger->note ("%s line %i: Header size\t\t\t\t= %i\n", __FILE__, __LINE__, header.RStartPos);
	logger->note ("%s line %i: Header version\t\t\t\t= %i\n", __FILE__, __LINE__, header.Version);
	logger->note ("%s line %i: Header flags\t\t\t\t= %i\n", __FILE__, __LINE__, header.Flags);
	logger->note ("%s line %i: Header NumFrames\t\t\t\t= %i\n", __FILE__, __LINE__, header.NumFrames);
	logger->note ("%s line %i: Header width\t\t\t\t= %i\n", __FILE__, __LINE__, header.Width);
	logger->note ("%s line %i: Header height\t\t\t\t= %i\n", __FILE__, __LINE__, header.Height);
	logger->note ("%s line %i: Header block width\t\t\t= %i\n", __FILE__, __LINE__, header.BlockW);
	logger->note ("%s line %i: Header block height\t\t\t= %i\n", __FILE__, __LINE__, header.BlockH);
	logger->note ("%s line %i: Header framerate\t\t\t\t= %i\n", __FILE__, __LINE__, header.FrameRate);
	logger->note ("%s line %i: Header numb frames with same lookup table= %i\n", __FILE__, __LINE__, header.CBParts);
	logger->note ("%s line %i: Header colors\t\t\t\t= %i\n", __FILE__, __LINE__, header.Colors);
	logger->note ("%s line %i: Header max image blocks\t\t\t= %i\n", __FILE__, __LINE__, header.MaxBlocks);
	logger->note ("%s line %i: Header Unknown1\t\t\t\t= %i\n", __FILE__, __LINE__, header.Unknown1);
	logger->note ("%s line %i: Header Unknown2\t\t\t\t= %i\n", __FILE__, __LINE__, header.Unknown2);
	logger->note ("%s line %i: Header Sound freq\t\t\t= %i\n", __FILE__, __LINE__, header.Freq);
	logger->note ("%s line %i: Header Sound channels\t\t\t= %i\n", __FILE__, __LINE__, header.Channels);
	logger->note ("%s line %i: Header Sound bits\t\t\t= %i\n", __FILE__, __LINE__, header.Bits);
#endif

    // DecodeFINFChunk uses offsets
    offsets = new Uint32[header.NumFrames];
    if (!DecodeFINFChunk()) {
        delete[] offsets;
        logger->error("VQA: Invalid FINF chunk\n");
        throw runtime_error("VQA: Invalid FINF chunk\n");
    }

    CBF_LookUp = new Uint8[0x0ff00 << 4];	// (0x0ff00 * 8)
    CBP_LookUp = new Uint8[0x0ff00 << 4];	// (0x0ff00 * 8)
    VPT_Table = new Uint8[lowoffset<<1];
    CBPOffset = 0; // Starting offset of CBP Look up table must be zero
    CBPChunks = 0; // Number of CBPChunks

    INIFile *inif = GetConfig("freecnc.ini");
    scaleVideo = inif->readInt("video", "scaleMovies", 0);
    videoScaleQuality = inif->readInt("video", "movieQuality", 0);

    //logger->debug("Video is %dfps %d %d %d\n", header.FrameRate, header.Freq, header.Channels, header.Bits);

    if (SDL_BuildAudioCVT(&cvt, AUDIO_S16SYS, header.Channels, header.Freq, SOUND_FORMAT, SOUND_CHANNELS, SOUND_FREQUENCY) < 0) {
        logger->error("Could not build SDL_BuildAudioCVT filter\n");
        return;
    }

    empty = SDL_CreateSemaphore(0);
    full = SDL_CreateSemaphore(1);
    sndBufLock = SDL_CreateMutex();

    sndbuf = new Uint8[2 * (16384 << 2)];
    sndbufMaxEnd = sndbuf + 2 * (16384 << 2);
    sndbufStart = sndbuf;
    sndbufEnd = sndbuf;
}

/**
 *
 */
VQA::VQAMovie::~VQAMovie()
{
    delete[] CBF_LookUp;
    delete[] CBP_LookUp;
    delete[] VPT_Table;
    delete[] offsets;

    /// Close the Virtual file
    VFSUtils::VFS_Close(vqafile);

    SDL_DestroySemaphore(empty);
    SDL_DestroySemaphore(full);
    SDL_DestroyMutex(sndBufLock);

    delete[] sndbuf;
}

/**
 */
void VQA::VQAMovie::play()
{
#ifdef __MORPHOS__
    printf("Sorry we skip videos due to a bug in Morphos SDL_Mixer\n");
    return;
#endif

    if (vqafile == 0){
        return;
    }

    if (pc::sfxeng->NoSound()) {
        return;
    }



    SDL_Surface* frame;
    SDL_Surface* cframe;
    SDL_Rect dest;
    SDL_Event esc_event;
    static ImageProc scaler;


	dest.w = header.Width<<1;
	dest.h = header.Height<<1;
	dest.x = (pc::gfxeng->getWidth()-(header.Width<<1))>>1;
	dest.y = (pc::gfxeng->getHeight()-(header.Height<<1))>>1;

    pc::gfxeng->clearScreen();

    // Seek to first frame/snd information of vqa
    vqafile->seekSet(offsets[0]);

    sndindex = 0;
    sndsample = 0;

    // create the frame to store the image in.
    frame = SDL_CreateRGBSurface(SDL_SWSURFACE, header.Width, header.Height, 8, 0, 0, 0, 0);

    // Initialise the scaler
    if (scaleVideo){
        scaler.initVideoScale(frame, videoScaleQuality);
    }

    Uint32 jumplen;
    Uint32 sndlen = DecodeSNDChunk(sndbufEnd);
    sndbufEnd += sndlen;
#ifdef DEBUG_VQA
	printf ("%s line %i: Decode SND done, size = %i\n", __FILE__, __LINE__, sndlen);
#endif

    // The jump len is how far ahead we are allowed to be
    jumplen = sndlen;

    // Start music (aka the sound)
    pc::sfxeng->SetMusicHook(AudioHook, this);
    for (Uint32 framenum = 0; framenum < header.NumFrames; ++framenum) {
        SDL_LockMutex(sndBufLock);

        // Decode SND Chunk first
        sndlen = DecodeSNDChunk(sndbufEnd);
        sndbufEnd += sndlen;

        // Signal we have more audio data
        SDL_SemPost(empty);

        // Wait until the buffer has been emptied again
        if (sndlen > 0){
	        while ((sndbufEnd - sndbufStart) >= (Sint32)(jumplen - (2 * sndlen))) {
        	    SDL_UnlockMutex(sndBufLock);
	            SDL_SemWait(full);
        	    SDL_LockMutex(sndBufLock);
	        }
	    }

        // Unlock the mutex
        SDL_UnlockMutex(sndBufLock);

        if (!DecodeVQFRChunk(frame)) {
            logger->error("VQA: Decoding VQA frame\n");
            break;
        }

        if (scaleVideo) {
            cframe = scaler.scaleVideo(frame);
            pc::gfxeng->drawVQAFrame(cframe);
        } else {
            pc::gfxeng->drawVQAFrame(frame);
        }

        while (SDL_PollEvent(&esc_event)) {
            if (esc_event.type == SDL_KEYDOWN) {
                if (esc_event.key.state != SDL_PRESSED) {
                    break;
                }
                if (esc_event.key.keysym.sym == SDLK_ESCAPE) {
                    framenum = header.NumFrames; // Jump to end of video
                    break;
                }
            }
        }
    }

    pc::sfxeng->SetMusicHook(0, 0);

    SDL_FreeSurface(frame);
    if (scaleVideo) {
        scaler.closeVideoScale();
    }
}

/**
 *
 */
bool VQA::VQAMovie::ReadChunk()
{
	//printf ("%s line %i: ***Read Chunk***\n", __FILE__, __LINE__);
	//printf ("%s line %i: Chunk Start = %i\n", __FILE__, __LINE__, vqafile->tell());

	Uint8 chunkid[4];
	vqafile->readByte(chunkid, 4);

	// We have read the PINF chunk...
	Uint32 len;
	//Uint8  byte;
	vqafile->readDWord(&len, 1);
	len = ((((Uint8*)(&len))[0] << 24) |
		   (((Uint8*)(&len))[1] << 16) |
		   (((Uint8*)(&len))[2] << 8) |
		   ((Uint8*)(&len))[3]);

	//len = SDL_Swap32(len);
	printf ("Length = %i\n", (int)len);
	vqafile->seekSet(vqafile->tell() + len);

	//printf ("Curpos = %i\n", vqafile->tell());

	// try again
//	vqafile->readByte(chunkid, 4);
//	printf ("%s line %i: Chunkid = %c%c%c%c\n", __FILE__, __LINE__, chunkid[0], chunkid[1], chunkid[2], chunkid[3]);
	return true;
}

/**
 *
 */
bool VQA::VQAMovie::DecodeFORMChunk()
{
    char chunkid[4];

//	Uint16 Startpos = vqafile->tell();

//	printf ("Curpos = %i\n", vqafile->tell());

    vqafile->readByte((Uint8*)chunkid, 4);

    if (strncmp(chunkid, "FORM", 4)) {
        logger->error("VQA: Decoding FORM Chunk - Expected \"FORM\", got \"%c%c%c%c\"\n",
                chunkid[0], chunkid[1], chunkid[2], chunkid[3]);
        return false;
    }

    Uint32 chunklen;
    vqafile->readDWord(&chunklen, 1);
    chunklen = SDL_Swap32(chunklen) + 42;

//	printf ("FORM chunck Length = %i\n", chunklen);

    // skip chunklen
//    vqafile->seekCur(4);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    vqafile->readByte(reinterpret_cast<Uint8 *>(&header), header_size);
#else
    vqafile->readByte((Uint8*)&header.Signature, sizeof(header.Signature));
    vqafile->readDWord(&header.RStartPos, 1);
    vqafile->readWord(&header.Version, 1);
    vqafile->readWord(&header.Flags, 1);
    vqafile->readWord(&header.NumFrames, 1);
    vqafile->readWord(&header.Width, 1);
    vqafile->readWord(&header.Height, 1);
    vqafile->readByte(&header.BlockW, 1);
    vqafile->readByte(&header.BlockH, 1);
    vqafile->readByte(&header.FrameRate, 1);
    vqafile->readByte(&header.CBParts, 1);
    vqafile->readWord(&header.Colors, 1);
    vqafile->readWord(&header.MaxBlocks, 1);
    vqafile->readWord(&header.Unknown1, 1);
    vqafile->readDWord(&header.Unknown2, 1);
    vqafile->readWord(&header.Freq, 1);
    vqafile->readByte(&header.Channels, 1);
    vqafile->readByte(&header.Bits, 1);
    vqafile->readByte((Uint8*)&header.Unknown3, sizeof(header.Unknown3));
#endif
    // Weird: need to byteswap on both BE and LE
    // readDWord probably swaps back on BE.
    header.RStartPos = SDL_Swap32(header.RStartPos);
    if (strncmp((const char*)header.Signature, "WVQAVQHD", 8) == 1) {
        logger->error("VQA: Invalid header: Expected \"WVQAVQHD\", got \"%c%c%c%c%c%c%c%c\"\n",
                header.Signature[0], header.Signature[1], header.Signature[2],
                header.Signature[3], header.Signature[4], header.Signature[5],
                header.Signature[6], header.Signature[7]);
        return false;
    }
    if (header.Version != 2) {
        logger->error("VQA: Unsupported version: Expected 2, got %i\n", header.Version);
        return false;
    }
    // Set some constants based on the header
    lowoffset = (header.Width/header.BlockW)*(header.Height/header.BlockH);
    modifier = header.BlockH == 2 ? 0x0f : 0xff;
	BlockS = header.BlockH * header.BlockW;

//	printf ("%s line %i: Modifier = 0x%x\n", __FILE__, __LINE__, modifier);

//	vqafile->seekSet (Startpos + chunklen);
    return true;
}

/**
 *
 */
bool VQA::VQAMovie::DecodeFINFChunk()
{
	Uint8 chunkid[4];
	vqafile->readByte(chunkid, 4);

	if (strncmp((const char*)chunkid, "FINF", 4) ) {
		// if this chunk id is PINF skip it..

		// Scip PINF chunk
		if (strncmp((const char*)chunkid, "PINF", 4) == 0) {
			// We have read the PINF chunk...
			Uint32 len;
			vqafile->readDWord(&len, 1);
			len = SDL_Swap32(len);
			vqafile->seekSet(vqafile->tell() + len);
			// try again
			vqafile->readByte(chunkid, 4);
		}

		if (strncmp((const char*)chunkid, "FINF", 4) ) {
			logger->error("VQA: Decoding FINF chunk - Expected \"FINF\", got \"%c%c%c%c\"\n", chunkid[0], chunkid[1], chunkid[2], chunkid[3]);
			return false;
		}
	}
//	printf ("%s line %i: Now at chunk: %s\n", __FILE__, __LINE__, chunkid);

	Uint32 chunklen;
	vqafile->readDWord(&chunklen, 1);
	chunklen = SDL_Swap32(chunklen);
	if (static_cast<Uint32>(header.NumFrames << 2) != chunklen) {
		logger->error("VQA: Invalid chunk length (%i != %i)\n", header.NumFrames << 2, chunklen);
		return false;
	}

	vqafile->readByte((Uint8*)offsets, header.NumFrames<<2);
	Uint32* offp = offsets;
	for (Uint32 framenum = 0; framenum < header.NumFrames; ++framenum, ++offp) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		*offp = SDL_Swap32(*offp);
#endif
		*offp &= 0x3FFFFFFF;
		*offp <<= 1;
	}
	return true;
}

/**
 * Decodes SND Chunk
 *
 * @param pointer to store the decoded chunk
 * @return length of chunk
 */
Uint32 VQA::VQAMovie::DecodeSNDChunk(Uint8 *outbuf)
{
    Uint32 chunklen;
    Uint8 *inbuf;

    // header Flags tells us that this VQA does not support sounds.. so lets quit
    if (!(header.Flags & 1))
        return 0;

    // seek to correct offset
    if (vqafile->tell() & 1)
        vqafile->seekCur(1);

    Uint8 chunkid[4];

	vqafile->readByte(chunkid, 4);

	if (strncmp((const char*)chunkid, "SND", 3) ) {
		// Scip SN2J chunk's
		if (strncmp((const char*)chunkid, "SN2J", 4) == 0) {
			// We have read the PINF chunk...
			Uint32 len;
			vqafile->readDWord(&len, 1);
			len = SDL_Swap32(len);
			vqafile->seekSet(vqafile->tell() + len);
			// try again
			vqafile->readByte(chunkid, 4);
		}
		if (strncmp((const char*)chunkid, "SND", 3) ) {
			logger->error("VQA: Decoding SND chunk - Expected \"SNDX\", got \"%c%c%c%c\"\n", chunkid[0], chunkid[1], chunkid[2], chunkid[3]);
			return 0; // Returning zero here, to set length of sound chunk to zero
		}
	}

    vqafile->readDWord(&chunklen, 1);
    chunklen = SDL_Swap32(chunklen);

    inbuf = new Uint8[chunklen];

    vqafile->readByte(inbuf, chunklen);

    switch (chunkid[3]/*VQA_HI_BYTE(chunkid)*/) {
    case '0': // Raw uncompressed wave data
        memcpy(outbuf, inbuf, chunklen);
        break;
    case '1': // Westwoods own algorithm
        // @todo: Add support for this algorithm
        logger->error("VQA: Decoding SND chunk - sound compressed using unsupported westwood algorithm\n");
        //Sound::WSADPCM_Decode(outbuf, inbuf, chunklen, uncompressed_size)
        chunklen = 0;
        break;
    case '2': // IMA ADPCM algorithm
        SoundUtils::IMADecode(outbuf, inbuf, chunklen, sndsample, sndindex);
        chunklen <<= 2; // IMA ADPCM decompresses sound to a size 4 times larger than the compressed size
        break;
    default:
        logger->error("VQA: Decoding SND chunk - sound in unknown format\n");
        chunklen = 0;
        break;
    }

    delete[] inbuf;

    return chunklen;
}

/**
 * Decodes VQFR Chunk into one frame(?)
 *
 * @param pointer to decoded frame
 * @param Current Frame to decode
 */
bool VQA::VQAMovie::DecodeVQFRChunk(SDL_Surface *frame)
{
    Uint32 chunkid;
    Uint32 chunklen;
    Uint8 HiVal, LoVal;
    Uint8 CmpCBP, compressed; // Is CBP Look up table compressed or not
    int cpixel, bx, by, fpixel;
    SDL_Color CPL[256];

    if (vqafile->tell() & 1) {
        vqafile->seekCur(1);
    }
	Uint8 chunkid1[4];
	vqafile->readByte(chunkid1, 4);
	if (strncmp((const char*)chunkid1, "VQFR", 3) ) {
		logger->error("VQA: Decoding VQFR chunk - Expected \"VQFR\", got \"%c%c%c%c\"\n", chunkid1[0], chunkid1[1], chunkid1[2], chunkid1[3]);
		return 0; // Returning zero here, to set length of sound chunk to zero
	}

    vqafile->readDWord(&chunklen, 1);
    chunklen = SDL_Swap32(chunklen);
    CmpCBP = 0;

    bool done = false;
    // Read chunks until we get to the VPT chunk
    while (!done) {
        if (vqafile->tell() & 1)
            vqafile->seekCur(1);

        vqafile->readDWord(&chunkid, 1);
        compressed = VQA_HI_BYTE(chunkid) == 'Z';
        chunkid = chunkid & vqa_t_mask;
        switch (chunkid) {
        case vqa_cpl_id:
            DecodeCPLChunk(CPL);
            SDL_SetColors(frame, CPL, 0, header.Colors);
            break;
        case vqa_cbf_id:
            DecodeCBFChunk(compressed);
            break;
        case vqa_cbp_id:
            CmpCBP = compressed;
            DecodeCBPChunk();
            break;
        case vqa_vpt_id:
            // This chunk is always the last one
            DecodeVPTChunk(compressed);
            done = true;
            break;
        default:
            DecodeUnknownChunk();
        }
    }

    // Optimize me harder
    cpixel = 0;
    fpixel = 0;
	//printf ("%s line %i: Height = %i, Width = %i, BlockH = %i, BlockW = %i\n", __FILE__, __LINE__, header.Height, header.Width, header.BlockH, header.BlockW);
    for (by = 0; by < header.Height; by += header.BlockH) {
        for (bx = 0; bx < header.Width; bx += header.BlockW) {
            HiVal = VPT_Table[ fpixel + lowoffset]; // formerly known as LowVal
            LoVal = VPT_Table[ fpixel ]; // formerly known as TopVal
            if (HiVal == modifier) {
				for (Uint8 brow = 0; brow < header.BlockH; brow++)
					memset((Uint8 *)frame->pixels + cpixel + (brow*header.Width), LoVal, header.BlockW);
            } else {
				for (Uint8 brow = 0; brow < header.BlockH; brow++)
					memcpy((Uint8 *)frame->pixels + cpixel + (brow*header.Width), &CBF_LookUp[(((HiVal<<8)|LoVal)*BlockS)+4*brow], header.BlockW);
            }
            cpixel += header.BlockW;
            fpixel++;
        }
        cpixel += header.Width*(header.BlockH - 1);
    }

    //    if( !((CurFrame + 1) % header.CBParts) ) {
    if (CBPChunks & ~7) {
        if (CmpCBP == 1) {
            Uint8 CBPUNZ[0x0ff00 << 4];
            Compression::decode80(CBP_LookUp, CBPUNZ);
            memcpy(CBF_LookUp, CBPUNZ, 0x0ff00 << 4);
        } else {
            memcpy(CBF_LookUp, CBP_LookUp, 0x0ff00 << 4);
        }
        CBPOffset = 0;
        CBPChunks = 0;
    }
    return true;
}

/**
 *
 */
inline void VQA::VQAMovie::DecodeCBPChunk()
{
    Uint32 chunklen;

    vqafile->readDWord(&chunklen, 1);
    chunklen = SDL_Swap32(chunklen);

    vqafile->readByte(CBP_LookUp + CBPOffset, chunklen);
    CBPOffset += chunklen;
    CBPChunks++;
}

/**
 *
 */
inline void VQA::VQAMovie::DecodeVPTChunk(Uint8 Compressed)
{
    Uint32 chunklen;

    vqafile->readDWord( &chunklen, 1);
    chunklen = SDL_Swap32(chunklen);

    if (Compressed) {
        Uint8 *VPTZ; // Compressed VPT_Table
        VPTZ = new Uint8[chunklen];
        vqafile->readByte(VPTZ, chunklen);
        Compression::decode80(VPTZ, VPT_Table);
        delete[] VPTZ;
    } else { // uncompressed VPT chunk. never found any.. but might be some
        vqafile->readByte(VPT_Table, chunklen);
    }
}

/**
 *
 */
inline void VQA::VQAMovie::DecodeCBFChunk(Uint8 Compressed)
{
    Uint32 chunklen;

    vqafile->readDWord(&chunklen, 1);
    chunklen = SDL_Swap32(chunklen);

    if (Compressed) {
        Uint8 *CBFZ; // Compressed CBF table
        CBFZ = new Uint8[chunklen];
        vqafile->readByte(CBFZ, chunklen);
        Compression::decode80(CBFZ, CBF_LookUp);
        delete[] CBFZ;
    } else {
        vqafile->readByte(CBF_LookUp, chunklen);
    }
}

/**
 *
 */
inline void VQA::VQAMovie::DecodeCPLChunk(SDL_Color *palette)
{
    Uint32 chunklen;
    int i;

    vqafile->readDWord(&chunklen, 1);
    chunklen = SDL_Swap32(chunklen);

    for (i = 0; i < header.Colors; i++) {
        vqafile->readByte((Uint8*)&palette[i], 3);
        palette[i].r <<= 2;
        palette[i].g <<= 2;
        palette[i].b <<= 2;
    }
}

/**
 *
 */
inline void VQA::VQAMovie::DecodeUnknownChunk()
{
    Uint32 chunklen;

    vqafile->readDWord(&chunklen, 1);
    chunklen = SDL_Swap32(chunklen);
    logger->error("Unknown chunk at %0x for %i bytes.\n", vqafile->tell(), chunklen);

    vqafile->seekCur(chunklen);
}

/**
 *
 */
void VQA::VQAMovie::AudioHook(void* udata, Uint8* stream, int len)
{
    VQAMovie* vqa = (VQAMovie* )udata;

    // We only need half of what they ask for, since SDL_ConvertAudio
    // makes our audio bigger
    len = len / vqa->cvt.len_mult;

    // While the buffer isn't filled, wait
    SDL_LockMutex(vqa->sndBufLock);
    while ((vqa->sndbufEnd - vqa->sndbufStart) < len) {
        SDL_UnlockMutex(vqa->sndBufLock);
        SDL_SemWait(vqa->empty);
        SDL_LockMutex(vqa->sndBufLock);
    }

    // Copy our buffered data into the stream
    memcpy(stream, vqa->sndbufStart, len);

    // Convert the audio to the format we like
    vqa->cvt.buf = stream;
    vqa->cvt.len = len;

    if (SDL_ConvertAudio(&vqa->cvt) < 0) {
        logger->warning("Could not run conversion filter: %s\n", SDL_GetError());
        return;
    }

    // Move the start up
    vqa->sndbufStart += len;

    // If we are too near the end, move back some
    if ((vqa->sndbufEnd + (16384 << 2)) >= vqa->sndbufMaxEnd) {

        int len = (int)(vqa->sndbufEnd - vqa->sndbufStart);

        // Move remaining data down
        memmove(vqa->sndbuf, vqa->sndbufStart, len);

        // Reset the start & ends
        vqa->sndbufStart = vqa->sndbuf;
        vqa->sndbufEnd = vqa->sndbufStart + len;
    }

    SDL_SemPost(vqa->full);

    SDL_UnlockMutex(vqa->sndBufLock);
}
