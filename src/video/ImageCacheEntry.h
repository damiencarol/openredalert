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
    
    /** Name of the SHP image file */
	string ImageName;
	/** Number of the image in the SHP image */
	int	NumbImages;
	/** Image */
    SDL_Surface *image;
    /** shadow of the image */
    SDL_Surface *shadow;
};

#endif //IMAGECACHEENTRY_H
