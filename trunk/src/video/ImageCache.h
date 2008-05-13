#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <map>
#include <string>
#include <vector>

#include "video/SHPImage.h"
#include "video/ImageCacheEntry.h"
#include "SHPImage.h"
#include "ImageCacheEntry.h"

class ImageCache
{
public:
    ImageCache();
    ~ImageCache();

	Uint32 getNumbImages(Uint32 imgnum);
	Uint32 getNumbImages(const char* fname);
    void setImagePool(std::vector<SHPImage *> *imagepool);
    ImageCacheEntry& getImage(Uint32 imgnum);
    ImageCacheEntry& getImage(Uint32 imgnum, Uint32 frame);

	void setImage(SDL_Surface*Image, SDL_Surface* Shadow, Uint32 imgnum);

    /** @TODO Arbitrary post-processing filter, e.g. colour fiddling.
     * ImageCacheEntry& getText(const char*); // Caches text
     * 1) typedef void (FilterFunc*)(Uint32, ImageCacheEntry&);  OR
     * 2) Policy class that provides this API:
     *    struct FilterFunc : public binary_functor(?) {
     *         void operator()(Uint32, ImageCacheEntry&);
     *    };
     * void applyFilter(const char* fname, const FilterFunc&);
     */
    /// @brief Loads the shpimage fname into the imagecache.
    Uint32 loadImage(const char* fname);
    Uint32 loadImage(const char* fname, int scaleq);

    void newCache();
    void flush();
	void Cleanup(void);

private:
    std::map<Uint32, ImageCacheEntry> cache; std::map<Uint32, ImageCacheEntry> prevcache;
    std::map<std::string, Uint32> namecache;
    std::vector<SHPImage*>* imagepool;
};

#endif //IMAGECACHE_H
