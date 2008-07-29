// RA_Label.cpp
// 1.5

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

#include "RA_Label.h"

#include <string>

#include "video/GraphicsEngine.h"
#include "Font.h"
#include "RaWindow.h"

using std::string;

namespace pc {
	extern GraphicsEngine * gfxeng;
}

RA_Label::RA_Label() :
	LabelFont("type.fnt")
{
	// Setup some vars
	Checked			= false;
	Width			= 20,
	Heigth			= 20;
	LabelSurface	= 0;
	LabelText 		= "Uninitialize";
	DrawingSurface		= 0;
	DrawingWindow		= 0;
	BackgroundBackup	= 0;

	// Initialize the display surface pointer
//	SetDrawingSurface (pc::gfxeng->get_SDL_ScreenSurface());

	// Initialize the checkbox color
	LabelFontColor.r = 0;
	LabelFontColor.g = 0;
	LabelFontColor.b = 0xff;
	ColorKeyColor.r = ColorKeyColor.g = ColorKeyColor.b = 0;

	LabelDest.x = 0;
	LabelDest.y = 0;
}

/**
 * 
 */
RA_Label::~RA_Label()
{
	if (LabelSurface != 0)
	{
		SDL_FreeSurface(LabelSurface);
	}
	
	if (BackgroundBackup != 0)
	{
		SDL_FreeSurface(BackgroundBackup);
	}

	LabelSurface = 0;
}

Uint32 RA_Label::getHeight()
{
	return LabelFont.getHeight();
}

Uint32 RA_Label::getWidth()
{
	return LabelFont.calcTextWidth(LabelText);
}

Uint32 RA_Label::getWidth(const string text){
	return LabelFont.calcTextWidth(text);
}

string RA_Label::getText(){
	return LabelText;
}

void RA_Label::setColor(SDL_Color RGBcolor)
{
	if (BackgroundBackup != 0 && DrawingWindow != 0)
	{
		SDL_BlitSurface(BackgroundBackup, NULL, DrawingWindow->GetWindowSurface (), &LabelDest);
		SDL_FreeSurface(BackgroundBackup);
		BackgroundBackup = NULL;
	}

	if ( RGBcolor.r != LabelFontColor.r || RGBcolor.g != LabelFontColor.g || RGBcolor.b != LabelFontColor.b ){
		memcpy (&LabelFontColor, &RGBcolor, sizeof (SDL_Color));
		recreate = true;
	}
}

void RA_Label::setColor(Uint32 color)
{
	SDL_Color RGBcolor;

	if (LabelSurface != NULL){
		SDL_GetRGB(color, pc::gfxeng->get_SDL_ScreenSurface()->format, &RGBcolor.r, &RGBcolor.g, &RGBcolor.b );
		setColor(RGBcolor);
	}
}

void RA_Label::setColor (Uint8 r, Uint8 g, Uint8 b)
{
	SDL_Color RGBcolor;

	RGBcolor.r = r;
	RGBcolor.g = g;
	RGBcolor.b = b;

	setColor(RGBcolor);
}

void RA_Label::SetDrawingSurface (SDL_Surface *DwgSurface)
{
	if (DrawingSurface != DwgSurface){
		DrawingWindow = NULL;
		DrawingSurface = DwgSurface;
		recreate = true;
	}
}

void RA_Label::SetDrawingWindow(RaWindow* window)
{
	if (DrawingWindow != window)
	{
		DrawingWindow = window;
		DrawingSurface = 0;
		recreate = true;
	}
}

void RA_Label::UseAntiAliasing(bool status)
{
	LabelFont.UseAntiAliasing(status);
	recreate = true;
}

void RA_Label::underline(bool status)
{
	LabelFont.underline(status);
	recreate = true;
}

