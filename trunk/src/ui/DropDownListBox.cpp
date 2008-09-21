// DropDownListBox.cpp
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

#include "DropDownListBox.h"

#include <math.h>

#include "SDL/SDL_video.h"

#include "RA_Label.h"
#include "Font.h"
#include "video/CPSImage.h"
#include "video/SHPImage.h"
#include "include/config.h"
#include "video/GraphicsEngine.h"
#include "video/ImageCache.h"
#include "include/sdllayer.h"
#include "RA_ProgressBar.h"
#include "RaWindow.h"
#include "TTextBox.h"
#include "video/ImageNotFound.h"

using std::string;

namespace pc {
	extern ImageCache* imgcache;
	extern GraphicsEngine * gfxeng;
}


DropDownListBox::DropDownListBox()
{
	List.empty();

	ListBoxSurface	= 0;
	DisplaySurface	= 0;
	WindowToDrawOn	= 0;
	ArrowDownImage	= 0;

	SizeAndPosition.x = 0;
	SizeAndPosition.y = 0;
	SizeAndPosition.w = 100;
	SizeAndPosition.h = 16;

	SelectedIndex	= 0;
	Selected		= false;
	Recreate		= true;
	button_down		= false;

	// Initialize the surfaces
	DisplaySurface	= pc::gfxeng->get_SDL_ScreenSurface();

	// Initialize the checkbox color
	ListBoxColor			= SDL_MapRGB(DisplaySurface->format, 0xff, 0, 0);
	ListBoxBackgroundColor	= SDL_MapRGB(DisplaySurface->format, 0, 0, 0);

	TextColor.r	=	0xff;
	TextColor.g	=	0;
	TextColor.b	=	0;

	// Load window images
	try {
		ArrowDwn = pc::imgcache->loadImage("btn-dn.shp", 1);
	}catch (ImageNotFound&){
		printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
	}
}

DropDownListBox::~DropDownListBox()
{
	if (ListBoxSurface != NULL){
		SDL_FreeSurface( ListBoxSurface );
	}

#ifndef USE_IMAGE_POOL
	if (ArrowDownImage != NULL){
		SDL_FreeSurface( ArrowDownImage );
	}
#endif
}

bool DropDownListBox::need_redraw(void)
{
	return Recreate;
}

/**
 * Set the surface (window) to draw the button on
 */
void DropDownListBox::SetDrawingWindow(RaWindow* window)
{
	if (window != 0){
		this->WindowToDrawOn = window;
	}
}

/**
 */
void DropDownListBox::Create()
{
	SDL_Rect	dest;
	Uint16		Ypos;
	//char		*temp;


	Recreate	= false;

	if (ListBoxSurface != NULL){
		//printf ("Free surface\n");
		SDL_FreeSurface(ListBoxSurface);
	}

	Spacing = (SizeAndPosition.h - ListBoxLabel.getHeight())/2;


	if (Selected){
		ListBoxSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, SizeAndPosition.w, (SizeAndPosition.h + Spacing + (SizeAndPosition.h - Spacing)* List.size() ), 16, 0, 0, 0, 0);
	}else{
		ListBoxSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, SizeAndPosition.w, SizeAndPosition.h, 16, 0, 0, 0, 0);
	}

	SDL_Surface *tmp = SDL_DisplayFormat(ListBoxSurface);
	SDL_FreeSurface(ListBoxSurface);
	ListBoxSurface = tmp;

	// this is the destinatc_strion as needed for the new surface
	dest.x = 0;
	dest.y = 0;
	dest.w = ListBoxSurface->w;
	dest.h = ListBoxSurface->h;

	// Fill the surface with the correct color
	SDL_FillRect(ListBoxSurface, &dest, ListBoxColor);

	// this is the destination as needed for the new surface
	dest.x = 1;
	dest.y = 1;
	dest.w = ListBoxSurface->w-2;
	dest.h = SizeAndPosition.h-2;

	// Fill the surface with the correct color
	SDL_FillRect(ListBoxSurface, &dest, ListBoxBackgroundColor);

	if (Selected){
		// this is the destination as needed for the new surface
		dest.x = 1;
		dest.y = SizeAndPosition.h;
		dest.w = ListBoxSurface->w-2;
		dest.h = ListBoxSurface->h - 2 -SizeAndPosition.h;

		// Fill the surface with the correct color
		SDL_FillRect(ListBoxSurface, &dest, ListBoxBackgroundColor);
	}

	if ((unsigned) SizeAndPosition.h < (unsigned) ListBoxLabel.getHeight())
		return;

	if (SelectedIndex < List.size()){
		if ((unsigned)  SizeAndPosition.w-8 < (unsigned) ListBoxLabel.getWidth(List[SelectedIndex])){
			return;
        }
	}

	if (Selected)
		Ypos = SizeAndPosition.h + (SizeAndPosition.h - ListBoxLabel.getHeight())/2;
	else
		Ypos = (SizeAndPosition.h - ListBoxLabel.getHeight())/2;

	if (SelectedIndex < List.size()){
		if (Selected){
			for (Uint32 i = 0; i< List.size(); i++){
				ListBoxLabel.Draw(List[i], ListBoxSurface, TextColor, 4, Ypos); //drawText("Your color", ListBox.geDrawingSurface(), 100, 20);
				Ypos += ListBoxLabel.getHeight()+Spacing;
			}
		}else{
			ListBoxLabel.Draw(List[SelectedIndex], ListBoxSurface, TextColor, 4, Ypos); //drawText("Your color", ListBox.geDrawingSurface(), 100, 20);
		}
	}
