// ImageCacheEntry.cpp
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
	if (image != 0)
	{
		// Free surface
		SDL_FreeSurface(image);
		// Set pointer to NULL
		image = NULL;
	}
	// If pointer are not NULL
	if (shadow != 0)
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
