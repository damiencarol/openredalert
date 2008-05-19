// PauseMenu.cpp
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

#include "PauseMenu.h"

#include "SDL/SDL_video.h"
#include "SDL/SDL_events.h"

#include "Button.h"
#include "RA_Label.h"
#include "RA_WindowClass.h"
#include "TCheckBox.h"
#include "game/Player.h"
#include "include/PlayerPool.h"
#include "include/config.h"
#include "video/GraphicsEngine.h"
#include "include/Logger.h"
#include "audio/SoundEngine.h"
#include "include/sdllayer.h"
#include "video/Dune2Image.h"

namespace p {
	extern PlayerPool* ppool;
}
namespace pc {
	extern ConfigType Config;
    extern GraphicsEngine * gfxeng;
}
extern Logger * logger;

PauseMenu::PauseMenu()
{
    //:MenuFont("type.fnt"), VersionFont ("grad6fnt.fnt")
    
    // Create the windows
    PauseWindow = new RA_WindowClass();
    
    // Create "Option" button
    OptionsButton = new Button();
	// Create "Exit" button
	ExitButton = new Button();
	// Create "continue" button
	ContinueButton = new Button();
	
	// Draw the buttons on the new window..
    OptionsButton->SetDrawingWindow(PauseWindow);
	OptionsButton->CreateSurface("game options", 120, 110, 180, 22);	
	ExitButton->SetDrawingWindow(PauseWindow);
	ExitButton->CreateSurface("abort Mission", 120, 145, 180, 22);	
	ContinueButton->SetDrawingWindow(PauseWindow);
	ContinueButton->CreateSurface("Resume Mission", 35, 200, 180, 22);
			
	// Setup the font color
	Font_color.r = 255;
	Font_color.g = 255;
	Font_color.b = 255;

	// set reference to the screen surface
	display = pc::gfxeng->get_SDL_ScreenSurface();

	// Load the cursor
    cursorimg = new Dune2Image("mouse.shp", -1);
	if (cursorimg == 0){
		logger->error("Couldn't load cursor image\n");
    }
	my_cursor = cursorimg->getImage(0);

	// Create the window
	PauseWindow = new RA_WindowClass();
	// Setup the first popup window
    PauseWindow->SetupWindow(130, 90, 430, 250);


	// Create the menu Label
	MenuLabel = new RA_Label();
	// set drawing params
	MenuLabel->Draw("Options", PauseWindow->GetWindowSurface(), Font_color, 175, 30) ;
	
	// Create the version Label
	VersionLabel = new RA_Label();
	// set drawing params
	VersionLabel->Draw(VERSION.c_str(), PauseWindow->GetWindowSurface(), Font_color, 290, 210) ;

	// ???
	for (int x = 177; x < 243; x++){
		SDLLayer::set_pixel(PauseWindow->GetWindowSurface(), 0xffffff, x, 45);
    }
}

/**
 */
PauseMenu::~PauseMenu()
{
	// Free "Option" button
	delete OptionsButton;	
	// Free "Exit" button
	delete ExitButton;
	// Free "continue" button
	delete ContinueButton;
	
	// Free the menu Label
	delete MenuLabel;
	// Free the version Label
	delete VersionLabel;
	
	// Free the windows
	delete PauseWindow;
	
	// Free the cursor surface
	if (my_cursor != NULL){
		SDL_FreeSurface(my_cursor);
    }
}

/**
 */
int PauseMenu::HandleMenu()
{
	int mx;
	int my;
	int old_mx = 0;
	int old_my = 100;
	string		TmpString;
	TCheckBox	CheckBox;	
	
	// Stop all sounds
	pc::sfxeng->PauseLoopedSound(-1);

	// Pause the build queue
	for (int i = 0; i < p::ppool->getNumPlayers(); i++){
		p::ppool->getPlayer(i)->pauseBuilding();
    }

	// Get the mouse coordinates
	SDL_GetMouseState(&mx, &my);

	// Handles inputs
	this->HandleInput();

	// Keep rendering the scene as background
	pc::gfxeng->renderScene(false);

	// Draw the buttons on the window
	OptionsButton->drawbutton();
	ExitButton->drawbutton();
	ContinueButton->drawbutton();

	// Draw the window (whith the buttons on it) to the screen
	PauseWindow->DrawWindow();

	// Draw the cursor
	this->DrawMousePointer();

	SDL_Flip(display);

	// Save old coords
	old_mx = mx;
	old_my = my;

	// return success
    return 0;
}

void PauseMenu::HandleInput()
{
	SDL_Event event;

	if (SDL_WaitEvent(&event)){

		// Pass events on to menu items
		if (ExitButton->handleMouseEvent(event)){
			DrawMousePointer();
        }
		if (ContinueButton->handleMouseEvent(event)){
			DrawMousePointer();
        }

		// Handle the diffirent mouse and keyboard effents
		switch (event.type){

			case SDL_MOUSEBUTTONDOWN:
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT){
					if (ExitButton->MouseOver()){
						// exit to main menu
						pc::Config.quit_mission = true;
						pc::Config.pause = false;

						// Stop all the music
						pc::sfxeng->StopMusic();
						pc::sfxeng->StopLoopedSound(-1);
						//pc::sfxeng->ResumeLoopedSound(-1);

						for (int i = 0; i < p::ppool->getNumPlayers(); i++)
							p::ppool->getPlayer(i)->resumeBuilding();
					}
					if (ContinueButton->MouseOver()){
						// Continue the game
						pc::Config.pause = false;
						pc::sfxeng->ResumeLoopedSound(-1);
						for (int i = 0; i < p::ppool->getNumPlayers(); i++)
							p::ppool->getPlayer(i)->resumeBuilding();
					}
				}
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE){
					pc::Config.pause = false;
				}
				break;
		}
	} else {
		printf ("Error");
	}
}

/**
 * Draw the mouse pointer in the Pause menu
 */
void PauseMenu::DrawMousePointer()
{
	SDL_Rect dest;
	int mx, my;

	// Get the mouse coord
	SDL_GetMouseState(&mx, &my);

	// create dest rect with coordinates
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

	// Actually draw the mouse
	SDL_BlitSurface(my_cursor, 0, display, &dest);
}
