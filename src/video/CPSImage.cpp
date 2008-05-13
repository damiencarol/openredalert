#include "CPSImage.h"

//#include <cstdlib>
#include <string>

#include "video/Renderer.h"
#include "misc/Compression.h"
#include "include/fcnc_endian.h"
#include "misc/INIFile.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "video/ImageNotFound.h"

using std::string;

CPSImage::CPSImage(const char* fname, int scaleq) :
	cpsdata(0), image(0) {
	VFile* imgfile;
	
	this->scaleq = scaleq;
	
	// Open the image file
	imgfile = VFSUtils::VFS_Open(fname);
	if (imgfile == NULL) {
		throw ImageNotFound("CPSImage: Image [" + string(fname) + "] not found.");
	}
	imgsize = imgfile->fileSize();
	image = NULL;
	cpsdata = new Uint8[imgsize];
	imgfile->readByte(cpsdata, imgsize);
	lnkHeader.size = cpsdata[0] + (cpsdata[0+1] << 8);
	lnkHeader.unknown = cpsdata[2] + (cpsdata[2+1] << 8);
	lnkHeader.imsize = cpsdata[4] + (cpsdata[4+1] << 8);
	lnkHeader.palette = cpsdata[6] + (cpsdata[6+1] << 8) + (cpsdata[6+2] << 16)
			+ (cpsdata[6+3] << 24);
	if (lnkHeader.palette == 0x3000000) {
		readPalette();
	} else {
		// magic here to select appropriate palette
		offset = 10;
	}
	VFSUtils::VFS_Close(imgfile);
}

CPSImage::~CPSImage() {
	delete[] cpsdata;
	SDL_FreeSurface(image);
}

SDL_Surface* CPSImage::getImage() {
	if (image == NULL) {
		loadImage();
	}
	return image;
}

void CPSImage::loadImage() {
	Uint32 len;
	Uint8* imgsrc;
	Uint8 *imgdst;
	SDL_Surface* imgtmp;

	len = imgsize-offset;
	imgsrc = new Uint8[len];
	imgdst = new Uint8[lnkHeader.imsize];
	memcpy(imgsrc, cpsdata + offset, len);
	memset(imgdst, 0, lnkHeader.imsize);
	Compression::decode80(imgsrc, imgdst);
	imgtmp = SDL_CreateRGBSurfaceFrom(imgdst, 320, 200, 8, 320, 0, 0, 0, 0);
	SDL_SetColors(imgtmp, palette, 0, 256);
	SDL_SetColorKey(imgtmp, SDL_SRCCOLORKEY, 0);
	delete[] imgsrc;
	delete[] cpsdata;
	cpsdata = NULL;
	if (scaleq >= 0) {
		image = lnkScaler.scale(imgtmp, scaleq);
		SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0);
	} else {
		image = SDL_DisplayFormat(imgtmp);
	}
	SDL_FreeSurface(imgtmp);
	delete[] imgdst;
}
void CPSImage::readPalette() {
	Uint16 i;

	offset = 10;
	for (i = 0; i < 256; i++) {
		palette[i].r = cpsdata[offset];
		palette[i].g = cpsdata[offset+1];
		palette[i].b = cpsdata[offset+2];
		palette[i].r <<= 2;
		palette[i].g <<= 2;
		palette[i].b <<= 2;
#if 0
		//        palette[i].r = (palette[i].r*255)/63;
		//        palette[i].g = (palette[i].g*255)/63;
		//        palette[i].b = (palette[i].b*255)/63;
		printf ("CPS ReadPalette r,g,b = %u,%u,%u\n", (unsigned char)palette[i].r, (unsigned char)palette[i].g, (unsigned char)palette[i].b);
#endif
		offset += 3;
	}
}
