// Renderer.cpp
// 1.4

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

#include "Renderer.h"

#include "SDL/SDL_error.h"
#include "SDL/SDL_video.h"

#include "include/sdllayer.h"
#include "include/config.h"
#include "include/Logger.h"
#include "VideoError.h"

extern Logger * logger;

/**
 * Constructor
 */
Renderer::Renderer()
{
	screen = 0;
	icon = 0;
}

/**
 * Destructor
 */
Renderer::~Renderer()
{
	if (icon != 0)
	{
		SDL_FreeSurface(icon);
	}
	icon = 0;
}

void Renderer::DrawRectangle(Uint32 color, Uint16 x, Uint16 y, Uint16 width,
		Uint16 height, Uint8 lnpx)
{
	SDLLayer::draw_rectangle(screen, color, x, y, width, height, lnpx);
}

void Renderer::DrawSolidRectangle(Uint32 color, Uint16 x, Uint16 y,
		Uint16 width, Uint16 height)
{
	SDLLayer::draw_solidrectangle(screen, color, x, y, width, height);

}

void Renderer::InitializeScreen()
{
	ConfigType config;
	Uint16 width;
	Uint16 height;

	// Get the config and screen dimensions
	config = getConfig();
	width = config.width;
	height = config.height;

#ifndef __MORPHOS__
	// Set the caption icon
	icon = SDL_LoadBMP("data/gfx/icon.bmp");
	if (icon != 0)
	{
		SDL_WM_SetIcon(icon, 0);
	}
#endif
	// Setup the screen
	screen = SDL_SetVideoMode(width, height, config.bpp, config.videoflags);
	if (screen == 0)
	{
		logger->error("Unable to set %dx%d video: %s\n", width, height,	SDL_GetError());
		throw VideoError("Unable to set video mode");
	}
}

SDL_Surface * Renderer::getScreen()
{
	return screen;
}
