// RaWindow.h
// 1.0

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

#ifndef RAWINDOW_H
#define RAWINDOW_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class Font;
class RA_Label;

/**
 * Window in the game
 * @author Joris Meijer (FreeRA)
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r???
 */
class RaWindow
{
public:
	RaWindow();
	~RaWindow();
	
	void setPalette(Uint8 pal);
	void DrawRaBackground();
	void SolidFill(Uint8 red, Uint8 green, Uint8 blue);
	void ResizeWindow(int Width, int Heigth);
	void GetWindowPosition(int *Xpos, int *Ypos);
	void ChangeWindowPosition(int Xpos, int Ypos);
	SDL_Surface* GetWindowSurface();
	void SetupWindow(int Xpos, int Ypos, int Width, int Heigth);
	void DrawWindow();

private:
	SDL_Surface* ReadShpImage(char *Name, int ImageNumb, Uint8 palnum = 1);
	
	SDL_Surface* DisplaySurface;
	SDL_Surface* WindowSurface;
	SDL_Rect SizeAndPosition;

	Uint32 Background;
	Uint32 LeftBorder;
	Uint32 RightBorder;
	Uint32 TopBorder;
	Uint32 BottomBorder;
	Uint32 Corner;
	Uint8 PalNr;
};

#endif //RAWINDOW_H
