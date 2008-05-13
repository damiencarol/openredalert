#ifndef IMAGEPROC_H
#define IMAGEPROC_H

#include "video/Renderer.h"

class ImageProc
{
public:
    ImageProc();
    ~ImageProc();
    //generic scale function that supports bpp's
    // It really should take the dimentions you want for output
    SDL_Surface* scale(SDL_Surface* input, Sint8 quality);
    SDL_Surface* scale(SDL_Surface* input, Sint32 width, Sint32 Heigth);

    //function the decrease the map to a size that it'll fit in the radar screen.
    //NOTE: it's not to produce the radar image, only the minimap
    SDL_Surface* minimapScale(SDL_Surface *input, Uint8 pixsize);

    //Functions to scale the VQAs on-the-fly
    //quality setting is not supported.
    //ever tried on-the-fly linear or bicubic interpolation on video??

    //initialise the videoOutputBuffer. Required before the start of scaling a VQA. Only needed once per VQA.
    void closeVideoScale();
    void initVideoScale(SDL_Surface* input, int videoq);
    //The actual scaling function. It will return the image with the size of the screen
    SDL_Surface* scaleVideo(SDL_Surface* input);

private:
    // these are the actual scaling functions, scales src to dest
    void scaleInterlace( SDL_Surface *src, SDL_Surface *dest);
    void scaleNearest( SDL_Surface *src, SDL_Surface *dest);
    void scaleLinear( SDL_Surface *src, SDL_Surface *dest);
    void scaleLinear8bppsrc(SDL_Surface *src, SDL_Surface *dest);

    //this is permanent stuff to avoid free, delete every function call when scaling VQAs
    SDL_Surface* videoOutputBuffer;
    int videoq;
};

#endif
