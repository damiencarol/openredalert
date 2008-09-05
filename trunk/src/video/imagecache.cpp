// ImageCache.cpp
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

#include "ImageCache.h"

#include <algorithm>

#include "ImageNotFound.h"
#include "SHPImage.h"
#include "ImageCacheEntry.h"

using std::string;
using std::map;
using std::transform;

/**
 * Return the numb of images contained in the shp (find the image by
 *  number)
 */
Uint32 ImageCache::getNumbImages(Uint32 imgnum)
{
	// Returns number of image
	return getImage(imgnum, 0).NumbImages;
}

/**
 * Return the numb of images contained in the shp (find the image by name)
 */
Uint32 ImageCache::getNumbImages(const char* fname)
{
	Uint32 imgnum; // number of main image

	// Try to load the image or return index of main image (image 0 of SHP)
	imgnum = loadImage(fname);

	// return the number of image in the SHP
	return getImage(imgnum).NumbImages;
}

/**
 * Assigns the source for images and purges both caches.
 *
 * @param imagepool The imagepool to use.
 */
void ImageCache::setImagePool(vector<SHPImage*>* imagepool)
{
	// set the Image pool
    this->imagepool = imagepool;
    // Clear cache
    cache.clear();
    // Clear previous cache
    prevcache.clear();
}

/**
 * Gets an image from the cache.
 *
 * If the image isn't cached, it is loaded in from the imagepool.
 *
 * @param imgnum The number of the image in the imagepool.
 *           The imgnum stores 3 values
 *           <--------------32bits------------>
 *           0123456701234567 01234 56701234567
 *           [index of image] [pal] [ frame # ]
 *           The index of the image, the palette to use, and the frame number
 * @returns An ImageCacheEntry object containing the image and the shadow.
 */
ImageCacheEntry& ImageCache::getImage(Uint32 imgnum)
{
    map<Uint32, ImageCacheEntry>::iterator cachepos;

    // Check that index is not -1
    if (imgnum == (Uint32)-1) {
        throw ImageNotFound("ImageCache::getImage imgnum == -1");
    }

    //logger->debug("%d %d\n", imgnum, cache.size());

    cachepos = cache.find(imgnum);
    if (cachepos != cache.end()) {
        // Found image
        return cachepos->second;
    }
    // Didn't find it, so check old cache
    if (!prevcache.empty()) {
        cachepos = prevcache.find(imgnum);
        if (cachepos != prevcache.end()) {
            // Found image in old cache
            cache[imgnum] = cachepos->second;
            // Entry in newer cache takes ownership of the surface pointers
            prevcache[imgnum].clear();
            return cachepos->second;
        }
    }

	// Didn't find it in either cache, so will retrieve from
    // the imagepool.
	// This is the only part of the imagecache that uses code
    // from elsewhere i.e. change the type of the vector and
    // how the decoded sprite data is stored into the "entry"
    // and it'll work elsewhere.

	ImageCacheEntry& entry = cache[imgnum];

	//Uint8 palnum = p::ppool->getLPlayer()->getMultiColour ();

    // Palette is ((imgnum>>11)&0x1f).
//		printf ("%s line %i: Palette number = %i\n", __FILE__, __LINE__, ((imgnum>>11)&0x1f));

	Uint32 NumbImages	= (*imagepool)[imgnum>>16]->getNumImg();
	string ImageName	= (*imagepool)[imgnum>>16]->getFileName();
	Uint8 palnumb		= ((imgnum>>11)&0x1f);
	Uint16 imagenumb	= imgnum&0x7FF;

	if ( (unsigned)imagenumb > (unsigned)NumbImages ){
		printf ("%s line %i: Error want image %i but %s only got %i images, pal_num = %i\n", __FILE__, __LINE__, imagenumb, ImageName.c_str(), NumbImages, palnumb);
		entry.image = 0;
		entry.shadow = 0;
		return entry;
	}

	entry.ImageName		= ImageName;
	entry.NumbImages	= NumbImages;
	(*imagepool)[imgnum>>16]->getImage(imagenumb, &(entry.image), &(entry.shadow), palnumb);

	// Return the entry
    return entry;
}

ImageCacheEntry& ImageCache::getImage(Uint32 imgnum, Uint32 frame)
{
    return getImage(imgnum | (frame &0x7FF));
}

void ImageCache::setImage(SDL_Surface* Image, SDL_Surface* Shadow, Uint32 imgnum)
{
    map<Uint32, ImageCacheEntry>::iterator cachepos;

    // Check that index is not -1
    if (imgnum == (Uint32)-1) {
    	throw ImageNotFound("ImageCache::setImage imgnum == -1");
    }


    cachepos = cache.find(imgnum);
    if (cachepos != cache.end()) {
        // Found image
		cachepos->second.image = Image;
		cachepos->second.shadow = Shadow;
    }
    // Didn't find it, so check old cache
    if (!prevcache.empty()) {
        cachepos = prevcache.find(imgnum);
        if (cachepos != prevcache.end()) {
            // Found image in old cache
            cache[imgnum] = cachepos->second;
            // Entry in newer cache takes ownership of the surface pointers
            prevcache[imgnum].clear();

			cachepos->second.image = Image;
			cachepos->second.shadow = Shadow;

        }
    }
}

/**
 * Load an image with this name
 *
 * @param fname Name of the file.
 */
Uint32 ImageCache::loadImage(const char* fname)
{
	// Returns index in the cache of the loaded image
    return loadImage(fname, -1);
}

/**
 * Load an image with this name
 *
 * @param fname Name of the file.
 * @param scaleq scale factor (-1 = no factor)
 */
Uint32 ImageCache::loadImage(const char* fname, int scaleq)
{
    string name; // Name of the file wanted
    map<string, Uint32>::iterator cachentry; // Iterator use to parse cache

    // coppy the char* string in c++ string
    name = string(fname);

    // UPPER the fname string
    transform(name.begin(), name.end(), name.begin(), toupper);

    // Parse the vector to find the image
    cachentry = namecache.find(name);

    // if the iterator is at end (image NOT FOUND) load the image and
    // push it on the vector
    if (cachentry == namecache.end())
    {
    	// Get the initial index
        Uint32 size = static_cast<Uint32>(imagepool->size());
        try {
            imagepool->push_back(new SHPImage(name.c_str(), scaleq));
            namecache[name] = size<<16;
            return size<<16;
        } catch (ImageNotFound&) {
            namecache[name] = (Uint32)-1;
        }
    }

    // Check if the index is -1 and trow ImageNotFound
    if ((Uint32)-1 == namecache[name]) {
    	// Throw an ImageNotFound exception
    	throw ImageNotFound("ImageCache::loadImage namecache[name] == -1");
    }

    // Returns the number of the image in imagepool
    return cachentry->second;
}

/**
 * Rotates caches so a new cache will be created next time an image is
 * requested.
 */
void ImageCache::newCache()
{
	// Clean previous cache
	prevcache.clear();
	// Swap the 2 cache (prevcache = cache AND cache = a new cache)
    prevcache.swap(cache);
}

/**
 * Clears both the current and previous caches
 */
void ImageCache::flush()
{
	// Clean previous cache
	prevcache.clear();
	// Clean cache
    cache.clear();
}

/**
 * Removes all images from the image pool and clears both caches
 */
void ImageCache::Cleanup()
{
	// Delete all objects in the pool
	for (unsigned int i = 0; i < imagepool->size(); i++){
		delete (*imagepool)[i];
	}
	// clean the pool
	imagepool->clear();
	// clean the prevcache
	prevcache.clear();
	// clean the cache
	cache.clear();
}
