#ifndef PCXIMAGE_H
#define PCXIMAGE_H

#include "SDL/SDL_types.h"

#include "PCXHeader.h"
#include "include/imageproc.h"

class VFile;

class PCXImage {
public:
    PCXImage(const char * fname, int scaleq);
    ~PCXImage();
    SDL_Surface * getImage();
private:
    void loadImage();
    void readPalette();
    Uint32 imgsize;
    Uint8 * pcxdata;
    SDL_Color palette[256];
    PCXHeader header;
    int scaleq;
    ImageProc scaler;
    SDL_Surface * image;
    bool Use16ColorPalette;
    bool HeaderError;
};

#endif
