// Button.cpp
// 1.4

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

#include "Button.h"

#include <string>
#include <math.h>

#include "RaWindow.h"
#include "video/GraphicsEngine.h"

using std::string;

namespace pc {
	extern GraphicsEngine * gfxeng;
}

Button::Button()
{

	this->ButtonImg_up = 0; // NULL
	this->ButtonImg_down = 0; // NULL
	this->ButtonImg_over = 0; // NULL
	this->ButtonState = 1;
	this->WindowToDrawOn = 0; // NULL
	this->ButtonType = 1;

	DisplaySurface = pc::gfxeng->get_SDL_ScreenSurface();

	// The default button color is white ;)
	if (DisplaySurface != NULL){
		color	= SDL_MapRGB(DisplaySurface->format, 0xff, 0xff, 0xff);
	}

	// Initialize position and size to something
	SizeAndPosition.x = 0;
	SizeAndPosition.y = 0;
	SizeAndPosition.w = 100;
	SizeAndPosition.h = 100;

	// set color
	setcolor (96, 0, 0); // TD = :setcolor (0, 100, 0);
	// set Color for Font when up
	setFontColor_up (220, 0, 0); // TD = :setFontColor_up (0, 200, 0);
	// Set Color for font when down
	setFontColor_down (0xff, 0xff, 0xff); // TD = :setFontColor_down (0, 255, 0);

	ButtonUpLabel.SetFont("grad6fnt.fnt");
	ButtonDownLabel.SetFont("grad6fnt.fnt");
//	ButtonUpLabel.UseAntiAliasing(false);
//	ButtonDownLabel.UseAntiAliasing(false);
}

Button::~Button()
{
	if (this->ButtonImg_up != NULL){
		SDL_FreeSurface(ButtonImg_up);
	}
	if (this->ButtonImg_down != NULL){
		SDL_FreeSurface(ButtonImg_down);
	}
	if (this->ButtonImg_over != NULL){
		SDL_FreeSurface(this->ButtonImg_over);
	}
}

bool Button::handleMouseEvent(SDL_Event event)
{
	bool OldButtonState;

	if (this->ButtonType == 2){
		return false;
	}

	OldButtonState = this->ButtonState;

	// Change the button state ;)
	if (event.type == SDL_MOUSEBUTTONDOWN){
		if (this->MouseOver()){
			if (this->ButtonState != 2 || ButtonType == 1){
				this->ButtonState = 2;
			} else {
				this->ButtonState = 1;
			}
		}
	} else if(event.type == SDL_MOUSEBUTTONUP || !this->MouseOver() && this->ButtonState != 1){
		if (this->ButtonType == 1 || this->ButtonState == 3){
			this->ButtonState = 1;
		}
	} else if (MouseOver() && this->ButtonState != 2){
		this->ButtonState = 3;
	}//else if (this->ButtonState == BUTTON_STATE_OVER)
	//	this->ButtonState = BUTTON_STATE_UP;

	if (OldButtonState != this->ButtonState){
		//this->drawbutton ();
		return true;
	}

	return false;
}

void Button::SetDrawingSurface(SDL_Surface *Surface)
{
	if (Surface != 0){
		DisplaySurface = Surface;
	}	
}

void Button::SetDrawingWindow(RaWindow *Window)
{
	if (Window != 0){
		WindowToDrawOn = Window;
		DisplaySurface = WindowToDrawOn->GetWindowSurface ();
	}
	else
	{
		printf("Button::SetDrawingWindow()   Window == NULL\n");
	}
}

void Button::setFontColor_up (Uint8 red, Uint8 green, Uint8 blue)
{
	this->FontColor_up.r = red;
	this->FontColor_up.g = green;
	this->FontColor_up.b = blue;
}

void Button::setFontColor_down (Uint8 red, Uint8 green, Uint8 blue)
{
	this->FontColor_down.r = red;
	this->FontColor_down.g = green;
	this->FontColor_down.b = blue;
}

void Button::setcolor(Uint8 red, Uint8 green, Uint8 blue)
{
	ButtonColor.r = red;
	ButtonColor.g = green;
	ButtonColor.b = blue;
	color = SDL_MapRGB(DisplaySurface->format, red, green, blue);
	LightColor = SDL_MapRGB(DisplaySurface->format, (char) (red*1.3), (char) (green*1.3), (char) (blue*1.3));
	DarkColor = SDL_MapRGB(DisplaySurface->format, (char) (red*0.7), (char) (green*0.7), (char) (blue*0.7));
}

void Button::setcolor(Uint32 SDL_color)
{
	color = SDL_color;
}

void Button::setcolor(SDL_Color Color)
{
/*
	color = SDL_MapRGB(DisplaySurface->format, Color.r, Color.g, Color.b);
	LightColor = SDL_MapRGB(DisplaySurface->format, (char) (Color.r*1.3), (char) (Color.g*1.3), (char) (Color.b*1.3));
	DarkColor = SDL_MapRGB(DisplaySurface->format, (char) (Color.r*0.7), (char) (Color.g*0.7), (char) (Color.b*0.7));
*/
	this->setcolor (Color.r, Color.g, Color.b);
//	printf ("%s line %i: Set button rgb color: %i, %i, %i\n", __FILE__, __LINE__, Color.r, Color.g, Color.b);
}

