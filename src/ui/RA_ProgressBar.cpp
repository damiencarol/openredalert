// RA_ProgressBar.cpp
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

#include "RA_ProgressBar.h"

#include <math.h>
#include <string>

#include "SDL/SDL_mouse.h"
#include "SDL/SDL_video.h"

#include "video/GraphicsEngine.h"
#include "RaWindow.h"

using std::string;

namespace pc {
	extern GraphicsEngine * gfxeng;
}

#ifdef __MORPHOS__
double round(double x) { return (x > 0) ? floor(x + 0.5) : ceil(x - 0.5); }
#endif


RA_ProgressBar::RA_ProgressBar()
{
	_windowToDrawOn		= NULL;
	_displaySurface		= NULL;
	_progressSurface	= NULL;

	_sizeAndPosition.x = 0;
	_sizeAndPosition.y = 0;
	_sizeAndPosition.h = 13;
	_sizeAndPosition.w = 50;

	_recreate = true;
	_position = 0;
	_maxSteps = -1;
	_curStep = 0;


	// Initialize the surfaces
	_displaySurface			= pc::gfxeng->get_SDL_ScreenSurface();
	_progressBarColor		= SDL_MapRGB(_displaySurface->format, 172, 141, 49);
	_progressBackgroundColor	= SDL_MapRGB(_displaySurface->format, 98, 0, 0);
	_edgeLightColor			= SDL_MapRGB(_displaySurface->format, 205, 0, 0);
	_edgeDarkColor			= SDL_MapRGB(_displaySurface->format, 65, 0, 0);
}

RA_ProgressBar::~RA_ProgressBar()
{

}

void RA_ProgressBar::SetDrawingSurface(SDL_Surface *DwgSurface)
{
	if (_displaySurface != DwgSurface){
		_windowToDrawOn = NULL;
		_displaySurface = DwgSurface;
		_recreate = true;
	}
}

void RA_ProgressBar::SetDrawingWindow(RaWindow* window)
{
	// Check if it's not NULL
	if (window != 0)
	{
		_windowToDrawOn = window;
	}
}

bool RA_ProgressBar::MouseOver()
{
	int mx, my;
	int WinXpos = 0, WinYpos = 0;

	SDL_GetMouseState(&mx, &my);

	if (_windowToDrawOn != NULL){
		_windowToDrawOn->GetWindowPosition(&WinXpos, &WinYpos);
		mx -= WinXpos;
		my -= WinYpos;
	}

	if (mx > _sizeAndPosition.x && mx < _sizeAndPosition.x + _sizeAndPosition.w){
		if (my > _sizeAndPosition.y && my < _sizeAndPosition.y + _sizeAndPosition.h){
			return true;
        }
    }
	return false;
}

/**
 * Return true if the progress was updated
 * Return false otherwise
 */
bool RA_ProgressBar::HandleMouseClick(void)
{
    int mx = 0;
    int my = 0;
    int WinXpos = 0;
    int WinYpos = 0;
    Uint8 perc = 0;

	if (!MouseOver()){
		return false;
    }

	SDL_GetMouseState(&mx, &my);

	if (_windowToDrawOn != NULL){
		_windowToDrawOn->GetWindowPosition (&WinXpos, &WinYpos);
		mx -= WinXpos;
		my -= WinYpos;
	}

	// Calc tbe mouse x-pos relative of the slider pos
	mx -= _sizeAndPosition.x;

	//printf ("Percentage = %u\n", perc);
	if (_maxSteps == -1){
		// Cacl the percentage
		perc = mx * 100 / _sizeAndPosition.w;
	}else{
		_curStep = (Uint32) round ( (double)mx * (double)_maxSteps / (double)_sizeAndPosition.w);
		//printf ("Step = %i\n", Step);
		perc = _curStep * 100 / _maxSteps;
		//printf ("perc = %i\n", perc);
		if (perc > 100){
			perc = 100;
        }
	}

	// Set the percentage
	setProgressPosition (perc);


	Draw(_sizeAndPosition.x, _sizeAndPosition.y);

	return true;
}

void RA_ProgressBar::setProgressPosition(Uint8 Pos)
{
	if (Pos > 100){
		return;
    }
	_position	= Pos;
	_recreate	= true;
}

