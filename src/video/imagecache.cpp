#include <cctype>
#include "include/common.h"
#include "video/ImageCache.h"
#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "video/ImageNotFound.h"
#include "SHPImage.h"
#include "ImageCacheEntry.h"
#include "ImageCache.h"

using std::string;
using std::map;
using std::transform;

extern Logger * logger;

ImageCache::ImageCache()
{
}

ImageCache::~ImageCache()
{
}

/** 
 * Return the numb of images contained in the shp (find the image by number)
 */
Uint32 ImageCache::getNumbImages(Uint32 imgnum)
{
	return getImage(imgnum, 0).NumbImages;
}

/** 
 * Return the numb of images contained in the shp (find the image by name)
 */
Uint32 ImageCache::getNumbImages(const char* fname)
{
	Uint32 imgnum = loadImage(fname);
	return getImage( imgnum ).NumbImages;
}

/** 
 * Assigns the source for images and purges both caches.
 * 
 * @param imagepool The imagepool to use.
 */
void ImageCache::setImagePool(std::vector<SHPImage*>* imagepool)
{
    this->imagepool = imagepool;
    cache.clear();
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
 * @returns A class containing the image and the shadow.
 */
ImageCacheEntry& ImageCache::getImage(Uint32 imgnum)
{
    std::map<Uint32, ImageCacheEntry>::iterator cachepos;

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

	Uint32 NumbImages		= (*imagepool)[imgnum>>16]->getNumImg();
	std::string ImageName	= (*imagepool)[imgnum>>16]->getFileName();
	Uint8 palnumb			= ((imgnum>>11)&0x1f);
	Uint16 imagenumb		= imgnum&0x7FF;

	if ( (unsigned)imagenumb > (unsigned)NumbImages ){
		printf ("%s line %i: Error want image %i but %s only got %i images, pal_num = %i\n", __FILE__, __LINE__, imagenumb, ImageName.c_str(), NumbImages, palnumb);
		entry.image = NULL;
		entry.shadow = NULL;
		return entry;
	}

	entry.ImageName		= ImageName;
	entry.NumbImages	= NumbImages;
	(*imagepool)[imgnum>>16]->getImage(imagenumb, &(entry.image), &(entry.shadow), palnumb);

    return entry;
}

ImageCacheEntry& ImageCache::getImage(Uint32 imgnum, Uint32 frame) 
{
    return getImage(imgnum | (frame &0x7FF));
}

void ImageCache::setImage(SDL_Surface*Image, SDL_Surface* Shadow, Uint32 imgnum)
{
    std::map<Uint32, ImageCacheEntry>::iterator cachepos;

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

Uint32 ImageCache::loadImage(const char* fname)
{
    return loadImage(fname, mapscaleq);
}

Uint32 ImageCache::loadImage(const char* fname, int scaleq) 
{
    string name;
    map<string, Uint32>::iterator cachentry;
    
    // coppy the char* string in c++ string
    name = string(fname);
    
    // UPPER the fname string
    transform(name.begin(), name.end(), name.begin(), toupper);

    cachentry = namecache.find(name);
    if (cachentry == namecache.end()) {
        Uint32 size = static_cast<Uint32>(imagepool->size());
        try {
            imagepool->push_back(new SHPImage(name.c_str(), scaleq));
            namecache[name] = size<<16;
            return size<<16;
        } catch (ImageNotFound&) {
            namecache[name] = (Uint32)-1;
        }
    }
    
    // Check that index is not -1
    if ((Uint32)-1 == namecache[name]) {
    	throw ImageNotFound("ImageCache::loadImage namecache[name] == -1");
    }

    return cachentry->second;
}

/** 
 * Rotates caches so a new cache will be created next time an image is 
 * requested.
 */
void ImageCache::newCache() {
	logger->warning ("%s line %i: Rotate cache??\n", __FILE__, __LINE__);
    prevcache.clear();
    prevcache.swap(cache);
}

/** 
 * Clears both the current and previous caches
 */
void ImageCache::flush() {
	logger->warning ("%s line %i: Flush image cache\n", __FILE__, __LINE__);
    prevcache.clear();
    cache.clear();
}

/** 
 * Removes all images from the image pool and clears both caches
 */
void ImageCache::Cleanup(void)
{
	//logger->warning ("%s line %i: Cleanup image cache\n", __FILE__, __LINE__);
	for (unsigned int i = 0; i < imagepool->size(); i++){
		delete (*imagepool)[i];
	}
	imagepool->clear();
	prevcache.clear();
	cache.clear();
}
