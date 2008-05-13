#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <map>
#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class ImageCacheEntry;
class SHPImage;

using std::map;
using std::string;
using std::vector;

/**
 * Class to bufferize the SHP image pool
 */
class ImageCache
{
public:

	Uint32 getNumbImages(Uint32 imgnum);
	Uint32 getNumbImages(const char* fname);
    void setImagePool(vector<SHPImage *> *imagepool);
    ImageCacheEntry& getImage(Uint32 imgnum);
    ImageCacheEntry& getImage(Uint32 imgnum, Uint32 frame);

	void setImage(SDL_Surface* Image, SDL_Surface* Shadow, Uint32 imgnum);

    /** @TODO Arbitrary post-processing filter, e.g. colour fiddling.
     * ImageCacheEntry& getText(const char*); // Caches text
     * 1) typedef void (FilterFunc*)(Uint32, ImageCacheEntry&);  OR
     * 2) Policy class that provides this API:
     *    struct FilterFunc : public binary_functor(?) {
     *         void operator()(Uint32, ImageCacheEntry&);
     *    };
     * void applyFilter(const char* fname, const FilterFunc&);
     */
    /** Loads the shpimage fname into the imagecache. */
    Uint32 loadImage(const char* fname);
    /** Loads the shpimage fname into the imagecache. */
    Uint32 loadImage(const char* fname, int scaleq);

    void newCache();
    void flush();
	void Cleanup(void);

private:
    map<Uint32, ImageCacheEntry> cache; 
    map<Uint32, ImageCacheEntry> prevcache;
    map<string, Uint32> namecache;
    vector<SHPImage*>* imagepool; // SHP Image pool

    /** @link association */
    /*# ImageCacheEntry lnkImageCacheEntry; */
};

#endif //IMAGECACHE_H
