// Menu.cpp
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

#include "Menu.h"

#include "RaWindow.h"
#include "RA_Label.h"
#include "DropDownListBox.h"
#include "ListboxClass.h"
#include "Font.h"
#include "Cursor.h"
#include "TCheckBox.h"

#include "include/config.h"
#include "include/Logger.h"
#include "game/MissionMapsClass.h"
#include "game/MultiPlayerMaps.h"
#include "game/GameMode.h"
#include "misc/StringTableFile.h"
#include "vfs/VFile.h"
#include "vfs/vfs.h"
#include "video/Dune2Image.h"
#include "video/ImageNotFound.h"
#include "video/PCXImage.h"
#include "video/CPSImage.h"
#include "video/VQAMovie.h"
#include "video/GraphicsEngine.h"

using VQA::VQAMovie;

namespace pc {
	extern ConfigType Config;
	extern GraphicsEngine * gfxeng;
}
extern Logger * logger;

/**
 *
 * @bug the constructor of menu make some bugs in map drawing
 */
Menu::Menu() : StartNewGameButton()
{
	SDL_Color	palette[256];
	SDL_Color	Fcolor;
	Uint16		Ypos;
	StringTableFile* strFile = NULL;

	// Use located strings
	strFile = new StringTableFile("conquer.eng");

	// Use the red backgound for the multiplayer menu and the mission menu
	this->MultiPlayerMenu.setPalette(9);
	MissionMenu1.setPalette(9);

	// Setup the font color
	Fcolor.r = 205;
	Fcolor.g = 0;
	Fcolor.b = 0;

	win95_logo	= 0;
	dos_logo	= 0;
	SDLlogo 	= 0;
	cursorimg	= 0;
	MenuState	= 1;

	done = false;
	quit = false;
	prolog = false;

	display = pc::gfxeng->get_SDL_ScreenSurface();

	this->loadPal("SNOW", palette);
	SHPBase::setPalette(palette);
	SHPBase::calculatePalettes();

	try {
		win95_logo = new PCXImage("title.pcx",-1);
	} catch (ImageNotFound&) {
		logger->error("Couldn't load startup graphic\n");
		win95_logo = NULL;
	}

	if (win95_logo == NULL){
		// failed to load the win95 logo -> try the dos logo
		try {
			dos_logo = new CPSImage("title.cps",1);
		} catch (ImageNotFound&) {
			logger->error("Couldn't load startup graphic\n");
			dos_logo = NULL;
		}
	}

	// Get the logo and draw it to the screen
	SDL_Surface* unscaled_logo = NULL;
	float scale_factor;
	#define MIN(a, b) (a < b ? a : b)
	if (win95_logo != NULL)
		unscaled_logo = win95_logo->getImage();
	else if (dos_logo != NULL)
		unscaled_logo = dos_logo->getImage();

	scale_factor = (1.0 * display->w / unscaled_logo->w < 1.0 * display->h / unscaled_logo->h ? 1.0 * display->w / unscaled_logo->w : 1.0 * display->h / unscaled_logo->h);

	ImageProc scaler;
	scale_factor = (1.0 * display->w / unscaled_logo->w < 1.0 * display->h / unscaled_logo->h ? 1.0 * display->w / unscaled_logo->w : 1.0 * display->h / unscaled_logo->h);
	SDLlogo = scaler.scale(unscaled_logo, display->w, display->h*unscaled_logo->h/480);

	/// The unscaled_logo image should not be freed (as it is cached and eventually freed by the pcx image class)

	this->DrawMenuBackground();
	SDL_Flip(display);

	// Load the cursor
	cursorimg = new Dune2Image("mouse.shp", -1);
	if (cursorimg == NULL)
		logger->error("Couldn't load cursor image\n");

	my_cursor = cursorimg->getImage(0);

	if (my_cursor == 0)
	{
		logger->error("Couldn't get cursor sld surface\n");
	}

	// Free cursorimg
	if (cursorimg != 0){
		delete cursorimg;
	}
	cursorimg = NULL;

	SDL_SetColors(my_cursor, SHPBase::getPalette(0), 0, 256);
	SDL_SetColorKey(my_cursor, SDL_SRCCOLORKEY, 0);

	int button_width = int(240 * scale_factor);
	int button_height = int(25 * scale_factor);
	int button_space = int(10 * scale_factor);
	int ButtonXpos	= display->w/2 - button_width/2;
	int ButtonYpos	= int(200 * scale_factor);


	// Set pos and label (the string 17 is "new game" but located)
	StartNewGameButton.CreateSurface(strFile->getString(17), ButtonXpos, ButtonYpos, button_width, button_height);
	// Add a space
	ButtonYpos += button_height + button_space;


	// Set pos and label (the string 479 is "internet game" but located)
	InternetGameButton.CreateSurface(strFile->getString(479), ButtonXpos, ButtonYpos, button_width, button_height);
	// Add a space
	ButtonYpos += button_height + button_space;


	// (the string 35 is "Load game" but located)
	LoadMissionButton.CreateSurface(strFile->getString(35), ButtonXpos, ButtonYpos, button_width, button_height);
	// Add a space
	ButtonYpos += button_height + button_space;;


	// Set pos and label (the string 154 is "multiplayer game" but located)
	// Set pos and label (the string 482 is "Escamouche game" but located)
	MultiplayerGameButton.CreateSurface(strFile->getString(482), ButtonXpos, ButtonYpos, button_width, button_height);
	// Add a space
	ButtonYpos += button_height + button_space;


	// Create the "Intro & Sneak Peek" button
	IntroAndSneakPeekButton = new Button();
	// (the string 18 is "Intro & Sneak Peek" but located)
	IntroAndSneakPeekButton->CreateSurface(strFile->getString(18), ButtonXpos, ButtonYpos, button_width, button_height);
	// Add a space
	ButtonYpos += button_height + button_space;


	// (the string 46 is "quit game" but located)
	ExitGameButton.CreateSurface(strFile->getString(46), ButtonXpos, ButtonYpos, button_width, button_height);



	//
	// Setup the first popup window
	//
	MissionMenu1.SetupWindow(190, 180, 280, 130);

	MultiPlayerMenu.SetupWindow(0, 0, 640, 400);


	//
	// Setup the buttons for selecting the local player color
	//
	ButtonYpos = 35;
	ButtonXpos = 400;
	ButtonColGreece.setcolor (SHPBase::getColour(display->format, 1, 85));
	ButtonColGreece.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColGreece.SetDrawingWindow (&MultiPlayerMenu);
	ButtonXpos += 25;
	ButtonColUssr.setcolor (SHPBase::getColour(display->format, 2, 85));
	ButtonColUssr.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColUssr.SetDrawingWindow (&MultiPlayerMenu);
	ButtonXpos += 25;
	ButtonColUk.setcolor (SHPBase::getColour(display->format, 3, 85));
	ButtonColUk.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColUk.SetDrawingWindow (&MultiPlayerMenu);
	ButtonXpos += 25;
	ButtonColSpain.setcolor (SHPBase::getColour(display->format, 4, 85));
	ButtonColSpain.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColSpain.SetDrawingWindow (&MultiPlayerMenu);
	ButtonXpos += 25;
	ButtonColItaly.setcolor (SHPBase::getColour(display->format, 5, 85));
	ButtonColItaly.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColItaly.SetDrawingWindow (&MultiPlayerMenu);
	ButtonXpos += 25;
	ButtonColGermany.setcolor (SHPBase::getColour(display->format, 6, 85));
	ButtonColGermany.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColGermany.SetDrawingWindow (&MultiPlayerMenu);
	ButtonXpos += 25;
	ButtonColFranse.setcolor (SHPBase::getColour(display->format, 7, 85));
	ButtonColFranse.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColFranse.SetDrawingWindow (&MultiPlayerMenu);
	ButtonXpos += 25;
	ButtonColTurkey.setcolor (SHPBase::getColour(display->format, 8, 85));
	ButtonColTurkey.CreateSurface(" ", ButtonXpos, ButtonYpos, 25, 25 );
	ButtonColTurkey.SetDrawingWindow (&MultiPlayerMenu);

	ButtonColUssr.setButtonState (2);

	//
	// Draw the buttons on the new window..
	//
	ButtonXpos = 30;
	ButtonYpos = 70;
	AlliesMissionButton.SetDrawingWindow (&MissionMenu1);
	AlliesMissionButton.CreateSurface(strFile->getString(473), ButtonXpos, ButtonYpos, 90, 25 );
	ButtonXpos = 155;
	RussianMissionButton.SetDrawingWindow (&MissionMenu1);
	RussianMissionButton.CreateSurface(strFile->getString(474), ButtonXpos, ButtonYpos, 90, 25 );

	Oke.SetDrawingWindow (&MultiPlayerMenu);
	Oke.CreateSurface(strFile->getString(23), 50, 350, 90, 25);
	//Oke.CreateSurface("Oke", 50, 350, 90, 25);
	Cancel.SetDrawingWindow (&MultiPlayerMenu);
	Cancel.CreateSurface(strFile->getString(19), 500, 350, 90, 25);
	//Cancel.CreateSurface("Cancel", 500, 350, 90, 25);

	// Initialize the labels to be drawn to the MultiPlayerMenu
	YourColor.SetDrawingWindow(&MultiPlayerMenu);
	YourName.SetDrawingWindow(&MultiPlayerMenu);
	YourSide.SetDrawingWindow(&MultiPlayerMenu);
	YourColor.setText("Your color");
	YourName.setText("Your name");
	YourSide.setText("Your side");
	YourColor.setColor(Fcolor);
	YourName.setColor(Fcolor);
	YourSide.setColor(Fcolor);
	YourColor.SetFont("grad6fnt.fnt");
	YourName.SetFont("grad6fnt.fnt");
	YourSide.SetFont("grad6fnt.fnt");
	YourColor.Draw(400, 15);
	YourName.Draw( 50, 15);
	YourSide.Draw(220, 15);


	// Init the units label
	Ypos = 260;
	UnitsLabel.SetFont("grad6fnt.fnt");
	UnitsLabel.setText("Unit Count:");
	UnitsLabel.setColor(Fcolor);
	UnitsLabel.SetDrawingWindow(&MultiPlayerMenu);
	UnitsLabel.Draw(78,Ypos);

	// Init the numb units progress bar
	NumbUnitsProgressbar.SetDrawingWindow(&MultiPlayerMenu);
	NumbUnitsProgressbar.setNumbSteps (10);
	NumbUnitsProgressbar.setCurStep (5);
	NumbUnitsProgressbar.Draw(170,Ypos+2);

	// Init the numb units label
	NumbUnitsLabel.SetFont("grad6fnt.fnt");
	NumbUnitsLabel.setText ("6");
	NumbUnitsLabel.setColor(Fcolor);
	NumbUnitsLabel.SetDrawingWindow(&MultiPlayerMenu);
	NumbUnitsLabel.Draw(225,Ypos);


	// Init the tech level label
	Ypos += 14;
	TechlevelLabel.SetFont("grad6fnt.fnt");
	TechlevelLabel.setText("Tech Level:");
	TechlevelLabel.setColor(Fcolor);
	TechlevelLabel.SetDrawingWindow(&MultiPlayerMenu);
	TechlevelLabel.Draw(76,Ypos);

	// Init the tech level progress bar
	TechLevelProgressbar.SetDrawingWindow(&MultiPlayerMenu);
	TechLevelProgressbar.setNumbSteps (10);
	TechLevelProgressbar.setCurStep (10);
	TechLevelProgressbar.Draw(170,Ypos+2);

	// Init the numb units label
	NumbTechlevelLabel.SetFont("grad6fnt.fnt");
	NumbTechlevelLabel.setText ("10");
	NumbTechlevelLabel.setColor(Fcolor);
	NumbTechlevelLabel.SetDrawingWindow(&MultiPlayerMenu);
	NumbTechlevelLabel.Draw(225,Ypos);


	// Set the initial start money to 10000
	pc::Config.startMoney = 10000;

	// Init the credits label
	Ypos += 14;
	CreditsLabel.SetFont("grad6fnt.fnt");
	CreditsLabel.setText("Credits:");
	CreditsLabel.setColor(Fcolor);
	CreditsLabel.SetDrawingWindow(&MultiPlayerMenu);
	CreditsLabel.Draw(106,Ypos);

	// Init the tech level progress bar
	CreditsProgressbar.SetDrawingWindow(&MultiPlayerMenu);
	CreditsProgressbar.setNumbSteps (10);
	CreditsProgressbar.setCurStep (10);
	CreditsProgressbar.Draw(170,Ypos+2);

	// Init the numb credits label
	NumbCreditsLabel.SetFont("grad6fnt.fnt");
	NumbCreditsLabel.setText("10000");
	NumbCreditsLabel.setColor(Fcolor);
	NumbCreditsLabel.SetDrawingWindow(&MultiPlayerMenu);
	NumbCreditsLabel.Draw(225,Ypos);


	// Init the ai players label (Label displayed in front of the progress bar)
	Ypos += 14;
	this->AiPlayersLabel.SetFont("grad6fnt.fnt");
	AiPlayersLabel.setText("AI players:");
	AiPlayersLabel.setColor(Fcolor);
	AiPlayersLabel.SetDrawingWindow(&MultiPlayerMenu);
	AiPlayersLabel.Draw(80,Ypos);

	// Init the ai players progress bar
	NumbAiProgressbar.SetDrawingWindow(&MultiPlayerMenu);
	NumbAiProgressbar.setNumbSteps (3);
	NumbAiProgressbar.Draw(170,Ypos+2);

	// Init the numb ai players label (number displayed behind the progress bar)
	NumbAiPlayersLabel.SetFont("grad6fnt.fnt");
	NumbAiPlayersLabel.setText("1");
	NumbAiPlayersLabel.setColor(Fcolor);
	NumbAiPlayersLabel.SetDrawingWindow(&MultiPlayerMenu);
	NumbAiPlayersLabel.Draw(225, Ypos);

	numb_ai_players = 1;


//Font DebugTest ("12metfnt.fnt");

//	DebugTest.drawCHAR('G');

	// Load mission maps data
	missionList = new MissionMapsClass();

	// Load multiPlayer maps
	multiPlayerMaps = new MultiPlayerMaps();

	// Build Lisbox with mission multi
	listBox = new ListboxClass();

	// Set that the user don't want quit the game
	this->quit = false;
}

