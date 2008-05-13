#ifndef SHPIMAGE_H
#define SHPIMAGE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "SHPHeader.h"
#include "SHPBase.h"

using std::string;

/**
 * shpimage - code to load/decode shp files
 */
class SHPImage : SHPBase
{
public:
	SHPImage(const char * fname, Sint8 scaleq);
	~SHPImage();

	void getImage(Uint16 imgnum, SDL_Surface * * img, SDL_Surface * * shadow,
			Uint8 palnum);
	void getImageAsAlpha(Uint16 imgnum, SDL_Surface * * img);

	Uint32 getWidth() const;
	Uint32 getHeight() const;
	Uint16 getNumImg() const;
	std::string getFileName() const;

private:
	static SDL_Color shadowpal[2];
	static SDL_Color alphapal[6];

	void DecodeSprite(Uint8 * imgdst, Uint16 imgnum);
	Uint8 * shpdata;
	SHPHeader lnkHeader;
};

#endif //SHPIMAGE_H
