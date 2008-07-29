// RaWindow.cpp
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

#include "RaWindow.h"

#include <math.h>

#include "RA_Label.h"
#include "Font.h"
#include "video/CPSImage.h"
#include "video/SHPImage.h"
#include "include/common.h"
#include "include/config.h"
#include "video/GraphicsEngine.h"
#include "video/ImageCache.h"
#include "include/sdllayer.h"
#include "RA_ProgressBar.h"
#include "video/ImageNotFound.h"

using std::string;

namespace pc {
	extern ConfigType Config;
	extern GraphicsEngine * gfxeng;
	extern ImageCache* imgcache;
}

RaWindow::RaWindow()
{
	WindowSurface	= 0; // NULL
	DisplaySurface	= pc::gfxeng->get_SDL_ScreenSurface();
	PalNr			= 0;

//	this->SetupWindow (Xpos, Ypos, Width, Heigth);
}

RaWindow::~RaWindow()
{
	SDL_FreeSurface(WindowSurface);
}

void RaWindow::setPalette(Uint8 pal)
{
	PalNr = pal;
}

void RaWindow::DrawRaBackground()
{
	SDL_Rect	dest;
	SDL_Surface	*Image;
	SDL_Surface* tmp;
    int topspacing = 0;
    int botspacing = 0;
    Uint32 EdgeOffset = 0;


	Uint32 BlackColor = SDL_MapRGB(DisplaySurface->format, 10, 10, 10);

	//
	// Draw the image background
	//
	dest.x = 0;
	dest.y = 0;
	dest.w = WindowSurface->w;
	dest.h = WindowSurface->h;
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-bkgnd.shp", 0, PalNr))!=NULL){
#else
	if ((Image = pc::imgcache->getImage( Background, 0 ).image) != NULL){
#endif
		tmp = SDL_DisplayFormat(Image);
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
		Image = tmp;



		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		if (WindowSurface->w > Image->w){
			dest.x = Image->w;
			dest.w = WindowSurface->w - Image->w;
			dest.h = WindowSurface->h;
			SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
			if (WindowSurface->h > Image->h){
				dest.y = Image->h;
				SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
			}
		}
		if (WindowSurface->h > Image->h){
			dest.x = 0;
			dest.y = Image->h;
			dest.w = WindowSurface->w;
			dest.h = WindowSurface->h;
			SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		}
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}else
		printf ("%s line %i: Failed to load image\n", __FILE__, __LINE__);

	//###################################################
	//
	// Draw the top edge
	//
	//###################################################

#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-top.shp", 0))!=NULL){
#else
	if ((Image = pc::imgcache->getImage( TopBorder, 0 ).image) != NULL){
#endif
		dest.x = 0;
		dest.y = 0;
		dest.w = WindowSurface->w;
		dest.h = Image->h;
		topspacing = Image->h;
		SDL_FillRect(WindowSurface, &dest, BlackColor);
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		if (WindowSurface->w > Image->w){
			dest.x = Image->w;
			dest.w = WindowSurface->w - Image->w;
			dest.h = Image->h;
			SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		}
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}

	//###################################################
	//
	// Draw the bottom window border
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-botm.shp", 0))!=NULL){
#else
	if ((Image = pc::imgcache->getImage( BottomBorder, 0 ).image) != NULL){
#endif
		dest.x = 0;
		dest.y = WindowSurface->h - Image->h;
		dest.w = WindowSurface->w;
		dest.h = Image->h;
		botspacing = Image->h;
		SDL_FillRect(WindowSurface, &dest, BlackColor);
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		if (WindowSurface->w > Image->w){
			dest.x = Image->w;
			dest.w = WindowSurface->w - Image->w;
			dest.h = Image->h;
			SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		}
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}

	//###################################################
	//
	// Draw the left edge
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-left.shp", 0))!=NULL){
#else
	if ((Image = pc::imgcache->getImage( LeftBorder, 0 ).image) != NULL){
#endif
		EdgeOffset = Image->w;
		dest.x = 0;
		dest.y = 0;
		dest.w = Image->w;
		dest.h = WindowSurface->h;
		SDL_FillRect(WindowSurface, &dest, BlackColor);
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		if (WindowSurface->w > Image->w){
			dest.x = 0;
			dest.y = Image->h;
			dest.w = Image->w;
			dest.h = WindowSurface->h - Image->h;
			SDL_FillRect(WindowSurface, &dest, BlackColor);
			SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		}
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}

	//###################################################
	//
	// Draw the rigth edge
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-right.shp", 0))!= NULL){
#else
	if ((Image = pc::imgcache->getImage( RightBorder, 0 ).image) != NULL){
#endif
		if (Image == NULL)
			printf ("NULL\n");
		dest.x = WindowSurface->w - Image->w;
		dest.y = 0;
		dest.w = Image->w;
		dest.h = WindowSurface->h;
		SDL_FillRect(WindowSurface, &dest, BlackColor);
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		if (WindowSurface->w > Image->w){
			dest.x = WindowSurface->w - Image->w;
			dest.y = Image->h;
			dest.w = Image->w;
			dest.h = WindowSurface->h - Image->h;
			SDL_FillRect(WindowSurface, &dest, BlackColor);
			SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
		}
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}
	//###################################################
	//
	// Draw the left/top corner
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-crnr.shp", 0)) != NULL){
#else
	if ((Image = pc::imgcache->getImage( Corner, 0 ).image) != NULL){
#endif
		dest.x = 0;
		dest.y = 0;
		dest.w = Image->w;
		dest.h = Image->h;
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}

	//###################################################
	//
	// Draw the rigth/top corner
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-crnr.shp", 1)) != NULL){
#else
	if ((Image = pc::imgcache->getImage( Corner, 1 ).image) != NULL){
#endif
		dest.x = WindowSurface->w - Image->w;
		dest.y = 0;
		dest.w = Image->w;
		dest.h = Image->h;
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}

	//###################################################
	//
	// Draw the left/bottum corner
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-crnr.shp", 2)) != NULL){
#else
	if ((Image = pc::imgcache->getImage( Corner, 2 ).image) != NULL){
#endif
		dest.x = 0;
		dest.y = WindowSurface->h - Image->h;
		dest.w = Image->w;
		dest.h = Image->h;
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}

	//###################################################
	//
	// Draw the rigth/bottum corner
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-crnr.shp", 3)) != NULL){
#else
	if ((Image = pc::imgcache->getImage( Corner, 3 ).image) != NULL){
#endif
		dest.x = WindowSurface->w - Image->w;
		dest.y = WindowSurface->h - Image->h;
		dest.w = Image->w;
		dest.h = Image->h;
		SDL_BlitSurface(Image, NULL, WindowSurface, &dest);
#ifndef USE_IMAGE_POOL
		SDL_FreeSurface(Image);
#endif
	}


SDL_Rect	SrcRect;
	//###################################################
	//
	// Draw the left edge decoration
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-edge.shp", 0, PalNr))!=NULL){
#else
	if ((Image = pc::imgcache->getImage( Edge, 3 ).image) != NULL){
#endif
		for (int i = topspacing; i < (WindowSurface->h - botspacing); i+=Image->h){
			dest.x = EdgeOffset;
			dest.y = i;
			dest.w = Image->w;
			dest.h = Image->h;
			//memcpy (&SrcRect, &dest, sizeof(SDL_Rect));

			SrcRect.x = 0;
			SrcRect.y = 0;
			SrcRect.w = Image->w;
			SrcRect.h = Image->h;
			if (dest.y + Image->h > WindowSurface->h - botspacing){
				SrcRect.h = WindowSurface->h - dest.y - botspacing;
			}

			SDL_BlitSurface(Image,&SrcRect, WindowSurface, &dest);
		}
		SDL_FreeSurface(Image);
	}

	//###################################################
	//
	// Draw the rigth edge decoration
	//
	//###################################################
#ifndef USE_IMAGE_POOL
	if ((Image = this->ReadShpImage ((char*)"dd-edge.shp", 1, PalNr))!=NULL){
#else
	if ((Image = pc::imgcache->getImage( Edge, 3 ).image) != NULL){
#endif
		for (int i = topspacing; i < (WindowSurface->h - botspacing); i+=Image->h){
			dest.x = WindowSurface->w - EdgeOffset - Image->w;
			dest.y = i;
			dest.w = Image->w;
			dest.h = Image->h;

			SrcRect.x = 0;
			SrcRect.y = 0;
			SrcRect.w = Image->w;
			SrcRect.h = Image->h;
			if (dest.y + Image->h > WindowSurface->h - botspacing){
				SrcRect.h = WindowSurface->h - dest.y - botspacing;
			}

			SDL_BlitSurface(Image, &SrcRect, WindowSurface, &dest);
		}
		SDL_FreeSurface(Image);
	}


}
	
void RaWindow::SolidFill (Uint8 red, Uint8 green, Uint8 blue)
{

	Uint32 Color = SDL_MapRGB(WindowSurface->format, red, green, blue);
	// Fill the surface with the correct color
	SDL_FillRect(WindowSurface, &SizeAndPosition, Color);
}

void RaWindow::ResizeWindow (int Width, int Heigth)
{
	SDL_Surface * tmp;


	SizeAndPosition.w = Width;
	SizeAndPosition.h = Heigth;

	SDL_FreeSurface(WindowSurface);

	WindowSurface = NULL; // not really needed here ;)

	WindowSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, Width, Heigth, 16, 0, 0, 0, 0);

	tmp = SDL_DisplayFormat(WindowSurface);
	SDL_FreeSurface(WindowSurface);
	WindowSurface = tmp;

	tmp = SDL_DisplayFormat(WindowSurface);
	SDL_FreeSurface(WindowSurface);
	WindowSurface = tmp;

	if (pc::Config.gamenum == GAME_RA){
		this->DrawRaBackground();
	}
}

