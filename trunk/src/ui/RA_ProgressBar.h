// RA_ProgressBar.h
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

#ifndef RA_PROGRESSBAR_H
#define RA_PROGRESSBAR_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class RaWindow;

/**
 * 
 */
class RA_ProgressBar
{
public:
	RA_ProgressBar();
	~RA_ProgressBar();

	void SetDrawingSurface(SDL_Surface *DwgSurface);
	void SetDrawingWindow(RaWindow* Window);

	bool MouseOver();
	bool HandleMouseClick();

	void setProgressPosition(Uint8 Pos);
	Uint8 getProgressPosition();
	void setNumbSteps(int Steps);
	void setCurStep(Uint8 Step);
	int getCurStep();

	void Draw(int X, int Y);
	void Redraw();

private:
	void Create();
	
	RaWindow* _windowToDrawOn;
	SDL_Surface* _progressSurface;
	SDL_Surface* _displaySurface;
	SDL_Rect _sizeAndPosition;
	bool _recreate;
	bool _selected;
	Uint32 _progressBarColor;
	Uint32 _progressBackgroundColor;
	Uint32 _edgeLightColor;
	Uint32 _edgeDarkColor;
	Uint32 _position;

	int _maxSteps;
	int _curStep;
};

#endif //RA_PROGRESSBAR_H
