// Menu.h
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

#ifndef MENU_H
#define MENU_H

#include <string>

#include "SDL/SDL_video.h"
#include "SDL/SDL_types.h"
#include "SDL/SDL_thread.h"

#include "RA_ProgressBar.h"
#include "RA_Label.h"
#include "Button.h"
#include "TTextBox.h"
#include "RaWindow.h"
#include "DropDownListBox.h"

#define MENUSTATE_MAIN_MENU	1
#define MENUSTATE_MISSION_1	2
#define MENUSTATE_MULTIPLAYER_1	3

class CPSImage;
class PCXImage;
class RA_Label;
class RA_ProgressBar;
class DropDownListBox;
class MissionMapsClass;
class MultiPlayerMaps;
class ListboxClass;
class Dune2Image;
class MPmapsClass;

using std::string;

/**
 * Menu of RedAlert
 */
class Menu {
public:
	Menu();
	~Menu();

	void DrawMenuBackground();
	void DrawMousePointer();
	void DrawMainMenuButtons();
	void HandleInput();
	int HandleMenu();

	/** Return true if the user selected an option */
	bool isDone() const;

	/** Return true if the user want exit the game */
	bool isQuit() const;

	/** Return true if the user want to see introduction movie of the game */
	bool isProlog() const;

private:
	void loadPal(const string& paln, SDL_Color *palette);
	void ResetSideColorButtonStates();

	/** List of mission in archives */
	MissionMapsClass* missionList;

	/** MultiPlayer maps list */
	MultiPlayerMaps* multiPlayerMaps;

	/** Button "new game" for the main menu */
	Button StartNewGameButton;

	/** Button "internet game" for the main menu */
	Button InternetGameButton;

	/** Button "load game" for the main menu */
	Button LoadMissionButton;

	/** Button "multiplayer game" for the main menu */
	Button MultiplayerGameButton;

	/** Button "review intro video" for the main menu */
	Button* IntroAndSneakPeekButton;

	/** Button "exit" for the main menu */
	Button ExitGameButton;

	Button ButtonColGreece;
	Button ButtonColUssr;
	Button ButtonColUk;
	Button ButtonColSpain;
	Button ButtonColItaly;
	Button ButtonColGermany;
	Button ButtonColFranse;
	Button ButtonColTurkey;

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
	Button RussianMissionButton;
	Button AlliesMissionButton;

	/** Buttons for the multiplayer menu */
	Button Oke;
	Button Cancel;

	/** Listbox that contains multi-player missions map */
	ListboxClass* listBox;

	RaWindow MissionMenu1;
	RaWindow MultiPlayerMenu;

	Dune2Image* cursorimg;
	CPSImage* dos_logo;
	PCXImage* win95_logo;
	SDL_Surface* SDLlogo;
	SDL_Surface* my_cursor;
	SDL_Surface* display;
	/** Exit the menu if this var is true */
	bool done;

	Uint8 numb_ai_players;

	RA_Label Labeltest;

	/**
	 * State that indicates witch menus to draw/use
	 * 1 = IntroMenu
	 * 2 = Single Player Campaign
	 * 3 = MultiPlayerGame
	 */
	int MenuState;
	int MultiplayerMapnr;

	/** Is true if the user want quit the game */
	bool quit;
	/** Is true if the user want see introduction/preview movie */
	bool prolog;
};

#endif //MENU_H