void RaWindow::GetWindowPosition (int *Xpos, int *Ypos)
{
	*Xpos = SizeAndPosition.x;
	*Ypos = SizeAndPosition.y;
}

void RaWindow::ChangeWindowPosition (int Xpos, int Ypos)
{
	SizeAndPosition.x = Xpos;
	SizeAndPosition.y = Ypos;
}

SDL_Surface* RaWindow::GetWindowSurface (void)
{
	return WindowSurface;
}

void RaWindow::SetupWindow (int Xpos, int Ypos, int Width, int Heigth)
{
	SizeAndPosition.x = Xpos;
	SizeAndPosition.y = Ypos;
	SizeAndPosition.w = Width;
	SizeAndPosition.h = Heigth;

	SDL_FreeSurface(WindowSurface);

	WindowSurface = NULL; // not really needed here ;)

	WindowSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, Width, Heigth, 16, 0, 0, 0, 0);

	if (pc::Config.gamenum == GAME_RA){
/**
 * 		The image cache somehow gets corrupted by this --> the imagecache is destroyed and recreated in maps.cpp
*/

		int scaleq = -1;

		// Load window images
        	try {
			Background	= pc::imgcache->loadImage("dd-bkgnd.shp", scaleq);
		}catch (ImageNotFound&){
			printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		}

        	try {
			LeftBorder	= pc::imgcache->loadImage("dd-left.shp", scaleq);
		}catch (ImageNotFound&){
			printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		}

        	try {
			RightBorder	= pc::imgcache->loadImage("dd-right.shp", scaleq);
		}catch (ImageNotFound&){
			printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		}

        	try {
			TopBorder	= pc::imgcache->loadImage("dd-top.shp", scaleq);
		}catch (ImageNotFound&){
			printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		}

        	try {
			BottomBorder	= pc::imgcache->loadImage("dd-botm.shp", scaleq);
		}catch (ImageNotFound&){
			printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		}

        	try {
			Corner		= pc::imgcache->loadImage("dd-crnr.shp", scaleq);
		}catch (ImageNotFound&){
			printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		}

		this->DrawRaBackground ();
	}
}

void RaWindow::DrawWindow()
{
/*
SDL_Rect dest;

	dest.x = 0;
	dest.y = 0;
	dest.w = WindowSurface->w;
	dest.h = WindowSurface->h;
*/
	SDL_BlitSurface(WindowSurface, NULL, DisplaySurface, &SizeAndPosition);

}

SDL_Surface* RaWindow::ReadShpImage (char *Name, int ImageNumb, Uint8 palnum)
{
	SDL_Surface* image;
	SDL_Surface* shadow;
	SHPImage* TempPic;

	try {
		TempPic = new SHPImage(Name, -1);
	} catch (ImageNotFound&) {
		printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		return NULL;
	}

//	printf ("%s line %i: %s has %i images\n", __FILE__, __LINE__, Name, TempPic->getNumImg());

	TempPic->getImage(ImageNumb, &image, &shadow, palnum /*1*/);

	delete TempPic;

	SDL_FreeSurface(shadow);

	return image;
}
