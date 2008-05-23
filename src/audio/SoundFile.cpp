#include "SoundFile.h"

#include <cassert>
#include <limits>
#include <memory>

#include "SDL/SDL_audio.h"

#include "SoundCommon.h"
#include "SoundUtils.h"
#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"

extern Logger * logger;


/**
 */
SoundFile::SoundFile() : fileOpened(false)
{
    if (!initconv) {
        #ifdef RA_SOUND_ENGINE
        if (SDL_BuildAudioCVT(&eightbitconv, AUDIO_U8, 1, 22050, SOUND_FORMAT, SOUND_CHANNELS, SOUND_FREQUENCY) < 0) {
            logger->error("Could not build 8bit->16bit conversion filter\n");
            return;
        }

        if (SDL_BuildAudioCVT(&monoconv, AUDIO_S16SYS, 1, 22050, SOUND_FORMAT, SOUND_CHANNELS, SOUND_FREQUENCY) < 0) {
            logger->error("Could not build mono->stereo conversion filter\n");
            return;
        }
        #endif
        initconv = true;
    }
}

SoundFile::~SoundFile(){
    Close();
}

bool SoundFile::Open(const std::string& filename){
    Close();

    // Open file
    file = VFSUtils::VFS_Open(filename.c_str());
    if (file == NULL) {
        logger->error("Sound: Could not open file \"%s\".\n", filename.c_str());
        return false;
    }

    if (file->fileSize() < 12) {
        logger->error("Sound: Could not open file \"%s\": Invalid file size.\n", filename.c_str());
    }

    // Parse header
    file->readWord(&frequency,1);
    file->readDWord(&comp_size,1);
    file->readDWord(&uncomp_size,1);
    file->readByte(&flags,1);
    file->readByte(&type,1);

#ifdef RA_SOUND_ENGINE
    // Check for known format
    if (type == 1) {
        conv = &eightbitconv;
    } else if (type == 99) {
        conv = &monoconv;
    } else {
        logger->error("Sound: Could not open file \"%s\": Corrupt header (Unknown type: %i).\n", filename.c_str(), type);
        return false;
    }

    if (frequency != 22050)
        logger->warning("Sound: \"%s\" needs converting from %iHz (should be 22050Hz)\n", filename.c_str(), frequency);

    imaSample = 0;
    imaIndex  = 0;

    this->filename = filename;
    fileOpened = true;

    return true;
#else
    return false;
#endif
}

void SoundFile::Close(){
    if (fileOpened) {
    	VFSUtils::VFS_Close(file);
        fileOpened = false;
    }
}

Uint32 SoundFile::Decode(SampleBuffer& buffer, Uint32 length){
    if (!initconv || !fileOpened)
        return SOUND_DECODE_ERROR;

    assert(buffer.empty());

#ifdef RA_SOUND_ENGINE
    Uint32 max_size = length == 0 ? uncomp_size * conv->len_mult : length;
    buffer.resize(max_size);

    Uint16 comp_sample_size, uncomp_sample_size;
    Uint32 ID;

    //if (offset < 12)
    //    offset = 12;
    //file->seekSet(offset);


    Uint32 written = 0;
    while ((file->tell()+8) < file->fileSize()) {
        // Each sample has a header
        file->readWord(&comp_sample_size, 1);
        file->readWord(&uncomp_sample_size, 1);
        file->readDWord(&ID, 1);

        if (comp_sample_size > (SOUND_MAX_CHUNK_SIZE)) {
            logger->warning("Size data for current sample too large\n");
            return SOUND_DECODE_ERROR;
        }

        // abort if id was wrong */
        if (ID != 0xDEAF) {
            logger->warning("Sample had wrong ID: %x\n", ID);
            return SOUND_DECODE_ERROR;
        }

        if (written + uncomp_sample_size*conv->len_mult > max_size) {
            file->seekCur(-8); // rewind stream back to headers
            buffer.resize(written); // Truncate buffer.
            return SOUND_DECODE_STREAMING;
        }

        // compressed data follows header
        file->readByte(chunk, comp_sample_size);

        if (type == 1) {
            SoundUtils::WSADPCM_Decode(tmpbuff, chunk, comp_sample_size, uncomp_sample_size);
        } else {
            SoundUtils::IMADecode(tmpbuff, chunk, comp_sample_size, imaSample, imaIndex);
        }
        conv->buf = tmpbuff;
        conv->len = uncomp_sample_size;
        if (SDL_ConvertAudio(conv) < 0) {
            logger->warning("Could not run conversion filter: %s\n", SDL_GetError());
            return SOUND_DECODE_ERROR;
        }
        memcpy(&buffer[written], tmpbuff, uncomp_sample_size*conv->len_mult);
        //offset += 8 + comp_sample_size;
        written += uncomp_sample_size*conv->len_mult;
    }

    // Truncate if final sample was too small.
    if (written < max_size)
        buffer.resize(written);

    return SOUND_DECODE_COMPLETED;
#else
    return SOUND_DECODE_ERROR;
#endif
}