#ifndef USE_IMAGE_POOL
	if (ArrowDownImage != NULL)
		SDL_FreeSurface(ArrowDownImage);
#endif


if (!button_down){
#ifndef USE_IMAGE_POOL
	if ((ArrowDownImage = this->ReadShpImage ((char*)"btn-dn.shp", 0, 2)) == NULL){
#else
	if ((ArrowDownImage = pc::imgcache->getImage( ArrowDwn, 0 ).image) == NULL){
#endif
		printf ("%s line %i: Failed to load button down image\n", __FILE__, __LINE__);
	}
}else{
#ifndef USE_IMAGE_POOL
	if ((ArrowDownImage = this->ReadShpImage ((char*)"btn-dn.shp", 1, 2)) == NULL){
#else
	if ((ArrowDownImage = pc::imgcache->getImage( ArrowDwn, 1 ).image) == NULL){
#endif
		printf ("%s line %i: Failed to load button down image\n", __FILE__, __LINE__);
	}
}

if (ArrowDownImage != NULL){
	Uint8 ColRed;
	SDL_Color color;
	for (Uint32 x = 0; x < (Uint32) ArrowDownImage->w; x++){
		for (Uint32 y = 0; y < (Uint32) ArrowDownImage->h; y++){
			SDLLayer::get_pixel ( ArrowDownImage, color, x, y );
			ColRed = color.r;
			color.r = (color.b+10)&0xff;
			if (color.g > 50){
				color.g = 50;
			}
			color.b = ColRed-10;
			SDLLayer::set_pixel(ArrowDownImage, color, x, y );
//		printf ("Col = %x\n", Temp[i]);
//		Temp[i] += 50;
		}
	}
}

// Fill the surface with the correct color
//ArrowDownImage = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, SizeAndPosition.w, SizeAndPosition.h, 16, 0, 0, 0, 0);
}

/**
 * Set the surface (window) to draw the button on
 */
void DropDownListBox::AddEntry(string Entry)
{
	List.push_back(Entry);
}

bool DropDownListBox::MouseOver()
{
	int mx, my;
	int WinXpos = 0;
	int WinYpos = 0;

	SDL_GetMouseState(&mx, &my);

	if (WindowToDrawOn != NULL){
		WindowToDrawOn->GetWindowPosition (&WinXpos, &WinYpos);
		mx -= WinXpos;
		my -= WinYpos;
	}

	Spacing = (SizeAndPosition.h - ListBoxLabel.getHeight())/2;

	if (mx > SizeAndPosition.x && mx < SizeAndPosition.x + SizeAndPosition.w){
		if (my > SizeAndPosition.y && my < (SizeAndPosition.y + (SizeAndPosition.h + Spacing + (SizeAndPosition.h - Spacing) * (signed)List.size())) && Selected){
			return true;
        }
		if (my > SizeAndPosition.y && my < (SizeAndPosition.y + SizeAndPosition.h) && !Selected){
			return true;
        }
	}
	return false;
}

bool DropDownListBox::MouseOver_button()
{
    int mx = 0;
    int my = 0;
    int WinXpos = 0;
    int WinYpos = 0;

	if (ArrowDownImage == NULL){
		return false;
    }

	SDL_GetMouseState(&mx, &my);

	if (WindowToDrawOn != NULL){
		WindowToDrawOn->GetWindowPosition (&WinXpos, &WinYpos);
		mx -= WinXpos;
		my -= WinYpos;
	}

	if (mx > SizeAndPosition.x + SizeAndPosition.w - 1 && mx < SizeAndPosition.x + SizeAndPosition.w + ArrowDownImage->w - 1){
		if (my > SizeAndPosition.y && my < (SizeAndPosition.y + SizeAndPosition.h)){
			return true;
        }
    }
	return false;
}

Uint32 DropDownListBox::MouseOver_entry()
{
	int mx, my;
	int WinXpos = 0;
	int WinYpos = 0;

	SDL_GetMouseState(&mx, &my);

	if (WindowToDrawOn != NULL){
		WindowToDrawOn->GetWindowPosition (&WinXpos, &WinYpos);
		mx -= WinXpos;
		my -= WinYpos;
	}

	if (!Selected){
		return 0;
    }

	Spacing = (SizeAndPosition.h - ListBoxLabel.getHeight())/2;

	if (mx > SizeAndPosition.x && mx < SizeAndPosition.x + SizeAndPosition.w){
		if (my > (SizeAndPosition.y + SizeAndPosition.h) && my < (SizeAndPosition.y + (SizeAndPosition.h + Spacing + (SizeAndPosition.h - Spacing) * (signed)List.size()))){
			return  List.size() - (unsigned)(((SizeAndPosition.y + (SizeAndPosition.h + Spacing + (SizeAndPosition.h - Spacing) * List.size())) - my)/SizeAndPosition.h) - 1;
		}
	}
	return 0;
}

/**
 * @return Selected index in the DropDownListBox
 */
unsigned int DropDownListBox::getSelected() const
{
	return SelectedIndex;
}

void DropDownListBox::HandleInput(SDL_Event event)
{
    //SDL_Event event;
    int mx, my;
    int WinXpos = 0, WinYpos = 0;

	SDL_GetMouseState(&mx, &my);

	if (WindowToDrawOn != NULL){
		WindowToDrawOn->GetWindowPosition(&WinXpos, &WinYpos);
		mx -= WinXpos;
		my -= WinYpos;
	}

	switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			//printf ("%s line %i: Entry = %i\n", __FILE__, __LINE__, mouseover_entry());

			if (MouseOver_button()){
				button_down = true;
            }
			if (MouseOver() || MouseOver_button()){
				if (Selected){
					SelectedIndex = MouseOver_entry();
					Selected = false;
				}else{
					Selected = true;
				}
			}else{
				Selected = false;
			}
			Recreate = true;
			break;

		case SDL_MOUSEBUTTONUP:
			if (button_down){
				button_down = false;
				Recreate = true;
			}
			break;

		case SDL_KEYDOWN:
			if (!Selected){
				return;
            }
			switch (event.key.keysym.sym){
				case SDLK_BACKSPACE:
				case SDLK_TAB:
				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					break;
				default:
					break;
			}
			break;
	}
}

