// ListBoxClass.cpp
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

#include "ListboxClass.h"

#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "RA_Label.h"
#include "Font.h"
#include "include/config.h"
#include "video/GraphicsEngine.h"
#include "video/ImageCache.h"
#include "include/sdllayer.h"
#include "RA_ProgressBar.h"
#include "RaWindow.h"
#include "TTextBox.h"
#include "video/ImageNotFound.h"
#include "video/CPSImage.h"
#include "video/SHPImage.h"

using std::string;
using std::vector;

namespace pc {
	extern GraphicsEngine * gfxeng;
}

ListboxClass::ListboxClass()
{
	ListBoxSurface = 0;

	ScrollPos	= 0;
	SelectPos	= 0;
	With		= 350;
	NumbLines	= 8;
	ListBoxType	= 1;
//	ListBoxType	= LISTBOX_TYPE_CHECK;
	DisplaySurface	= pc::gfxeng->get_SDL_ScreenSurface();

	btn_up_down	= 0;
	btn_down_down	= 0;

	for (int i = 0; i < 2; i++){
		btn_up[i]	= NULL;
		btn_down[i]	= NULL;
	}

	//printf ("ListBox class initialized\n");
}

/**
 * 
 */
ListboxClass::~ListboxClass()
{
	//
	if (ListBoxSurface != NULL){
		SDL_FreeSurface(ListBoxSurface);
	}

	for (int i = 0; i < 2; i++){
		if (btn_up[i] != NULL){
			SDL_FreeSurface(btn_up[i]);
		}
		if (btn_down[i] != NULL){
			SDL_FreeSurface(btn_down[i]);
		}
	}

	for (unsigned int i = 0; i < ListboxLines.size(); i++){
		SDL_FreeSurface(ListboxLines[i].Line);
		SDL_FreeSurface(ListboxLines[i].SelectedLine);
	}

}

/**
 * 
 */
SDL_Surface *ListboxClass::ReadShpImage (char *Name, int ImageNumb, Uint8 palnum)
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

void ListboxClass::SetType (unsigned int type)
{
	if (type < 1 || type > 2)
		return;

	ListBoxType	= type;
}

bool ListboxClass::IsChecked (unsigned int line)
{
	if (ListBoxType != 2)
		return false;

	if (line >= CheckedList.size())
		return false;

	if (CheckedList[line])
		return true;

	return false;
}

int ListboxClass::GetSelectionIndex (void)
{
	return (/*ScrollPos + */SelectPos);
}

SDL_Surface* ListboxClass::geDrawingSurface()
{
	return ListBoxSurface;
}

void ListboxClass::ScrollUp()
{
//	if (SelectPos > 0){
//		SelectPos--;
//		Recreate = true;
//	}else
	if (ScrollPos > 0){
		ScrollPos--;
		Recreate = true;
	}
}

void ListboxClass::ScrollDown()
{
//	if (SelectPos < NumbLines - 1){
//		SelectPos++;
//		Recreate = true;
//	}else
	if ((Uint16)(ScrollPos + 1) < ((Uint16)MessageList.size() - NumbLines)){
		ScrollPos++;
		Recreate = true;
	}
}

