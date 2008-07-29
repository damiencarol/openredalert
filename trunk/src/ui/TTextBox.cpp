// TTextBox.cpp
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

#include "TTextBox.h"

#include <string>
#include <math.h>

#include "RA_Label.h"
#include "TTextBox.h"
#include "SDL/SDL_video.h"
#include "Font.h"
#include "video/CPSImage.h"
#include "include/common.h"
#include "include/config.h"
#include "video/GraphicsEngine.h"
#include "video/ImageCache.h"
#include "include/sdllayer.h"
#include "RaWindow.h"

using std::string;

namespace pc {
	extern GraphicsEngine * gfxeng;
}

bool TTextBox::AddChar(char AddChar)
{
    std::string	newtext;
	newtext = TextBoxString;
	newtext += AddChar;

	if ((unsigned) SizeAndPosition.h < (unsigned) TextBoxLabel.getHeight())
		return false;

	if ((unsigned)  SizeAndPosition.w-8 < (unsigned) TextBoxLabel.getWidth(newtext))
		return false;

	TextBoxString = newtext;

	Recreate = true;

	return true;
}

TTextBox::TTextBox()
{
	// Setup some vars
	Selected	= false;
	HasMarker   = false;
	Recreate	= true;
	TextBoxSurface	= NULL;
	DisplaySurface	= NULL;
	WindowToDrawOn	= NULL;

	SizeAndPosition.x = 0;
	SizeAndPosition.y = 0;
	SizeAndPosition.h = 16;
	SizeAndPosition.w = 100;

	// Initialize the surfaces
	DisplaySurface	= pc::gfxeng->get_SDL_ScreenSurface();
	//TextBoxSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, Width, Heigth, 16, 0, 0, 0, 0);

	// Initialize the checkbox color
	TextColor		        = SDL_MapRGB(DisplaySurface->format, 0xff, 0, 0);
	TextBoxColor		    = SDL_MapRGB(DisplaySurface->format, 0xff, 0, 0);
	TextBoxBackgroundColor	= SDL_MapRGB(DisplaySurface->format, 0, 0, 0);
	TextColorSelected       = SDL_MapRGB(DisplaySurface->format, 255,255,255); //    TextColorNotSelected
	SDL_EnableUNICODE(true);

	// Initialize the text
	TextBoxString = "Player";
}

TTextBox::~TTextBox()
{
	if (TextBoxSurface != NULL){
		SDL_FreeSurface(TextBoxSurface);
	}
}

bool TTextBox::need_redraw(void)
{
	return Recreate;
}
bool TTextBox::Draw(int X, int Y)
{
	// Set size and posisiton
	SizeAndPosition.x = X;
	SizeAndPosition.y = Y;

	if (this->Recreate){
		Create();
	}

	if (TextBoxSurface == NULL){
		return false;
	}

	if (DisplaySurface == NULL){
		return false;
	}

	if (WindowToDrawOn != NULL){
		SDL_BlitSurface(TextBoxSurface, NULL, WindowToDrawOn->GetWindowSurface (), &SizeAndPosition);
	} else {
		SDL_BlitSurface(TextBoxSurface, NULL, DisplaySurface, &SizeAndPosition);
	}
	
	return true;
}

void TTextBox::SetDrawingWindow(RaWindow *Window)
{
	if (Window != NULL){
		WindowToDrawOn = Window;
	}
}

char* TTextBox::getText()
{
     if(HasMarker)
       this->DeleteChar(TextBoxString.size() - 1);

	 return (char*)TextBoxString.c_str();
}

bool TTextBox::DeleteChar(unsigned int pos)
{
	if (TextBoxString.size() > pos){
		TextBoxString.erase (pos, 1);
	}else
		return false;

	Recreate = true;

	return true;
}

