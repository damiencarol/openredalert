// PCXImage.cpp
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

#include "PCXImage.h"

#include <cstdlib>
#include <cstring>

#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "misc/INIFile.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "include/Logger.h"
#include "video/ImageNotFound.h"

extern Logger * logger;

PCXImage::PCXImage(const char* fname, int scaleq) : pcxdata(0), image(0) 
{
	image			= NULL;
	HeaderError		= false;
	Use16ColorPalette	= true;

	VFile* imgfile;
	this->scaleq = scaleq;
	imgfile = VFSUtils::VFS_Open(fname);
	if (imgfile == NULL) {
		logger->error ("%s line %i: Image not found %s\n", __FILE__, __LINE__, fname);
		throw ImageNotFound("Image [" + string(fname) + "] not found");
	}
	imgsize = imgfile->fileSize();

	// The image should at least be bigger that the header
	if (imgsize  < 129){
		VFSUtils::VFS_Close(imgfile);
		logger->error ("%s line %i: Not an pcx image %s\n", __FILE__, __LINE__, fname);
		HeaderError = true;
		return;
	}

	pcxdata = new Uint8[imgsize];
	imgfile->readByte(pcxdata, imgsize);

	header.Signature	= pcxdata[0];
	header.Version		= pcxdata[1];
	header.Encoding		= pcxdata[2];
	header.BitsPixel	= pcxdata[3];
	header.XMin			= pcxdata[4] + (pcxdata[4+1] << 8);
	header.YMin			= pcxdata[6] + (pcxdata[6+1] << 8);
	header.XMax			= pcxdata[8] + (pcxdata[8+1] << 8);
	header.YMax			= pcxdata[10] + (pcxdata[10+1] << 8);
	header.HRes			= pcxdata[12] + (pcxdata[12+1] << 8);
	header.VRes			= pcxdata[14] + (pcxdata[14+1] << 8);

	// The image should at least be bigger that the header
	if (header.Signature !=  10){
		VFSUtils::VFS_Close(imgfile);
		logger->error ("%s line %i: Not an pcx image %s\n", __FILE__, __LINE__, fname);
		HeaderError = true;
		return;
	}

	// Read the palette
	Uint32 offset = 16;
	for (int i = 0; i < 16; i++){
		header.palette[i].r = pcxdata[offset];
		header.palette[i].g = pcxdata[offset+1];
		header.palette[i].b = pcxdata[offset+2];
		offset += 3;
	}

	header.Reserved		= pcxdata[64];
	header.NumPlanes	= pcxdata[65];
	header.BytesLine	= pcxdata[66] + (pcxdata[66+1] << 8);
	header.PalType		= pcxdata[68] + (pcxdata[68+1] << 8);
	header.HscreenSize	= pcxdata[70] + (pcxdata[70+1] << 8);
	header.VscreenSize	= pcxdata[72] + (pcxdata[72+1] << 8);

#ifdef DEBUG_PCX
	Uint32 Height	= header.YMax-header.YMin+1;
	Uint32 Width	= header.BytesLine*header.NumPlanes; //Width:= XMax-XMin+1)
	logger->note ("%s line %i: PRINTING pcx header for file: %s\n", __FILE__, __LINE__, fname);
	logger->note ("%s line %i: Signature \t\t= %i\n", __FILE__, __LINE__, header.Signature);
	logger->note ("%s line %i: Version \t\t= %i\n", __FILE__, __LINE__, header.Version);
	logger->note ("%s line %i: Encoding \t\t= %i\n", __FILE__, __LINE__, header.Encoding);
	logger->note ("%s line %i: BitsPixel \t\t= %i\n", __FILE__, __LINE__, header.BitsPixel);
	logger->note ("%s line %i: XMin \t\t\t= %i\n", __FILE__, __LINE__, header.XMin);
	logger->note ("%s line %i: YMin \t\t\t= %i\n", __FILE__, __LINE__, header.YMin);
	logger->note ("%s line %i: XMax \t\t\t= %i\n", __FILE__, __LINE__, header.XMax);
	logger->note ("%s line %i: YMax \t\t\t= %i\n", __FILE__, __LINE__, header.YMax);
	logger->note ("%s line %i: HRes \t\t\t= %i\n", __FILE__, __LINE__, header.HRes);
	logger->note ("%s line %i: VRes \t\t\t= %i\n", __FILE__, __LINE__, header.VRes);
	logger->note ("%s line %i: NumPlanes \t\t= %i\n", __FILE__, __LINE__, header.NumPlanes);
	logger->note ("%s line %i: BytesLine \t\t= %i\n", __FILE__, __LINE__, header.BytesLine);
	logger->note ("%s line %i: PalType \t\t= %i\n", __FILE__, __LINE__, header.PalType);
	logger->note ("%s line %i: HscreenSize \t= %i\n", __FILE__, __LINE__, header.HscreenSize);
	logger->note ("%s line %i: VscreenSize \t= %i\n", __FILE__, __LINE__, header.VscreenSize);

	logger->note ("%s line %i: Image width \t= %i\n", __FILE__, __LINE__, Width);
	logger->note ("%s line %i: Image height \t= %i\n", __FILE__, __LINE__, Height);
	logger->note ("%s line %i: Image size \t\t= %i\n", __FILE__, __LINE__, Height*Width);
#endif

	// Read the vga pallette if present
	readPalette();

	/// Close the VFile
	VFSUtils::VFS_Close(imgfile);
}

