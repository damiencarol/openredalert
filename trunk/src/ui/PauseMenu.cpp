#include "PauseMenu.h"

#include "SDL/SDL_video.h"
#include "SDL/SDL_events.h"

#include "RA_Label.h"
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

	// Setup the font color
	Font_color.r = 255;
	Font_color.g = 255;
	Font_color.b = 255;

	display = pc::gfxeng->get_SDL_ScreenSurface ();

	// Load the cursor
    cursorimg = new Dune2Image("mouse.shp", -1);
	if (cursorimg == NULL){
		logger->error("Couldn't load cursor image\n");
    }
	my_cursor = cursorimg->getImage(0);

	// Setup the first popup window
    PauseWindow.SetupWindow(130, 90, 430, 250);


	// Draw the buttons on the new window..
    OptionsButton.SetDrawingWindow (&PauseWindow);
	OptionsButton.CreateSurface("game options", 120, 110, 180, 22 );
	ExitButton.SetDrawingWindow (&PauseWindow);
	ExitButton.CreateSurface("abort Mission", 120, 145, 180, 22 );
	ContinueButton.SetDrawingWindow (&PauseWindow);
	ContinueButton.CreateSurface("Resume Mission", 35, 200, 180, 22 );

	MenuLabel.Draw("Options", PauseWindow.GetWindowSurface (), Font_color, 175, 30) ;
	VersionLabel.Draw(VERSION.c_str(), PauseWindow.GetWindowSurface (), Font_color, 290, 210) ;

	// ???
	for (int x = 177; x < 243; x++){
		SDLLayer:: set_pixel( PauseWindow.GetWindowSurface (), 0xffffff, x, 45 );
    }
}

PauseMenu::~PauseMenu()
{
	// Free the cursor surface
	if (my_cursor != NULL){
		SDL_FreeSurface(my_cursor);
    }
}

int PauseMenu::HandleMenu(void)
{
	int mx,	my,	old_mx = 0,
				old_my = 100;
	std::string		TmpString;
	TCheckBox		CheckBox;
	
	
	//
	pc::sfxeng->PauseLoopedSound(-1);

	for (int i = 0; i < p::ppool->getNumPlayers(); i++){
		p::ppool->getPlayer(i)->pauseBuilding();
    }

	SDL_GetMouseState(&mx, &my);

	this->HandleInput();

	// Keep rendering the scene as background
	pc::gfxeng->renderScene(false);

	// Draw the buttons on the window
	OptionsButton.drawbutton();
	ExitButton.drawbutton();
	ContinueButton.drawbutton();

	// Draw the window (whith the buttons on it) to the screen
	PauseWindow.DrawWindow();

	this->DrawMousePointer();

	SDL_Flip(display);

	old_mx = mx;
	old_my = my;

    return 0;
}

void PauseMenu::HandleInput (void)
{
	SDL_Event event;

	if (SDL_WaitEvent(&event)){

		// Pass events on to diffirent menu items
		if (ExitButton.handleMouseEvent (event)){
			DrawMousePointer ();
        }
		if (ContinueButton.handleMouseEvent (event)){
			DrawMousePointer ();
        }

		// Handle the diffirent mouse and keyboard effents
		switch (event.type){

			case SDL_MOUSEBUTTONDOWN:
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT){
					if (ExitButton.MouseOver()){
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
					if (ContinueButton.MouseOver()){
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
	SDL_BlitSurface(my_cursor, NULL, display, &dest);
}