bool TTextBox::MouseOver()
{
int mx, my;
int WinXpos = 0, WinYpos = 0;

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

void TTextBox::HandleInput(SDL_Event event)
{
    //SDL_Event event;
    int mx, my;
    int WinXpos = 0, WinYpos = 0;

	SDL_GetMouseState(&mx, &my);

	if (WindowToDrawOn != NULL){
		WindowToDrawOn->GetWindowPosition (&WinXpos, &WinYpos);
		mx -= WinXpos;
		my -= WinYpos;
	}

	switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			if (MouseOver()){
                if(!Selected){
                  this->AddChar('_');
                  HasMarker = true;
                }
                TextColorNotSelected = TextColor;
                TextColor = TextColorSelected;
				Selected = true;
			}else{
               if(HasMarker){
                  this->DeleteChar(TextBoxString.size() - 1);
                  HasMarker = false;
                  TextColor = TextColorNotSelected;
               }
				Selected = false;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			break;

		case SDL_KEYDOWN:
			if (!Selected)
				return;

			switch (event.key.keysym.sym){
				case SDLK_BACKSPACE:
					// Remove last char
					if(TextBoxString.size() > 1)
					  this->DeleteChar(TextBoxString.size() - 2);
					break;
				case SDLK_TAB:
				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					break;
				default:
					//SDL_EnableUNICODE(true); is needed for this to work ;)
					char ch = 0;
					if ( (event.key.keysym.unicode & 0xFF80) == 0 ) {
  						ch = event.key.keysym.unicode & 0x7F;
						//printf ("Character = %c\n", ch);
						if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')){
                          if(HasMarker && TextBoxString.size() > 0){
                            this->DeleteChar(TextBoxString.size() - 1);
                            HasMarker = false;
                          }
							this->AddChar (ch);
                            if(!HasMarker){
                              this->AddChar('_');
                              HasMarker = true;
                            }
                        }
                    }
					break;
			}
			break;
	}
}

void TTextBox::Create()
{
    SDL_Rect	dest;
    Uint16		Ypos;
    char		*temp;
	Recreate	= false;

	TextBoxLabel.setText(TextBoxString);
	TextBoxLabel.setColor(TextColor);

	temp = (char *) TextBoxString.c_str();

//	printf ("%s line %i: Recreate text box, text = %s\n", __FILE__, __LINE__, temp);

	if (TextBoxSurface != NULL){
		SDL_FreeSurface(TextBoxSurface);
	}

	TextBoxSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, SizeAndPosition.w, SizeAndPosition.h, 16, 0, 0, 0, 0);

	SDL_Surface *tmp = SDL_DisplayFormat(TextBoxSurface);
	SDL_FreeSurface(TextBoxSurface);
	TextBoxSurface = tmp;

	// this is the destinatc_strion as needed for the new surface
	dest.x = 0;
	dest.y = 0;
	dest.w = SizeAndPosition.w;
	dest.h = SizeAndPosition.h;

	// Fill the surface with the correct color
	SDL_FillRect(TextBoxSurface, &dest, TextBoxColor);

	// this is the destination as needed for the new surface
	dest.x = 1;
	dest.y = 1;
	dest.w = SizeAndPosition.w-2;
	dest.h = SizeAndPosition.h-2;

	// Fill the surface with the correct color
	SDL_FillRect(TextBoxSurface, &dest, TextBoxBackgroundColor);

	if ((unsigned) SizeAndPosition.h < (unsigned) TextBoxLabel.getHeight())
		return;

	if ((unsigned)  SizeAndPosition.w-8 < (unsigned) TextBoxLabel.getWidth()){
		return;
    }

	Ypos = (SizeAndPosition.h - TextBoxLabel.getHeight())/2;

//	TextBoxFont.drawText(TextBoxString, TextBoxSurface, TextColor, 4, Ypos) ; //drawText("Your color", ListBox.geDrawingSurface(), 100, 20);
	TextBoxLabel.Draw(TextBoxSurface, 4, Ypos);

}

void TTextBox::setColor(Uint32 color)
{
	TextColor = color;
	TextBoxLabel.setColor(color);
}

Uint32 TTextBox::getWidth()
{
	return SizeAndPosition.w;
}
