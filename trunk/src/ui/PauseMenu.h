// PauseMenu.h
// 1.3

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
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
class RA_WindowClass;

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
	
	RA_Label* MenuLabel;
	RA_Label* VersionLabel;

	SDL_Color Font_color;
	
	/** Buttons for the main menu */
	Button* OptionsButton;
	Button* ExitButton;
	Button* ContinueButton;

	RA_WindowClass* PauseWindow;

	SDL_Surface* display;
	SDL_Surface* my_cursor;

	Dune2Image *cursorimg;
	
	/** Exit the menu if this var is true */
	bool isDone; 
};

#endif //PAUSEMENU_H