/**
 *
 */
Menu::~Menu()
{

	if (win95_logo != NULL){
		delete win95_logo;
	}
	if (dos_logo != NULL){
		delete dos_logo;
	}
	if (my_cursor != NULL){
		SDL_FreeSurface(my_cursor);
	}

	// Free List of missions (not needed)
	//delete missionList;

	// Free Listbox component that contains list of multi-player missions
	delete listBox;
}

void Menu::DrawMenuBackground()
{
	SDL_Rect dest;

	dest.x = 0;
	dest.y = 0;
	dest.w = display->w;
	dest.h = display->h;

	//Black the screen
	static Uint32 blackpix = SDL_MapRGB(display->format, 0, 0, 0);
	SDL_FillRect(display, &dest, blackpix);

	// Draw the menu background pixture
	if (SDLlogo != 0) {
		dest.x = (display->w - SDLlogo->w)/2;
		dest.y = (display->h - SDLlogo->h)/2;

		//printf ("dest.x = %i, dest.y = %i, logo.w = %i, logo.h = %i\n", dest.x, dest.y, SDLlogo->w, SDLlogo->h);
		SDL_BlitSurface(SDLlogo, 0, display, &dest);
	}
}

/**
 * Draw the mouse pointer in the screen
 */
void Menu::DrawMousePointer()
{
	int mx;
	int my;
	SDL_Rect dest;

	// Get real coordinates
	SDL_GetMouseState(&mx, &my);

	// Actually draw the mouse
	dest.x = mx;
	dest.y = my;
	if (dest.x < 0){
		dest.x = 0;
	}
	if (dest.y < 0){
		dest.y = 0;
	}
	dest.w = my_cursor->w;
	dest.h = my_cursor->h;

	// Blit the surface
	SDL_BlitSurface(my_cursor, 0, display, &dest);
}

