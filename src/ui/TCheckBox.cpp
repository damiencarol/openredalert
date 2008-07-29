// TCheckBox.cpp
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

#include "TCheckBox.h"

#include "SDL/SDL_video.h"

#include "video/GraphicsEngine.h"

namespace pc
{
extern GraphicsEngine * gfxeng;
}

TCheckBox::~TCheckBox()
{
	SDL_FreeSurface(CheckBoxSurface);
}

void TCheckBox::Draw(int X, int Y)
{
	SDL_Rect dest;

	if (CheckBoxSurface == 0)
	{
		return;
	}
	if (DisplaySurface == 0)
	{
		return;
	}

	dest.x = X;
	dest.y = Y;
	dest.w = Width;
	dest.h = Heigth;

	SDL_BlitSurface(CheckBoxSurface, NULL, DisplaySurface, &dest);
}

void TCheckBox::Create()
{
	SDL_Rect dest;

	if (CheckBoxSurface != 0)
	{
		SDL_FreeSurface(CheckBoxSurface);
	}

	CheckBoxSurface
			= SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, Width, Heigth, 16, 0, 0, 0, 0);

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

TCheckBox::TCheckBox()
{
	// Setup some vars
	Checked = false;
	Width = 20, Heigth = 20;
	CheckBoxSurface = 0;

	// Initialize the surfaces
	DisplaySurface = pc::gfxeng->get_SDL_ScreenSurface();

	// Initialize the checkbox color
	CheckboxColor = SDL_MapRGB(DisplaySurface->format, 0xff, 0, 0);
	CheckboxBackgroundColor = SDL_MapRGB(DisplaySurface->format, 0, 0, 0);

	this->Create();
}