void RA_Label::Draw(int X, int Y)
{
	SDL_Rect BackupDest;

	// Recreate the text if we draw it somewhere else
	if (LabelDest.x != X || LabelDest.y != Y)
		recreate = true;

	// Init LabelDest
	LabelDest.x = X;
	LabelDest.y = Y;
	LabelDest.w = Width;
	LabelDest.h = Heigth;

	// Recreate the Label surface if needed
	if (recreate)
		Create();

	if (LabelSurface == 0)
		return;

	if (DrawingSurface == 0 && DrawingWindow == 0)
		return;

	if (DrawingSurface != NULL){
		SDL_BlitSurface(LabelSurface, NULL, DrawingSurface, &LabelDest);
	}
	else if (DrawingWindow != NULL)
	{
		if (BackgroundBackup == NULL){
			BackgroundBackup = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, LabelDest.w, LabelFont.getHeight(), 16, 0, 0, 0, 0);
			BackupDest.x = 0; BackupDest.y = 0; BackupDest.w = LabelDest.w; BackupDest.h = LabelDest.h;
			SDL_BlitSurface(DrawingWindow->GetWindowSurface (), &LabelDest, BackgroundBackup, &BackupDest);
			SDL_BlitSurface(LabelSurface, NULL, DrawingWindow->GetWindowSurface (), &LabelDest);
		} else {
			SDL_BlitSurface(BackgroundBackup, NULL, DrawingWindow->GetWindowSurface (), &LabelDest);
			SDL_BlitSurface(LabelSurface, NULL, DrawingWindow->GetWindowSurface (), &LabelDest);
		}
	}
}

void RA_Label::Draw(SDL_Surface *DrawingSurface, int X, int Y)
{
	SetDrawingSurface(DrawingSurface);
	Draw(X, Y);
}

void RA_Label::Draw(const string& text, SDL_Surface *DrawingSurface, int X, int Y)
{
	setText(text);
	SetDrawingSurface(DrawingSurface);
	Draw(X, Y);
}

void RA_Label::Draw(const string& text, SDL_Surface *DrawingSurface, SDL_Color Fcolor, int X, int Y)
{
	setText(text);
	setColor(Fcolor);
	SetDrawingSurface(DrawingSurface);
	Draw(X, Y);
}

void RA_Label::Redraw()
{
	Draw(LabelDest.x, LabelDest.y);
}

void RA_Label::Create()
{
	SDL_Rect dest;
	SDL_Surface	*tmp;

	if (LabelSurface != NULL)
		SDL_FreeSurface(LabelSurface);

	if (BackgroundBackup != NULL && DrawingWindow != NULL){
		SDL_BlitSurface(BackgroundBackup, NULL, DrawingWindow->GetWindowSurface (), &LabelDest);
		SDL_FreeSurface(BackgroundBackup);
		BackgroundBackup = NULL;
	}

	LabelSurface = NULL;

	if (LabelText.length() == 0)
		return;

	Width = LabelFont.calcTextWidth(LabelText) + 4;
	Heigth = LabelFont.getHeight()+4;

	LabelSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, Width, Heigth, 16, 0, 0, 0, 0);

	// this is the destination as needed for the new surface
	dest.x = 0;
	dest.y = 0;
	dest.w = Width;
	dest.h = Heigth;

	// Fill the surface with the correct color
	SDL_FillRect(LabelSurface, &dest, SDL_MapRGB( LabelSurface->format, ColorKeyColor.r, ColorKeyColor.g, ColorKeyColor.b ));

	if (DrawingSurface != NULL)
		LabelFont.drawText(LabelText, DrawingSurface, LabelDest.x, LabelDest.y, LabelSurface, LabelFontColor, 0, 0);
	else if (DrawingWindow != NULL){
		LabelFont.drawText(LabelText, DrawingWindow->GetWindowSurface(), LabelDest.x, LabelDest.y, LabelSurface, LabelFontColor, 0, 0);
	}

	SDL_SetColorKey(LabelSurface, SDL_SRCCOLORKEY, 0);

//    SDL_SetAlpha(LabelSurface, SDL_SRCALPHA, 150);

	tmp = SDL_DisplayFormat(LabelSurface);
	SDL_FreeSurface(LabelSurface);
	LabelSurface = tmp;

	recreate = false;
}

void RA_Label::setText(const string text)
{
	if (LabelText != text)
	{
		if (BackgroundBackup != NULL && DrawingWindow != NULL)
		{
			SDL_BlitSurface(BackgroundBackup, NULL, DrawingWindow->GetWindowSurface (), &LabelDest);
			SDL_FreeSurface(BackgroundBackup);
			BackgroundBackup = NULL;
		}
		LabelText = text;
		recreate = true;
	}
}

/**
 * Set the font of the Label
 */
void RA_Label::SetFont(const string FontName)
{
	LabelFont.Load(FontName);
	recreate = true;
}
