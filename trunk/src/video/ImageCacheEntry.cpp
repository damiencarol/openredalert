#include "ImageCacheEntry.h"

#include "SDL/SDL_video.h"

/** 
 * @brief Ensure that the pointers start off pointing somewhere that's safe to
 * delete.
 */
ImageCacheEntry::ImageCacheEntry() : image(0), shadow(0) {}

/** 
 * @brief Frees the surfaces.  If the destructor is invoked by 
 * a copy of the main instance, the program will most likely
 * crash or otherwise mess up horribly.
 */
ImageCacheEntry::~ImageCacheEntry() {
	if (image != NULL){
		SDL_FreeSurface(image);
		image = NULL;
	}
	if (shadow != NULL){
    	SDL_FreeSurface(shadow);
		shadow = NULL;
	}
}

/**
 * @brief This function exists because we don't have shared
 *  pointers.
 */
void ImageCacheEntry::clear() {
    image = 0;
    shadow = 0;
}