void ListboxClass::HandleInput(SDL_Event event)
{
	int MouseXpos;
	int MouseYpos;


	switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			//printf ("%s line %i: Entry = %i\n", __FILE__, __LINE__, MouseOver_entry());
			SDL_GetMouseState(&MouseXpos, &MouseYpos);

			// Handle item selection by mouse click
			if (ListBoxType	== 1){
				if ((MouseXpos > Xpos + 10) && (MouseXpos < (ListBoxSurface->w + Xpos - 15 - 10))){
					if ((MouseYpos > Ypos + 10) && (MouseYpos < (ListBoxSurface->h + Ypos - 10))){
						Uint16 pos = (MouseYpos - Ypos - 10) / (FontHeigth+2);
						if ((unsigned)(pos + ScrollPos) < MessageList.size()){
							SelectPos = pos + ScrollPos;
							Recreate = true;
						}
					}
				}
			}else if (ListBoxType	== 2){
				if ((MouseXpos > Xpos + 10) && (MouseXpos < (ListBoxSurface->w + Xpos - 15 - 10))){
					if ((MouseYpos > Ypos + 10) && (MouseYpos < (ListBoxSurface->h + Ypos - 10))){
						Uint16 pos = (MouseYpos - Ypos - 10) / (FontHeigth+2);
						if ((unsigned)(pos + ScrollPos) < CheckedList.size()){
							if (CheckedList[pos + ScrollPos] == false)
								CheckedList[pos + ScrollPos] = true;
							else
								CheckedList[pos + ScrollPos] = false;
							Recreate = true;
						}
					}
				}
			}

			// Handle the scrollup button
			if (this->MouseOverArrowDown ()){
				this->ScrollDown ();
				btn_down_down = 1;
			}

			// Handle the scrolldown button
			if (this->MouseOverArrowUp ()){
				this->ScrollUp ();
				btn_up_down = 1;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (btn_up_down){
				btn_up_down = 0;
				Recreate = true;
			}
			if (btn_down_down){
				btn_down_down = 0;
				Recreate = true;
			}
			break;

		case SDL_KEYDOWN:

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

bool ListboxClass::MouseOverArrowUp()
{
	int MouseXpos, MouseYpos;

	if (ListBoxSurface == NULL)
		return false;

	SDL_GetMouseState(&MouseXpos, &MouseYpos);

	if ((MouseXpos > (ListBoxSurface->w + Xpos - 15)) && (MouseXpos < (ListBoxSurface->w + Xpos)))
		if ((MouseYpos > Ypos) && (MouseYpos < Ypos + 16))
			return true;

	return false;
}

bool ListboxClass::MouseOverArrowDown()
{
int MouseXpos, MouseYpos;

	SDL_GetMouseState(&MouseXpos, &MouseYpos);

	if ((MouseXpos > (ListBoxSurface->w + Xpos - 15)) && (MouseXpos < (ListBoxSurface->w + Xpos)))
		if ((MouseYpos > Ypos + ListBoxSurface->h - 16) && (MouseYpos < Ypos + ListBoxSurface->h))
			return true;

	return false;
}

void ListboxClass::AddString(const string String)
{
	// Add the text to the list
	MessageList.push_back (String);
	CheckedList.push_back (false);
	Recreate = true;
}

void ListboxClass::DrawListBox(int X, int Y)
{
	SDL_Rect dest;

	Xpos = X;
	Ypos = Y;

	if (Recreate)
		this->CreateListBox ();

	if (ListBoxSurface == NULL)
		return;

	dest.x = Xpos;
	dest.y = Ypos;
	dest.w = With;
	dest.h = FontHeigth * NumbLines;

	if (DisplaySurface != NULL)
		SDL_BlitSurface(ListBoxSurface, NULL, DisplaySurface, &dest);
}

unsigned int ListboxClass::CalcSliderHeight()
{
	unsigned int AvailableHeight;

	AvailableHeight = ListBoxSurface->h - 2*16;

	if (MessageList.size() > (unsigned) NumbLines)
		return (AvailableHeight * NumbLines)/MessageList.size();
	else
		return AvailableHeight;
}

unsigned int ListboxClass::CalcSliderYoffset()
{
	unsigned int AvailableHeight;

	AvailableHeight = ListBoxSurface->h - 2*16;

	if (MessageList.size() > (unsigned) NumbLines && ScrollPos > 0 ){
		if ( (MessageList.size() - NumbLines) > 0)
			return (AvailableHeight - CalcSliderHeight()) * ScrollPos/(MessageList.size() - NumbLines - 1);
	}
	return 0;
}

void ListboxClass::DrawSlider()
{
	SDL_Surface	*Slider, *tmp;
	SDL_Rect	dest;

	if (ListBoxSurface == NULL)
		return;

	Slider = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, 15, CalcSliderHeight(), 16, 0, 0, 0, 0);

	tmp = SDL_DisplayFormat(Slider);
	SDL_FreeSurface(Slider);
	Slider = tmp;

	Uint32 ColBlack		= SDL_MapRGB(DisplaySurface->format, 0, 0, 0);
	Uint32 ColEdgeDark	= SDL_MapRGB(DisplaySurface->format, 65, 0, 0);
	Uint32 ColEdgeLigth	= SDL_MapRGB(DisplaySurface->format, 205, 0, 0);

	// Fill the surface with black
	dest.x = 0;
	dest.y = 0;
	dest.w = Slider->w;
	dest.h = Slider->h;
	SDL_FillRect(Slider, &dest, ColBlack);

	// Top
	dest.x = 0;
	dest.y = 0;
	dest.w = Slider->w;
	dest.h = 1;
	SDL_FillRect(Slider, &dest, ColEdgeLigth);

	// Bottum
	dest.x = 0;
	dest.y = Slider->h - 1;
	dest.w = Slider->w;
	dest.h = 1;
	SDL_FillRect(Slider, &dest, ColEdgeDark);

	// Left
	dest.x = 0;
	dest.y = 0;
	dest.w = 1;
	dest.h = Slider->h;
	SDL_FillRect(Slider, &dest, ColEdgeLigth);

	// Rigth
	dest.x = Slider->w - 1;
	dest.y = 0;
	dest.w = 1;
	dest.h = Slider->h;
	SDL_FillRect(Slider, &dest, ColEdgeDark);


	dest.x = ListBoxSurface->w - 15;
	dest.y = 16 + CalcSliderYoffset();
	dest.w = Slider->w;
	dest.h = Slider->h;

	SDL_SetColorKey(Slider,SDL_SRCCOLORKEY, 0x000000);

	SDL_BlitSurface(Slider, NULL, ListBoxSurface, &dest);
	SDL_FreeSurface(Slider);
}

void ListboxClass::CreateListBox()
{
	SDL_Rect	dest;
	SDL_Surface	*TempImage;
	RA_Label	Label;
	int		BorderWidth = 10,
	Spacing = 2;
	SDL_Color 	RGB_ColWhite, RGB_ColRed;
	SDL_Surface	* tmp;

	Label.SetFont("grad6fnt.fnt");

	// Take care of loading the button surfaces only once
	for (int i = 0; i < 2; i++){
		if (btn_up[i] == NULL){
			btn_up[i] = ReadShpImage ((char*)"btn-up.shp", i, 2);
			tmp = SDL_DisplayFormat(btn_up[i]);
			SDL_FreeSurface(btn_up[i]);
			btn_up[i] = tmp;

			//
			// Make buttons from blue to red
			// Stupid hack, but don't know how else to do this
			//
			if (btn_up[i] != NULL){
				SDL_Color color;
				for (Uint32 x = 0; x < (Uint32) btn_up[i]->w; x++){
					for (Uint32 y = 0; y < (Uint32) btn_up[i]->h; y++){
						SDLLayer::get_pixel ( btn_up[i], color, x, y );
						//printf ("Color = [%u:%u:%u]\n", color.r, color.g, color.b);
						color.r = (color.b-40)&0xff;
						color.g = color.b = 0;
						SDLLayer::set_pixel (btn_up[i], color, x, y );
					}
				}
			}
		}
		if (btn_down[i] == NULL){
			btn_down[i] = ReadShpImage ((char*)"btn-dn.shp", i, 2);
			tmp = SDL_DisplayFormat(btn_down[i]);
			SDL_FreeSurface(btn_down[i]);
			btn_down[i] = tmp;

			//
			// Make buttons from blue to red
			// Stupid hack, but don't know how else to do this
			//
			if (btn_down[i] != NULL){
				SDL_Color color;
				for (Uint32 x = 0; x < (Uint32) btn_down[i]->w; x++){
					for (Uint32 y = 0; y < (Uint32) btn_down[i]->h; y++){
						SDLLayer::get_pixel ( btn_down[i], color, x, y );
						color.r = (color.b-40)&0xff;
						color.g = color.b = 0;
						SDLLayer::set_pixel (btn_down[i], color, x, y );
					}
				}
			}
		}
	}

	RGB_ColWhite.r = RGB_ColWhite.g = RGB_ColWhite.b = 255;
	RGB_ColRed.r = 205; RGB_ColRed.g = 0; RGB_ColRed.b = 0;

	this->FontHeigth = Label.getHeight();

	if (ListBoxSurface != NULL){
		SDL_FreeSurface(ListBoxSurface);
	}

	ListBoxSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, With, ((2 * BorderWidth) + (this->FontHeigth * NumbLines) + (NumbLines* Spacing)), 16, 0, 0, 0, 0);

	tmp = SDL_DisplayFormat(ListBoxSurface);
	SDL_FreeSurface(ListBoxSurface);
	ListBoxSurface = tmp;

	Uint32 ColBlack 	= SDL_MapRGB(DisplaySurface->format, 1, 1, 1);
	Uint32 ColRed		= SDL_MapRGB(DisplaySurface->format, 98, 0, 0);
	Uint32 ColEdgeRed	= SDL_MapRGB(DisplaySurface->format, 255, 0, 0);

	dest.x = 0;
	dest.y = 0;
	dest.w = ListBoxSurface->w;
	dest.h = ListBoxSurface->h;

	SDL_FillRect(ListBoxSurface, &dest, ColBlack);

	// Draw the background for the slider side
	dest.x = ListBoxSurface->w - 15;
	dest.y = 0;
	dest.w = 15;
	dest.h = ListBoxSurface->h;
	SDL_FillRect(ListBoxSurface, &dest, ColRed);


	// Draw a rectange around the text area
	dest.x = 0;
	dest.w = ListBoxSurface->w;
	dest.h = 1;
	SDL_FillRect(ListBoxSurface, &dest, ColEdgeRed);
	dest.y = ListBoxSurface->h - 1;
	SDL_FillRect(ListBoxSurface, &dest, ColEdgeRed);
	dest.y = 0;
	dest.w = 1;
	dest.h = ListBoxSurface->h;
	SDL_FillRect(ListBoxSurface, &dest, ColEdgeRed);
	dest.x = ListBoxSurface->w - 1 - 15;
	SDL_FillRect(ListBoxSurface, &dest, ColEdgeRed);
	dest.x = ListBoxSurface->w - 1;
	SDL_FillRect(ListBoxSurface, &dest, ColEdgeRed);


	// Draw the scroll up icons
	if (btn_up[btn_up_down] != NULL && btn_down[btn_down_down] != NULL){
		if (btn_up_down > 1)
			btn_up_down = 0 ;
		if (btn_down_down > 1)
			btn_down_down = 0 ;

		dest.x = ListBoxSurface->w - btn_up[btn_up_down]->w;
		dest.y = 0;
		dest.w = btn_up[btn_up_down]->w;
		dest.h = btn_up[btn_up_down]->h;
		SDL_BlitSurface(btn_up[btn_up_down], NULL, ListBoxSurface, &dest);

		// Draw the scroll down icons
		dest.y = ListBoxSurface->h - btn_down[btn_down_down]->h;
		SDL_BlitSurface(btn_down[btn_down_down], NULL, ListBoxSurface, &dest);
	}

	if (MessageList.size() <= 0){
		return;
	}

	if (ScrollPos >= MessageList.size()){
		ScrollPos = MessageList.size() - 1;
}


#if 0
	/// @todo cache all line surface in selected and not selected mode, this will speed up considerably !!
	/// Use ListboxLines to do this ;)
	TempImage = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, ListBoxSurface->w - LISTBOX_SCROLLBAR_WITH  - 2 * BorderWidth/*Font.calcTextWidth(MessageList[i+ScrollPos])*/, this->FontHeigth, 16, 0, 0, 0, 0);

	tmp = SDL_DisplayFormat(TempImage);
	SDL_FreeSurface(TempImage);
	TempImage = tmp;

	// Draw the font images on the listbox surface
	for (Uint16 i = 0; i < NumbLines; i++){

		if ((unsigned)(i+ScrollPos) < MessageList.size()){

			if (ListBoxType	== LISTBOX_TYPE_NORMAL){
				dest.x = 0;
				dest.y = 0;
				dest.w = ListBoxSurface->w - LISTBOX_SCROLLBAR_WITH - 10;
				dest.h = this->FontHeigth;

				if (i == (SelectPos - ScrollPos)){
					// Draw the selection line
					SDL_FillRect(TempImage, &dest, ColRed);
					Font.drawText(MessageList[i+ScrollPos], TempImage, RGB_ColWhite, 5, 0);
				}else{
					SDL_FillRect(TempImage, &dest, ColBlack);
					Font.drawText(MessageList[i+ScrollPos], TempImage, RGB_ColWhite, 5, 0);
				}
			}else if (ListBoxType	== LISTBOX_TYPE_CHECK){
				dest.x = 0;
				dest.y = 0;
				dest.w = ListBoxSurface->w - LISTBOX_SCROLLBAR_WITH - 10;
				dest.h = this->FontHeigth;

				if (CheckedList[i + ScrollPos]){
					// Draw the selection line
					SDL_FillRect(TempImage, &dest, ColRed);
					Font.drawText(MessageList[i+ScrollPos], TempImage, RGB_ColWhite, 5, 0);
				}else{
					SDL_FillRect(TempImage, &dest, ColBlack);
					Font.drawText(MessageList[i+ScrollPos], TempImage, RGB_ColWhite, 5, 0);
				}
			}

			dest.x = BorderWidth;
			dest.w = With - 2*BorderWidth;
			dest.y = (FontHeigth * i) + (Spacing * i) + BorderWidth; //TempImage->h * i;
			dest.h = TempImage->h;
			SDL_BlitSurface(TempImage, NULL, ListBoxSurface, &dest);
		}
	}
	SDL_FreeSurface(TempImage);
