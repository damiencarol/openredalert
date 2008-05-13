#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "SDL/SDL_video.h"

#include "ButtonClass.h"
#include "RA_WindowClass.h"

//class ButtonClass;
class Dune2Image;
class RA_Label;
//class RA_WindowClass;

class PauseMenu {
public:
	PauseMenu();
	~PauseMenu();

	int HandleMenu(void);

private:
	void HandleInput(void);

	RA_Label MenuLabel;
	RA_Label VersionLabel;

	SDL_Color Font_color;

	void DrawMousePointer(void);

	/** Buttons for the main menu */
	ButtonClass OptionsButton;
	ButtonClass ExitButton;
	ButtonClass ContinueButton;

	RA_WindowClass PauseWindow;

	SDL_Surface* display;
	SDL_Surface* my_cursor;

	Dune2Image *cursorimg;
	
	/** Exit the menu if this var is true */
	bool isDone; 
};

#endif //PAUSEMENU_H
