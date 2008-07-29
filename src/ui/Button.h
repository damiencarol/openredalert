// Button.h
// 1.3

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

#ifndef BUTTON_H
#define BUTTON_H

#include <list>
#include <string>
#include <vector>

#include "SDL/SDL_events.h"
#include "SDL/SDL_types.h"

#define BUTTON_TYPE_DEFAULT	1
#define BUTTON_TYPE_LOCK_UNLOCK	2
#define BUTTON_TYPE_EXTERNAL	2

#define BUTTON_STATE_UP		1
#define BUTTON_STATE_DOWN	2
#define BUTTON_STATE_OVER	3	// NOT USED YET

#include "RA_Label.h"

class RA_Label;
class RaWindow;

using std::string;

/**
 * Button in windows in RedAlertMenus
 */
class Button
{
public:
	Button();
	~Button();
	
	bool handleMouseEvent(SDL_Event event);
	void SetDrawingSurface(SDL_Surface *Surface);
	void SetDrawingWindow(RaWindow* Window);
	/** Set the font color in the button up state.*/
	void setFontColor_up(Uint8 red, Uint8 green, Uint8 blue);
	/**  Set the font color in the button down state.*/
	void setFontColor_down(Uint8 red, Uint8 green, Uint8 blue);
	void setcolor(Uint8 red, Uint8 green, Uint8 blue);
	void setcolor(Uint32 SDL_color);
	void setcolor(SDL_Color Color);
	Uint32 GetColor();
	void setposition(int x, int y);
	void setsize(int w, int h);
	void setText(const string buff);
	void setButtonState(int state);
	int getButtonState();
	void drawbutton();
	bool MouseOver();
	bool NeedsRedraw();
	void CreateSurface();
	void CreateSurface(const string& ButtonText, int Xpos, int Ypos, int Width, int Heigth);

private:
	RaWindow* WindowToDrawOn;
	SDL_Color FontColor_up;
	SDL_Color FontColor_down;
	SDL_Color ButtonColor;
	SDL_Surface* DisplaySurface;
	SDL_Surface* ButtonImg_up;
	SDL_Surface* ButtonImg_down;
	SDL_Surface* ButtonImg_over;
	SDL_Rect SizeAndPosition;
	/** SDL color (of the button) */
	Uint32 color;
	Uint32 LightColor;
	Uint32 DarkColor;
	RA_Label ButtonUpLabel;
	RA_Label ButtonDownLabel;
	
	string ButtonText;
	/**
	 * Button state tells the class if the button is up,
	 *  down or the mouse is  over the button
	 */
	int ButtonState;
	Uint8 ButtonType;
};

#endif //BUTTON_H
