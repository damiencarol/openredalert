// DropDownListBox.h
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

#ifndef DROPDOWNLISTBOX_H
#define DROPDOWNLISTBOX_H

#include <list>
#include <string>
#include <vector>

#include "SDL/SDL_events.h"
#include "RA_Label.h"

/**
 *
 */
class DropDownListBox {
public:
	DropDownListBox();
	~DropDownListBox();

	bool need_redraw();
	void SetDrawingWindow(RaWindow* Window);
	void Create();
	void AddEntry(string Entry);
	bool MouseOver();
	bool MouseOver_button();
	Uint32 MouseOver_entry();

	/** Get the entry number that was selected */
	unsigned int getSelected() const;

	void HandleInput(SDL_Event event);
	bool Draw(int X, int Y);

private:
	SDL_Surface* ReadShpImage(char *Name, int ImageNumb, Uint8 palnum = 1);

	RaWindow* WindowToDrawOn;
	RA_Label ListBoxLabel;

	/** The entry number that was selected */
	unsigned int SelectedIndex;

	vector<string> List;
	SDL_Surface* ListBoxSurface;
	SDL_Surface* DisplaySurface;
	SDL_Surface* ArrowDownImage;
	SDL_Rect SizeAndPosition;
	Uint32 ListBoxColor;
	Uint32 ListBoxBackgroundColor;
	SDL_Color TextColor;
	volatile bool Selected;
	volatile bool Recreate;
	Uint16 Spacing;
	bool button_down;
	Uint32 ArrowDwn;
};

#endif //DROPDOWNLISTBOX_H
