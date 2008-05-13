#ifndef MENU_H
#define MENU_H

#include <string>

#include "SDL/SDL_video.h"
#include "SDL/SDL_types.h"
#include "SDL/SDL_thread.h"

#include "RA_ProgressBar.h"
#include "RA_Label.h"
#include "ButtonClass.h"
#include "TTextBox.h"
#include "RA_WindowClass.h"
#include "ListboxClass.h"
#include "DropDownListBox.h"
#include "game/MPmapsClass.h"
class Dune2Image;

#define MENUSTATE_MAIN_MENU	1
#define MENUSTATE_MISSION_1	2
#define MENUSTATE_MULTIPLAYER_1	3


class CPSImage;
class PCXImage;
class RA_Label;
class RA_ProgressBar;
class ButtonClass;
class DropDownListBox;
class MissionMapsClass;


class Menu {
public:
	Menu();
	~Menu();
	void DrawMenuBackground(void);
	void DrawMousePointer(void);
	void DrawMainMenuButtons(void);
	void HandleInput(void);
	int HandleMenu(void);

private:
	MPmapsClass MultiPlayerMaps;

	/** Buttons for the main menu */
	ButtonClass StartNewGameButton;
	ButtonClass InternetGameButton;
	ButtonClass LoadMissionButton;
	ButtonClass MultiplayerGameButton;
	ButtonClass IntroAndSneakPeekButton;
	ButtonClass ExitGameButton;
	ButtonClass ButtonColGreece;
	ButtonClass ButtonColUssr;
	ButtonClass ButtonColUk;
	ButtonClass ButtonColSpain;
	ButtonClass ButtonColItaly;
	ButtonClass ButtonColGermany;
	ButtonClass ButtonColFranse;
	ButtonClass ButtonColTurkey;

	RA_Label YourColor;
	RA_Label YourName;
	RA_Label YourSide;
	RA_Label AiPlayersLabel;
	RA_Label NumbAiPlayersLabel;
	RA_Label UnitsLabel;
	RA_Label NumbUnitsLabel;
	RA_Label TechlevelLabel;
	RA_Label NumbTechlevelLabel;
	RA_Label CreditsLabel;
	RA_Label NumbCreditsLabel;

	RA_ProgressBar NumbAiProgressbar;
	RA_ProgressBar NumbUnitsProgressbar;
	RA_ProgressBar TechLevelProgressbar;
	RA_ProgressBar CreditsProgressbar;

	TTextBox PlayerName;
	DropDownListBox SideSelection;

	/** Buttons for the select mission side popup menu */
	ButtonClass RussianMissionButton;
	ButtonClass AlliesMissionButton;

	/** Buttons for the multiplayer menu */
	ButtonClass Oke;
	ButtonClass Cancel;
	ListboxClass ListBox;

	RA_WindowClass MissionMenu1;
	RA_WindowClass MultiPlayerMenu;

	Dune2Image *cursorimg;
	CPSImage *dos_logo;
	PCXImage *win95_logo;
	SDL_Surface* SDLlogo;
	SDL_Surface* my_cursor;
	SDL_Surface* display;
	/** Exit the menu if this var is true */
	bool isDone;

	Uint8 numb_ai_players;

	RA_Label Labeltest;

	// Liste of mission in archives
	MissionMapsClass* missionList;

	/** 
	 * State that indicates witch menus to draw/use 
	 * 1 = IntroMenu
	 * 2 = Single Player Campaign
	 * 3 = MultiPlayerGame
	 */
	int MenuState;
	int MultiplayerMapnr;

	void loadPal(const std::string& paln, SDL_Color *palette);
	void ResetSideColorButtonStates(void);
};

#endif //MENU_H