/**
 * Draw the button under the window
 */
void Menu::DrawMainMenuButtons()
{
//	if (StartNewGameButton.NeedRedraw())
		StartNewGameButton.drawbutton();
//	if (InternetGameButton.NeedRedraw())
		InternetGameButton.drawbutton();
//	if (LoadMissionButton.NeedRedraw())
		LoadMissionButton.drawbutton();
//	if (MultiplayerGameButton.NeedRedraw())
		MultiplayerGameButton.drawbutton();
//	if (IntroAndSneakPeekButton.NeedRedraw())
		IntroAndSneakPeekButton->drawbutton();
//	if (ExitGameButton.NeedRedraw())
		ExitGameButton.drawbutton();

}

/**
 * Called every frame !!!!!!!!!!!!!!
 * don't override with heavy call !!!
 */
void Menu::HandleInput()
{
	SDL_Event	event;
	string		SelectedMap;

#if 0
//	if (MenuState == MENUSTATE_MULTIPLAYER_1)
		while ( !SDL_PollEvent(&event) && !isDone ) {
			SDL_Delay (5);
		}
 //       if ( !isDone && SDL_WaitEvent(&event)  ) {
			if (!isDone){
#else
	if ( !done && SDL_WaitEvent(&event)  ) {
		do {
#endif
		// Handle the buttons for the different menus
		switch (MenuState){
			case 1:
				StartNewGameButton.handleMouseEvent(event);
				InternetGameButton.handleMouseEvent(event);
				LoadMissionButton.handleMouseEvent(event);
				MultiplayerGameButton.handleMouseEvent(event);
				IntroAndSneakPeekButton->handleMouseEvent(event);
				ExitGameButton.handleMouseEvent(event);
				break;
			case 2:
				RussianMissionButton.handleMouseEvent(event);
				AlliesMissionButton.handleMouseEvent(event);
				break;
			case 3:
				this->PlayerName.HandleInput(event);
				this->SideSelection.HandleInput(event);
				this->listBox->HandleInput(event);
				Oke.handleMouseEvent(event);
				Cancel.handleMouseEvent(event);
				break;
		}
		// Handle the different mouse and keyboard events
		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (MenuState){
					case 3:
						if (ButtonColGreece.MouseOver()){
							this->ResetSideColorButtonStates();
							ButtonColGreece.setButtonState(2);
						}else if (ButtonColUssr.MouseOver()){
							this->ResetSideColorButtonStates();
							ButtonColUssr.setButtonState (2);
						}else if (ButtonColUk.MouseOver()){
							this->ResetSideColorButtonStates ();
							ButtonColUk.setButtonState(2);
						}else if (ButtonColSpain.MouseOver()){
							this->ResetSideColorButtonStates ();
							ButtonColSpain.setButtonState (2);
						}else if (ButtonColItaly.MouseOver()){
							this->ResetSideColorButtonStates ();
							ButtonColItaly.setButtonState (2);
						}else if (ButtonColGermany.MouseOver()){
							this->ResetSideColorButtonStates ();
							ButtonColGermany.setButtonState (2);
						}else if (ButtonColFranse.MouseOver()){
							this->ResetSideColorButtonStates ();
							ButtonColFranse.setButtonState (2);
						}else if (ButtonColTurkey.MouseOver()){
							this->ResetSideColorButtonStates ();
							ButtonColTurkey.setButtonState (2);
						}
						if (NumbAiProgressbar.HandleMouseClick())
						{
							int Numb = NumbAiProgressbar.getCurStep ();
							Numb++;
							std::stringstream NumbStr;
							NumbStr << Numb;
							NumbAiPlayersLabel.setText(NumbStr.str().c_str());
							NumbAiPlayersLabel.Redraw();
							numb_ai_players = Numb;
						}
						if (CreditsProgressbar.HandleMouseClick()){
							int Numb = CreditsProgressbar.getCurStep ();
							Numb = 5000 + Numb * 500;
							std::stringstream NumbStr;
							NumbStr << Numb;
							NumbCreditsLabel.setText(NumbStr.str().c_str());
							NumbCreditsLabel.Redraw();
							pc::Config.startMoney = Numb;
						}
						break;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT){
					switch (MenuState){
						case 1:
							//printf ("Left mouse -->\n");
							if (StartNewGameButton.MouseOver()){
								MenuState = 2;
								//isDone=true;
							}else if (ExitGameButton.MouseOver()){
								done=true;
								this->quit = true;
							}else if (MultiplayerGameButton.MouseOver()){
								MenuState = 3;
								Oke.SetDrawingWindow (&MultiPlayerMenu);
								Cancel.SetDrawingWindow (&MultiPlayerMenu);
							}else if (IntroAndSneakPeekButton->MouseOver())
							{
								/*try
								{
									VQAMovie mov("PROLOG");
									mov.play();
								} catch (...)
								{}*/
								done = true;
								this->prolog = true;
							}
							break;
						case 2:
							if (RussianMissionButton.MouseOver()){
								printf ("RussianMission selected\n");
								pc::Config.UseFogOfWar	= true;
								SelectedMap = missionList->getNodMissionMap(0);
								pc::Config.mapname	= SelectedMap;//"SCG01EA";	// First nod mission
								pc::Config.gamemode	= GAME_MODE_SINGLE_PLAYER;	// single player (missions)
								pc::Config.mside	= "nod"; 	//"badguy";
								pc::Config.cside	= "multi";
								pc::Config.side_colour	= "red";
								pc::Config.pause = false;
								pc::Config.quit_mission = false;
								done=true;
							}else if (AlliesMissionButton.MouseOver()){
								printf ("AlliesMission selected\n");
								pc::Config.UseFogOfWar	= true;
								SelectedMap = missionList->getGdiMissionMap(0);
								pc::Config.mapname	= SelectedMap;	//"SCU01EA";	// First allied mission??
								pc::Config.gamemode	= GAME_MODE_SINGLE_PLAYER;		// single player (missions)
								pc::Config.mside	= "gdi"; //"goodguy";
								pc::Config.cside	= "multi";
								pc::Config.side_colour = "yellow";
								pc::Config.pause = false;
								pc::Config.quit_mission = false;
								done = true;
							}
							break;
						case 3:
							//if (ListBox.MouseOverArrowDown ()){
							//	ListBox.ScrollDown();
							//}
							if (Cancel.MouseOver()){
								MenuState = 1;
							}


							// Supported colors
							//"yellow"
							//"red"
							//"blue"
							//"orange"
							//"green"
							//"turquoise"
							if (Oke.MouseOver()){
								printf ("Selected map nr = %i\n", this->listBox->GetSelectionIndex ());
								multiPlayerMaps->getMapName(this->listBox->GetSelectionIndex(), pc::Config.mapname);
								// Printf ("For some reason the game crashes when skirmisch is selected??
								pc::Config.UseFogOfWar	= true;
								pc::Config.playernum	= 1;	// Local player player number
								pc::Config.gamemode 	= 1;	// Skermisch
								pc::Config.totalplayers = 2;
								if (SideSelection.selected() == 0 || SideSelection.selected() == 2)
									pc::Config.mside	= "nod";
								else
									pc::Config.mside	= "gdi";
								pc::Config.cside		= "goodguy";
								pc::Config.side_colour		= "red";
								//pc::Config.side_colour	= "blue"; //"orange"; //"red";
								pc::Config.nick			= PlayerName.getText();
								pc::Config.pause		= false;
								pc::Config.quit_mission		= false;
								pc::Config.totalplayers		= 1 + numb_ai_players;
								//pc::Config.numb_ai_players	= 2;
								done=true;
								printf ("Mapname = %s\n", pc::Config.mapname.c_str());

								if (ButtonColGreece.getButtonState () == 2){
									pc::Config.side_colour	= "blue";
								}else if (ButtonColUssr.getButtonState () == 2){
									pc::Config.side_colour	= "red";
								}else if (ButtonColUk.getButtonState () == 2){
									pc::Config.side_colour	= "green";
								}else if (ButtonColSpain.getButtonState () == 2){
									pc::Config.side_colour	= "yellow";
								}else if (ButtonColItaly.getButtonState () == 2){
									pc::Config.side_colour	= "orange";
								}else if (ButtonColGermany.getButtonState () == 2){
									pc::Config.side_colour	= "gray";
								}else if (ButtonColFranse.getButtonState () == 2){
									pc::Config.side_colour	= "turquoise";
								}else if (ButtonColTurkey.getButtonState () == 2){
									pc::Config.side_colour	= "darkred";
								}

							}
							break;
					}
				}
				break;
        		case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						printf ("Escape key\n");
						if (MenuState == 2 || MenuState == 3){
							MenuState = 1;
						}else{
							done = true;
							this->quit = true;
						}
						break;
					default:
						break;
				}
				break;
		}

	} while ( !done && SDL_PollEvent(&event) );
	}
}