Uint32 Button::GetColor ()
{
	return color;
}

void Button::setposition(int x, int y)
{
	SizeAndPosition.x = x;
	SizeAndPosition.y = y;
}

void Button::setsize(int w, int h)
{
	SizeAndPosition.w	= w;
	SizeAndPosition.h	= h;
}

void Button::setButtonState(int state)
{
	this->ButtonState = state;
}

int Button::getButtonState()
{
	return this->ButtonState;
}

void Button::drawbutton()
{
	// this is the destination as needed for the new surface
	if (this->ButtonState == 1 && this->ButtonImg_up != NULL){
		SDL_BlitSurface(this->ButtonImg_up, NULL, DisplaySurface, &SizeAndPosition);
		//printf ("Blit button up, buttontext = %s, DisplaySurface = %i\n", ButtonText.c_str(), (int)display);
	} else if (this->ButtonState == 2 && this->ButtonImg_down != NULL){
		SDL_BlitSurface(this->ButtonImg_down, NULL, DisplaySurface, &SizeAndPosition);
		//printf ("Blit button down, buttontext = %s\n", ButtonText.c_str());
	} else if (this->ButtonState == 3 && this->ButtonImg_over != NULL){
		SDL_BlitSurface(this->ButtonImg_over, NULL, DisplaySurface, &SizeAndPosition);
		//printf ("Blit button over, buttontext = %s\n", ButtonText.c_str());
	} else {
		printf ("%s line %i: Unknown buttonstate\n", __FILE__, __LINE__);
	}
}

bool Button::MouseOver()
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

	if (mx > SizeAndPosition.x && mx < SizeAndPosition.x + SizeAndPosition.w)
		if (my > SizeAndPosition.y && my < SizeAndPosition.y + SizeAndPosition.h)
			return true;
	return false;
}

bool Button::NeedsRedraw()
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

	if (mx > SizeAndPosition.x - 10 && mx < SizeAndPosition.x + 10 + SizeAndPosition.w)
		if (my > SizeAndPosition.y - 10 && my < SizeAndPosition.y + SizeAndPosition.h)
			return true;
	return false;
}

void Button::CreateSurface(const string& ButtonText, int Xpos, int Ypos, int Width, int Heigth )
{
	this->setText(ButtonText);
	this->setposition(Xpos, Ypos);
	this->setsize(Width, Heigth);
	this->CreateSurface();
}

void Button::CreateSurface()
{
	SDL_Rect		dest;
	SDL_Surface * 	tmp;

	ButtonUpLabel.setColor(FontColor_up);
	ButtonDownLabel.setColor(FontColor_down);

	// this is the destination as needed for the new surface
	dest.x = 0;
	dest.y = 0;
	dest.w = SizeAndPosition.w;
	dest.h = SizeAndPosition.h;

	// Free the button image surface (if needed)
	if (this->ButtonImg_up != NULL){
		SDL_FreeSurface(ButtonImg_up);
	}

	ButtonImg_up = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, SizeAndPosition.w, SizeAndPosition.h, 16, 0, 0, 0, 0);

	tmp = SDL_DisplayFormat(ButtonImg_up);
	SDL_FreeSurface(ButtonImg_up);
	ButtonImg_up = tmp;


	// Fill the surface with the correct color
	SDL_FillRect(ButtonImg_up, &dest, color);

	// draw left and right lines
	dest.w = 2;
	dest.h = SizeAndPosition.h;
	dest.x = 0;
	dest.y = 0;
	SDL_FillRect(ButtonImg_up, &dest, LightColor);
	dest.x = SizeAndPosition.w - 2;
	SDL_FillRect(ButtonImg_up, &dest, DarkColor);


	// draw top and bottum lines
	dest.w = SizeAndPosition.w;
	dest.h = 2;
	dest.x = 0;
	dest.y = 0;
	SDL_FillRect(ButtonImg_up, &dest, LightColor);
	dest.y = SizeAndPosition.h - 2;
	SDL_FillRect(ButtonImg_up, &dest, DarkColor);

	//
	SDL_SetColorKey(this->ButtonImg_up, SDL_SRCCOLORKEY, 0);

	int TextWidth = ButtonUpLabel.getWidth(); //this->ButtonFont.calcTextWidth(ButtonText);
	int TextHeith = ButtonUpLabel.getHeight(); //this->ButtonFont.getHeight();

	tmp = SDL_DisplayFormat(ButtonImg_up);
	SDL_FreeSurface(ButtonImg_up);
	ButtonImg_up = tmp;

	ButtonUpLabel.Draw (ButtonImg_up, SizeAndPosition.w/2 - TextWidth/2, SizeAndPosition.h/2 - TextHeith/2);
//	ButtonFont.drawText(this->ButtonText, ButtonImg_up, FontColor_up, SizeAndPosition.w/2 - TextWidth/2, SizeAndPosition.h/2 - TextHeith/2);



