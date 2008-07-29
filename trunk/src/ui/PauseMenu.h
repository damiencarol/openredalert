// PauseMenu.h
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

#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "SDL/SDL_video.h"

class Button;
class Dune2Image;
class RA_Label;
class RaWindow;

/**
 * Pause menu in the game
 */
class PauseMenu 
{
public:
	PauseMenu();
	~PauseMenu();

	int HandleMenu();

private:
	void HandleInput();
	void DrawMousePointer();
	
	/** Label for the Pause menu */
	RA_Label* MenuLabel;
	/** Label for version */
	RA_Label* VersionLabel;

	SDL_Color Font_color;
	
	/** Buttons "Option" for the main menu */
	Button* OptionsButton;
	/** Buttons "Exit" for the main menu */
	Button* ExitButton;
	/** Buttons "Resume" for the main menu */
	Button* ContinueButton;

	RaWindow* PauseWindow;

	SDL_Surface* display;
	SDL_Surface* my_cursor;

	/** Image for the cursor */
	Dune2Image *cursorimg;
	
	/** Exit the menu if this var is true */
	bool isDone; 
};

#endif //PAUSEMENU_H