int Menu::HandleMenu()
{
	int	mx; // Mouse X coords
	int my; // Mouse Y coords
	int old_mx = 0;
	int	old_my = 100;
	string		TmpString;
	int			i;
	TCheckBox	CheckBox;
    StringTableFile* strFile;

    // Load located strings
    strFile = new StringTableFile("conquer.eng");

	// Setup the multi player listbox with all the multiplayer maps
	i = 0;
	while (multiPlayerMaps->getMapDescription(i, TmpString)){
		this->listBox->AddString(TmpString);
		i++;
	}

	// should be: Russia, England, Ukraine, Germany, France
	SideSelection.AddEntry(strFile->getString(372)); //"Russia"
	SideSelection.AddEntry(strFile->getString(369)); //"England");
	SideSelection.AddEntry(strFile->getString(373)); //"Ukraine");
	SideSelection.AddEntry(strFile->getString(370)); //"Germany");
	SideSelection.AddEntry(strFile->getString(375)); //"France");

    // Free strings
    delete strFile;

    // Start with no done
    done = false;

    quit = false;
    prolog = false;

    //
	while( !done )
	{
		// get mouse coords
		SDL_GetMouseState(&mx, &my);

		// Draw the background pixture
		this->DrawMenuBackground();
		// Draw buttons
		this->DrawMainMenuButtons();


		if (MenuState == 2){
			// first draw the buttons on the window,
			// than draw the window to the screen
			RussianMissionButton.drawbutton();
			AlliesMissionButton.drawbutton();
			MissionMenu1.DrawWindow();
		}
		else if (MenuState == 3)
		{
			// first draw the buttons on the window,
			// than draw the window to the screen
			Oke.drawbutton ();
			Cancel.drawbutton ();
			ButtonColGreece.drawbutton ();
			ButtonColUssr.drawbutton ();
			ButtonColUk.drawbutton ();
			ButtonColSpain.drawbutton ();
			ButtonColItaly.drawbutton ();
			ButtonColGermany.drawbutton ();
			ButtonColFranse.drawbutton ();
			ButtonColTurkey.drawbutton ();
			MultiPlayerMenu.DrawWindow();
			PlayerName.Draw(50,35);
			this->listBox->DrawListBox(130, 80);
			SideSelection.Draw (220,35);
		}

		this->DrawMousePointer ();

		SDL_Flip(display);

		old_mx = mx;
		old_my = my;

		this->HandleInput();
		//SDL_Delay(15);
    }

    return 0;
}

