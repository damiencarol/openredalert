#ifndef FONTCACHE_H
#define FONTCACHE_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "TFontImage.h"

using std::string;
using std::vector;

class FontCache {
public:
    FontCache(void);
    ~FontCache(void);
    void Add(TFontImage Image);
    void Add(std::string fontname, SDL_Surface * img, std::vector < SDL_Rect > chrdest);
    bool Get(std::string fontname, SDL_Surface * * fontimg, std::vector < SDL_Rect > & chrdest);

private:
    std::vector < TFontImage > font_cache;
};

#endif //FONTCACHE_H
