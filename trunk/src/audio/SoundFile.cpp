// SoundFile.cpp
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

#include "SoundFile.h"

#include <sstream>
#include <limits>
#include <memory>
#include <string>

#include "SDL/SDL_audio.h"

#include "SOUND_DECODE_STATE.h"
#include "SoundCommon.h"
#include "SoundUtils.h"
#include "Logger.hpp"
#include "vfs/vfs.h"
#include "vfs/VFile.h"

using std::string;
using std::stringstream;

/**
 */
SoundFile::SoundFile() : fileOpened(false)
{
    if (!initconv)
    {
        if (SDL_BuildAudioCVT(&eightbitconv, AUDIO_U8, 1, 22050, SOUND_FORMAT, SOUND_CHANNELS, SOUND_FREQUENCY) < 0) {
            Logger::getInstance()->Error("Could not build 8bit->16bit conversion filter.");
            return;
        }

        if (SDL_BuildAudioCVT(&monoconv, AUDIO_S16SYS, 1, 22050, SOUND_FORMAT, SOUND_CHANNELS, SOUND_FREQUENCY) < 0) {
            Logger::getInstance()->Error("Could not build mono->stereo conversion filter.");
            return;
        }
        initconv = true;
    }
}

/**
 *
 */
SoundFile::~SoundFile()
{
    Close();
}

/**
 *
 */
bool SoundFile::Open(const string& filename)
{
    Close();

    // Open file
    file = VFSUtils::VFS_Open(filename.c_str());
    if (file == 0) {
        Logger::getInstance()->Error("Sound: Could not open file " + filename);
        return false;
    }

    if (file->fileSize() < 12) {
        Logger::getInstance()->Error("Sound: Could not open file " + filename + " Invalid file size.");
    }

    // Parse header
    file->readWord(&frequency,1);
    file->readDWord(&comp_size,1);
    file->readDWord(&uncomp_size,1);
    file->readByte(&flags,1);
    file->readByte(&type,1);


    // Check for known format
    if (type == 1) {
        conv = &eightbitconv;
    } else if (type == 99) {
        conv = &monoconv;
    } else {
        stringstream message;
        message << "Sound: Could not open file '" << filename << "': Corrupt header (Unknown type: " << type << ").";
        Logger::getInstance()->Error(message.str());
        return false;
    }

    if (frequency != 22050)
    {
        stringstream message;
        message << "Sound: '" << filename << "' needs converting from " 
            << frequency << "Hz (should be 22050Hz)";
        Logger::getInstance()->Warning(message.str());
    }

    imaSample = 0;
    imaIndex  = 0;

    this->filename = filename;
    fileOpened = true;

    return true;
}

/**
 *
 */
void SoundFile::Close()
{
    if (fileOpened)
    {
        /// Close the file in the VFS
        VFSUtils::VFS_Close(file);
        fileOpened = false;
    }
}

/**
 */
Uint32 SoundFile::Decode(SampleBuffer& buffer, Uint32 length)
{
    if (!initconv || !fileOpened)
    {
        return SOUND_DECODE_ERROR;
    }

    // Check that the buffer is empty
    if (!buffer.empty())
    {
        Logger::getInstance()->Error("[SoundFile::Decode()] the buffer to decode in is not empty !");
        return SOUND_DECODE_ERROR;
    }

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
            Logger::getInstance()->Warning("Size data for current sample too large.");
            return SOUND_DECODE_ERROR;
        }

        // abort if id was wrong
        // HACK : ID = 0xDEAF ???
        if (ID != 0xDEAF)
        {
            stringstream message;
            message << "Sample had wrong ID:" << ID;
            Logger::getInstance()->Warning(message.str());
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
            Logger::getInstance()->Warning("Could not run conversion filter.");
            Logger::getInstance()->Warning(SDL_GetError());
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
}