PCXImage::~PCXImage()
{
	if (pcxdata != NULL){
		delete[] pcxdata;
	}
	if (image != NULL){
		SDL_FreeSurface(image);
	}
}

SDL_Surface* PCXImage::getImage()
{
	if (HeaderError)
		return 0;

    if (image == 0) {
        loadImage();
    }
    return image;
}

void PCXImage::loadImage()
{
unsigned char Pixel;
unsigned char Number;
unsigned int y, x;
long Index;
unsigned int offset;
SDL_Surface* imgtmp;
Uint8 *imgdst;


	Uint32 Height	= header.YMax-header.YMin+1;
	Uint32 Width	= header.BytesLine*header.NumPlanes; //header.XMax-header.XMin+1;

	// Image data var ( + Width to be absolutely sure ;) )
	imgdst = new Uint8[Height*Width+Width];

	/* RLE (Run Length Encoding) */
	offset = 128;
	Index = 0;
	y = 0;
	while( y < Height ) {
		x = 0;
		while( x < Width ){
			Pixel = pcxdata[offset];
			offset++;
			if( Pixel > 192 ){
				Number = Pixel-192;
				Pixel = pcxdata[offset];
				offset++;
				for( unsigned int i=0; i<Number; i++ ){
					imgdst[Index++] = Pixel;
					x++;
				}
			}else{
				imgdst[Index++] = Pixel;
				x++;
			}
		}
		y++;
	}

	//imgtmp = SDL_CreateRGBSurfaceFrom( imgdst, header.HRes, header.VRes, header.BitsPixel, header.BytesLine, 0, 0, 0, 0 );
	imgtmp = SDL_CreateRGBSurfaceFrom( imgdst, header.XMax-header.YMin, header.YMax-header.XMin, header.BitsPixel, header.BytesLine, 0, 0, 0, 0 );
	if (!Use16ColorPalette)
		SDL_SetColors(imgtmp,palette,0,256);
	else
		SDL_SetColors(imgtmp,header.palette,0,16);

	SDL_SetColorKey(imgtmp,SDL_SRCCOLORKEY,0);

	if (scaleq >= 0) {
		image = scaler.scale(imgtmp,scaleq);
		SDL_SetColorKey(image,SDL_SRCCOLORKEY,0);
	} else {
		image = SDL_DisplayFormat(imgtmp);
	}
	SDL_FreeSurface(imgtmp);
	delete[] imgdst;
}
void PCXImage::readPalette()
{
    Uint16 i;

	Uint32 offset = imgsize - 769;

#ifdef DEBUG_PCX
	logger->note ("%s line %i: Vga palette indicator = %i (should be 12 if it contains a 256 color palette)\n", __FILE__, __LINE__, readbyte(pcxdata, offset));
#endif

	// This is the vga palette indicator (must be 12 if there is a palette)
	if (pcxdata[offset] == 12){
		Use16ColorPalette = false;
		offset++;
		for (i = 0; i < 256; i++) {
			palette[i].r = pcxdata[offset];
			palette[i].g = pcxdata[offset+1];
			palette[i].b = pcxdata[offset+2];
			offset += 3;
		}
	}
}
