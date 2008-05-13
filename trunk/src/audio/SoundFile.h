#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_audio.h"

#include "SoundBuffer.h"

class VFile;

class SoundFile
{
public:
    SoundFile();
    ~SoundFile();

    bool Open(const std::string& filename);
    void Close();

    // Length is the max size in bytes of the uncompressed sample, returned
    // in buffer. If length is zero, the full file is decoded.
    Uint32 Decode(SampleBuffer& buffer, Uint32 length = 0);

private:
    // File data
    std::string filename;
    VFile* file;
    //Uint32 offset;
    bool fileOpened;

    // Header information
    Uint16 frequency;
    Uint32 comp_size; Uint32 uncomp_size;
    Uint8 flags; Uint8 type;

    // IMADecode state
    Sint32 imaSample;
    Sint32 imaIndex;


    SDL_AudioCVT* conv;
};

#endif
