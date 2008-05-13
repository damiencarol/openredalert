#include "TCheckBox.h"

#include <math.h>

#include "SDL/SDL_video.h"
#include "video/GraphicsEngine.h"
#include "RA_ProgressBar.h"
#include "RA_WindowClass.h"
#include "TTextBox.h"

using std::string;

namespace pc {
	extern GraphicsEngine * gfxeng;
}

TCheckBox::~TCheckBox(){
	SDL_FreeSurface(CheckBoxSurface);
}

void TCheckBox::Draw(int X, int Y)
{
	SDL_Rect dest;

	if (CheckBoxSurface == NULL){
		return;
	}
	if (DisplaySurface == NULL){
		return;
	}

	dest.x = X;
	dest.y = Y;
	dest.w = Width;
	dest.h = Heigth;

	SDL_BlitSurface(CheckBoxSurface, NULL, DisplaySurface, &dest);
}

void TCheckBox::Create (void)
{
	SDL_Rect dest;

	if (CheckBoxSurface != NULL){
		SDL_FreeSurface(CheckBoxSurface);
	}

	CheckBoxSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, Width, Heigth, 16, 0, 0, 0, 0);

	// this is the destination as needed for the new surface
	dest.x = 0;
	dest.y = 0;
	dest.w = Width;
	dest.h = Heigth;

	// Fill the surface with the correct color
	SDL_FillRect(CheckBoxSurface, &dest, CheckboxColor);

	// this is the destination as needed for the new surface
	dest.x = 1;
	dest.y = 1;
	dest.w = Width-2;
	dest.h = Heigth-2;

	// Fill the surface with the correct color
	SDL_FillRect(CheckBoxSurface, &dest, CheckboxBackgroundColor);
}

TCheckBox::TCheckBox(){
	// Setup some vars
	Checked		= false;
	Width		= 20,
	Heigth		= 20;
	CheckBoxSurface = NULL;

	// Initialize the surfaces
	DisplaySurface	= pc::gfxeng->get_SDL_ScreenSurface();

	// Initialize the checkbox color
	CheckboxColor		= SDL_MapRGB(DisplaySurface->format, 0xff, 0, 0);
	CheckboxBackgroundColor	= SDL_MapRGB(DisplaySurface->format, 0, 0, 0);

	this->Create ();
}