#else
	
	ListBoxLine Line;
	
	// Create the line surfaces (if needed)
	if (ListboxLines.size() < (unsigned) MessageList.size()){

		// First free the previous listboxlines.
		for (unsigned int i = 0; i < ListboxLines.size(); i++){
			SDL_FreeSurface(ListboxLines[i].Line);
			SDL_FreeSurface(ListboxLines[i].SelectedLine);
		}
		ListboxLines.clear();

		// Draw the font images on the listbox surface
		for (Uint16 i = 0; i < MessageList.size(); i++){
			if (ListBoxType	== 1){
				dest.x = 0;
				dest.y = 0;
				dest.w = ListBoxSurface->w - 15 - 10;
				dest.h = this->FontHeigth;

				// Draw the selection line
				TempImage = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, ListBoxSurface->w - 15  - 2 * BorderWidth/*Font.calcTextWidth(MessageList[i+ScrollPos])*/, this->FontHeigth, 16, 0, 0, 0, 0);
				tmp = SDL_DisplayFormat(TempImage);
				SDL_FreeSurface(TempImage);
				TempImage = tmp;
				SDL_FillRect(TempImage, &dest, ColRed);
				Label.Draw(MessageList[i+ScrollPos], TempImage, RGB_ColWhite, 5, 0);
				Line.SelectedLine = TempImage;

				// Create the not selected line surface
				TempImage = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, ListBoxSurface->w - 15  - 2 * BorderWidth/*Font.calcTextWidth(MessageList[i+ScrollPos])*/, this->FontHeigth, 16, 0, 0, 0, 0);
				tmp = SDL_DisplayFormat(TempImage);
				SDL_FreeSurface(TempImage);
				TempImage = tmp;
				SDL_FillRect(TempImage, &dest, ColBlack);
				Label.Draw(MessageList[i+ScrollPos], TempImage, RGB_ColRed /*RGB_ColWhite*/, 5, 0);
				Line.Line = TempImage;

				ListboxLines.push_back (Line);
			}else if (ListBoxType	== 2){
				dest.x = 0;
				dest.y = 0;
				dest.w = ListBoxSurface->w - 15 - 10;
				dest.h = this->FontHeigth;


				// Create the selected line surface
				TempImage = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, ListBoxSurface->w - 15  - 2 * BorderWidth/*Font.calcTextWidth(MessageList[i+ScrollPos])*/, this->FontHeigth, 16, 0, 0, 0, 0);
				tmp = SDL_DisplayFormat(TempImage);
				SDL_FreeSurface(TempImage);
				TempImage = tmp;
				SDL_FillRect(TempImage, &dest, ColRed);
				Label.Draw(MessageList[i+ScrollPos], TempImage, RGB_ColWhite, 5, 0);
				Line.SelectedLine = TempImage;

				// Create the not selected line surface
				TempImage = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, ListBoxSurface->w - 15  - 2 * BorderWidth/*Font.calcTextWidth(MessageList[i+ScrollPos])*/, this->FontHeigth, 16, 0, 0, 0, 0);
				tmp = SDL_DisplayFormat(TempImage);
				SDL_FreeSurface(TempImage);
				TempImage = tmp;
				SDL_FillRect(TempImage, &dest, ColBlack);
				Label.Draw(MessageList[i+ScrollPos], TempImage, RGB_ColRed /*RGB_ColWhite*/, 5, 0);
				Line.Line = TempImage;

				ListboxLines.push_back (Line);
			}
		}
	}

	// Draw the line surfaces
	for (Uint16 i = 0; i < NumbLines; i++){
		if ((unsigned)(i+ScrollPos) < MessageList.size()){

			TempImage = NULL;

			if (i == (SelectPos - ScrollPos)){
				// Draw the selection line
				TempImage = ListboxLines[i+ScrollPos].SelectedLine;
			}else{
				// Draw the not selection line
				TempImage = ListboxLines[i+ScrollPos].Line;
			}

			if (TempImage != NULL){
				dest.x = BorderWidth;
				dest.w = With - 2*BorderWidth;
				dest.y = (FontHeigth * i) + (Spacing * i) + BorderWidth; //TempImage->h * i;
				dest.h = TempImage->h;
				SDL_BlitSurface(TempImage, NULL, ListBoxSurface, &dest);
			}
		}
	}
#endif
	DrawSlider();

	Recreate = false;
}
