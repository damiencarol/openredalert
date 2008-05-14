#include "ImageCacheEntry.h"

#include "SDL/SDL_video.h"

/** 
 * Ensure that the pointers start off pointing somewhere that's safe to
 * delete.
 */
ImageCacheEntry::ImageCacheEntry()
{
	// Set pointer to NULL
	image = 0;
	// Set pointer to NULL
	shadow = 0;
}

/** 
 * Frees the surfaces.  If the destructor is invoked by 
 * a copy of the main instance, the program will most likely
 * crash or otherwise mess up horribly.
 */
ImageCacheEntry::~ImageCacheEntry()
{
	// If pointer are not NULL
	if (image != NULL)
	{
		// Free surface
		SDL_FreeSurface(image);
		// Set pointer to NULL
		image = NULL;
	}
	// If pointer are not NULL
	if (shadow != NULL)
	{
		// Free surface
		SDL_FreeSurface(shadow);
		// Set pointer to NULL
		shadow = NULL;
	}
}

/**
 * This function exists because we don't have shared pointers.
 */
void ImageCacheEntry::clear()
{
	image = 0;
	shadow = 0;
}