Uint8 RA_ProgressBar::getProgressPosition()
{
	return _position;
}

void RA_ProgressBar::setNumbSteps(int Steps)
{
	_maxSteps = Steps;
}

void RA_ProgressBar::setCurStep(Uint8 Step)
{
	_curStep = Step;

	Uint8 perc = _curStep * 100 / _maxSteps;

	if (perc > 100){
		perc = 100;
    }

	// Set the percentage
	setProgressPosition (perc);
}

int RA_ProgressBar::getCurStep()
{
	return _curStep;
}

void RA_ProgressBar::Draw(int X, int Y)
{
	// Set size and posisiton
	_sizeAndPosition.x = X;
	_sizeAndPosition.y = Y;

	if (_recreate){
		Create();
    }

	if (_progressSurface == NULL){
		return;
    }

	if (_windowToDrawOn != NULL) {
		SDL_BlitSurface(_progressSurface, NULL, _windowToDrawOn->GetWindowSurface (), &_sizeAndPosition);
    } else {
		SDL_BlitSurface(_progressSurface, NULL, _displaySurface, &_sizeAndPosition);
    }

}

void RA_ProgressBar::Redraw()
{
	Draw ( _sizeAndPosition.x, _sizeAndPosition.y);
}

void RA_ProgressBar::Create(void)
{
	SDL_Rect	dest;

	if (_progressSurface != NULL){
		SDL_FreeSurface(_progressSurface);
	}

	_progressSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, _sizeAndPosition.w, _sizeAndPosition.h, 16, 0, 0, 0, 0);

	// Draw the progress bar background
	dest.x = 0;
	dest.y = 0;
	dest.w = _sizeAndPosition.w;
	dest.h = _sizeAndPosition.h;
	SDL_FillRect(_progressSurface, &dest, _progressBackgroundColor);

	// Draw the actual progress bar
	dest.x = 0;
	dest.y = 0;
	dest.w = (_sizeAndPosition.w-4) * _position/100;
	dest.h = _sizeAndPosition.h;
	SDL_FillRect(_progressSurface, &dest, _progressBarColor);


	/// Start code for the progress bar edge

	// Draw the upper dark line
	dest.x = 0;
	dest.y = 0;
	dest.w = _sizeAndPosition.w;
	dest.h = 1;
	SDL_FillRect(_progressSurface, &dest, _edgeDarkColor);

	// Draw the lower light line
	dest.x = 0;
	dest.y = _sizeAndPosition.h - 1;
	dest.w = _sizeAndPosition.w;
	dest.h = 1;
	SDL_FillRect(_progressSurface, &dest, _edgeLightColor);

	// Draw the left dark line
	dest.x = 0;
	dest.y = 0;
	dest.w = 1;
	dest.h = _sizeAndPosition.h;
	SDL_FillRect(_progressSurface, &dest, _edgeDarkColor);

	// Draw the rigth light line
	dest.x = _sizeAndPosition.w - 1;
	dest.y = 0;
	dest.w = 1;
	dest.h = _sizeAndPosition.h;
	SDL_FillRect(_progressSurface, &dest, _edgeLightColor);

	/// Start code for drawing the position indicator

	// Draw the position left indicator line
	dest.x = (_sizeAndPosition.w-4) * _position/100;
	dest.y = 0;
	dest.w = 1;
	dest.h = _sizeAndPosition.h;
	SDL_FillRect(_progressSurface, &dest,_edgeLightColor);

	// Draw the position top ligth indicator line
	dest.x = (_sizeAndPosition.w-4) * _position/100;
	dest.y = 0;
	dest.w = 4;
	dest.h = 1;
	SDL_FillRect(_progressSurface, &dest,_edgeLightColor);


	// Draw the position rigth indicator line
	dest.x = (_sizeAndPosition.w-4) * _position/100 + 3;
	dest.y = 0;
	dest.w = 1;
	dest.h = _sizeAndPosition.h;
	SDL_FillRect(_progressSurface, &dest,_edgeDarkColor);

	// Draw the position bottum dark indicator line
	dest.x = (_sizeAndPosition.w-4) * _position/100;
	dest.y = _sizeAndPosition.h - 1;
	dest.w = 4;
	dest.h = 1;
	SDL_FillRect(_progressSurface, &dest,_edgeDarkColor);

	_recreate = false;
}
