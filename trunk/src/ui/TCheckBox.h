// TCheckBox.h
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

#ifndef TCHECKBOX_H
#define TCHECKBOX_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

/**
 * Component of the interface which represent a checkbox
 */
class TCheckBox 
{
public:
	TCheckBox();
	~TCheckBox();
	
	void Draw(int X, int Y);

private:
	void Create();
	
	bool Checked;
	int Width;
	int Heigth;
	Uint32 CheckboxColor;
	Uint32 CheckboxBackgroundColor;
	SDL_Surface* CheckBoxSurface;
	SDL_Surface* DisplaySurface;
};

#endif //TCHECKBOX_H
