// TTextBox.h
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

#ifndef TTEXTBOX_H
#define TTEXTBOX_H

#include "SDL/SDL_events.h"
#include "SDL/SDL_video.h"

#include "RA_Label.h"

class RaWindow;

/**
 * 
 */
class TTextBox 
{
public:
	TTextBox();
	~TTextBox();
	
	bool need_redraw();
	bool Draw(int X, int Y);
	void SetDrawingWindow(RaWindow* Window);
	bool setText(const std::string& text);
	char* getText();

	bool DeleteChar(unsigned int pos);
	bool MouseOver();
	void HandleInput(SDL_Event event);
	void Create();
	void setColor(Uint32 Color);
	Uint32 getWidth();

private:
	bool AddChar(char AddChar);
	
	string TextBoxString;
	RaWindow* WindowToDrawOn;
	RA_Label TextBoxLabel;
	volatile bool Selected;
	volatile bool Recreate;
	volatile bool HasMarker;
	Uint32 TextColorSelected;
	Uint32 TextColorNotSelected;
	Uint32 TextColor;
	Uint32 TextBoxColor;
	Uint32 TextBoxBackgroundColor;
	SDL_Surface* TextBoxSurface;
	SDL_Surface* DisplaySurface;
	SDL_Rect SizeAndPosition;
};

#endif //TTEXTBOX_H