//###############################################################
//
//	FROM HERE WE MAKE THE BUTTON DOWN IMAGE
//
//###############################################################

	// this is the destination as needed for the new surface
	dest.x = 0;
	dest.y = 0;
	dest.w = SizeAndPosition.w;
	dest.h = SizeAndPosition.h;

	// Free the button image surface (if needed)
	if (this->ButtonImg_down != NULL){
		SDL_FreeSurface(ButtonImg_down);
	}

	ButtonImg_down = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, SizeAndPosition.w, SizeAndPosition.h, 16, 0, 0, 0, 0);

	tmp = SDL_DisplayFormat(ButtonImg_down);
	SDL_FreeSurface(ButtonImg_down);
	ButtonImg_down = tmp;

	// Fill the surface with the correct color
	SDL_FillRect(ButtonImg_down, &dest, color);

	// draw left and right lines
	dest.w = 2;
	dest.h = SizeAndPosition.h;
	dest.x = 0;
	dest.y = 0;
	SDL_FillRect(ButtonImg_down, &dest, DarkColor);
	dest.x = SizeAndPosition.w - 2;
	SDL_FillRect(ButtonImg_down, &dest, LightColor);


	// draw top and bottum lines
	dest.w = SizeAndPosition.w;
	dest.h = 2;
	dest.x = 0;
	dest.y = 0;
	SDL_FillRect(ButtonImg_down, &dest, DarkColor);
	dest.y = SizeAndPosition.h - 2;
	SDL_FillRect(ButtonImg_down, &dest, LightColor);

	//
	SDL_SetColorKey(this->ButtonImg_down, SDL_SRCCOLORKEY, 0);

	TextWidth = ButtonUpLabel.getWidth(); //this->ButtonFont.calcTextWidth(ButtonText);
	TextHeith = ButtonUpLabel.getHeight(); //this->ButtonFont.getHeight();

	tmp = SDL_DisplayFormat(ButtonImg_down);
	SDL_FreeSurface(ButtonImg_down);
	ButtonImg_down = tmp;

	ButtonDownLabel.Draw (ButtonImg_down, SizeAndPosition.w/2 - TextWidth/2, SizeAndPosition.h/2 - TextHeith/2);
//	ButtonFont.drawText(this->ButtonText, ButtonImg_down, FontColor_down, SizeAndPosition.w/2 - TextWidth/2, SizeAndPosition.h/2 - TextHeith/2);




//###############################################################
//
//	FROM HERE WE MAKE THE BUTTON OVER IMAGE (BUTTON UP WITH FONTCOLOR FOR BUTTON DOWN)
//
//###############################################################

	// this is the destination as needed for the new surface
	dest.x = 0;
	dest.y = 0;
	dest.w = SizeAndPosition.w;
	dest.h = SizeAndPosition.h;

	// Free the button image surface (if needed)
	if (this->ButtonImg_over != NULL){
		SDL_FreeSurface(this->ButtonImg_over);
	}
	
	ButtonImg_over = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, SizeAndPosition.w, SizeAndPosition.h, 16, 0, 0, 0, 0);

	tmp = SDL_DisplayFormat(ButtonImg_over);
	SDL_FreeSurface(ButtonImg_over);
	ButtonImg_over = tmp;

	// Fill the surface with the correct color
	SDL_FillRect(ButtonImg_over, &dest, color);

	// draw left and right lines
	dest.w = 2;
	dest.h = SizeAndPosition.h;
	dest.x = 0;
	dest.y = 0;
	SDL_FillRect(ButtonImg_over, &dest, LightColor);
	dest.x = SizeAndPosition.w - 2;
	SDL_FillRect(ButtonImg_over, &dest, DarkColor);


	// draw top and bottum lines
	dest.w = SizeAndPosition.w;
	dest.h = 2;
	dest.x = 0;
	dest.y = 0;
	SDL_FillRect(ButtonImg_over, &dest, LightColor);
	dest.y = SizeAndPosition.h - 2;
	SDL_FillRect(ButtonImg_over, &dest, DarkColor);

	//
	SDL_SetColorKey(this->ButtonImg_over, SDL_SRCCOLORKEY, 0);

	TextWidth = ButtonUpLabel.getWidth(); //this->ButtonFont.calcTextWidth(ButtonText);
	TextHeith = ButtonUpLabel.getHeight(); //this->ButtonFont.getHeight();

	tmp = SDL_DisplayFormat(ButtonImg_over);
	SDL_FreeSurface(ButtonImg_over);
	ButtonImg_over = tmp;

	ButtonDownLabel.Draw (ButtonImg_over, SizeAndPosition.w/2 - TextWidth/2, SizeAndPosition.h/2 - TextHeith/2);
//	ButtonFont.drawText(this->ButtonText, ButtonImg_over, FontColor_down, SizeAndPosition.w/2 - TextWidth/2, SizeAndPosition.h/2 - TextHeith/2);

	//printf ("%s line %i: End create surfaces\n", __FILE__, __LINE__);
}

void Button::setText(const string text)
{
	ButtonText = text;

	ButtonUpLabel.setText(text);
	ButtonDownLabel.setText(text);
}
