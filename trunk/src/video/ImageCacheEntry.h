#ifndef IMAGECACHEENTRY_H
#define IMAGECACHEENTRY_H

#include <string>

#include "SDL/SDL_video.h"

using std::string;

class ImageCacheEntry
{
public:
    ImageCacheEntry();
    ~ImageCacheEntry();
    void clear();
	std::string ImageName;
	int			NumbImages;
    SDL_Surface *image;
    SDL_Surface *shadow;
};

#endif //IMAGECACHEENTRY_H
