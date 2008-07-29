// SoundFile.h
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

#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_audio.h"

#include "SoundBuffer.h"

class VFile;

using std::string;

/**
 * Audio file
 */
class SoundFile
{
public:
    SoundFile();
    ~SoundFile();

    bool Open(const string& filename);
    void Close();

    // Length is the max size in bytes of the uncompressed sample, returned
    // in buffer. If length is zero, the full file is decoded.
    Uint32 Decode(SampleBuffer& buffer, Uint32 length = 0);

private:
    /** Name of the audio file */
    string filename;
    /** Reference to the file */
    VFile* file;
    //Uint32 offset;
    bool fileOpened;

    // Header information
    Uint16 frequency;
    Uint32 comp_size;
    Uint32 uncomp_size;
    Uint8 flags;
    Uint8 type;

    // IMADecode state
    Sint32 imaSample;
    Sint32 imaIndex;


    SDL_AudioCVT* conv;
};

#endif //SOUNDFILE_H