bool DropDownListBox::Draw(int X, int Y)
{
    Uint32 Heigth = SizeAndPosition.h;
    SDL_Rect	dest;

	// Set size and posisiton
	SizeAndPosition.x = X;
	SizeAndPosition.y = Y;

	if (this->Recreate){
		Create();
    }

	if (ListBoxSurface == NULL){
		return false;
    }

	if (DisplaySurface == NULL){
		return false;
    }

	if (WindowToDrawOn != NULL){
		SDL_BlitSurface(ListBoxSurface, NULL, WindowToDrawOn->GetWindowSurface (), &SizeAndPosition);
    } else {
		SDL_BlitSurface(ListBoxSurface, NULL, DisplaySurface, &SizeAndPosition);
    }

	if ( ArrowDownImage != NULL ){

		dest.x = SizeAndPosition.x + SizeAndPosition.w - 1;
		dest.y = SizeAndPosition.y;
		dest.w = ArrowDownImage->w;
		dest.h = ArrowDownImage->h;

		if (WindowToDrawOn != NULL){
			SDL_BlitSurface(ArrowDownImage, NULL, WindowToDrawOn->GetWindowSurface (), &dest);
		}else{
			SDL_BlitSurface(ArrowDownImage, NULL, DisplaySurface, &dest);
		}
	}

	SizeAndPosition.h = Heigth;

	return true;
}

SDL_Surface *DropDownListBox::ReadShpImage (char *Name, int ImageNumb, Uint8 palnum)
{
	SDL_Surface	*image;
	SDL_Surface *shadow;
	SHPImage	*TempPic;

	try {
		TempPic = new SHPImage(Name, -1);
	} catch (ImageNotFound&) {
		printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		return NULL;
	}

	TempPic->getImage(ImageNumb, &image, &shadow, palnum);

	delete TempPic;

	SDL_FreeSurface(shadow);

	SDL_SetColorKey(image,SDL_SRCCOLORKEY, 0xffffff);

	return image;
}