void Menu::loadPal(const string& paln, SDL_Color *palette)
{
	VFile *palfile;
	int i;

	// string palname = missionData.theater;
	std::string palname = paln;

	logger->debug("Loading pallete: %s\n", palname.c_str());

	if (palname.length() > 8) {
		palname.insert(8, ".PAL");
	} else {
		palname += ".PAL";
	}

	// Seek the palette file in the mix
	palfile = VFSUtils::VFS_Open(palname.c_str());

	if (palfile == NULL) {
		logger->error("%s line %i: Unable to open palette file (\"%s\").\n", __FILE__, __LINE__, palname.c_str());
		return;
	}

	// Load the palette
	for (i = 0; i < 256; i++) {
		palfile->readByte(&palette[i].r, 1);
		palfile->readByte(&palette[i].g, 1);
		palfile->readByte(&palette[i].b, 1);
		palette[i].r <<= 2;
		palette[i].g <<= 2;
		palette[i].b <<= 2;
	}


	VFSUtils::VFS_Close(palfile);
}

void Menu::ResetSideColorButtonStates()
{
	ButtonColGreece.setButtonState (1);
	ButtonColUssr.setButtonState (1);
	ButtonColUk.setButtonState (1);
	ButtonColSpain.setButtonState (1);
	ButtonColItaly.setButtonState (1);
	ButtonColGermany.setButtonState (1);
	ButtonColFranse.setButtonState (1);
	ButtonColTurkey.setButtonState (1);
}

/**
 * @return <code>true</code> if the user selected an option
 */
bool Menu::isDone() const
{
	return done;
}

/**
 * @return <code>true</code> if the user want quit the game
 */
bool Menu::isQuit() const
{
	return quit;
}

/**
 * @return <code>true</code> if the user want to see the introduction movie the game
 */
bool Menu::isProlog() const
{
	return prolog;
}
