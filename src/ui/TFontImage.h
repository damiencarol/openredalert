#ifndef TFONTIMAGE_H
#define TFONTIMAGE_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

using std::string;
using std::vector;

class TFontImage {
public:
	std::vector<SDL_Rect>	chrdest;
	std::string 			fontname;
	SDL_Surface				*fontimg;
};

#endif

