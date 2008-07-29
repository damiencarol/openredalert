// Listbox.cpp
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

#ifndef LISTEBOXCLASS_H
#define LISTEBOXCLASS_H

#include <vector>
#include <string>

#include "SDL/SDL_events.h"
#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"
#include "ListBoxLine.h"

// Listbox defines
#define LISTBOX_SCROLLBAR_WITH 		15
#define LISTBOX_ARROW_UPDOWN_HEIGTH	16
#define LISTBOX_BORDER_WITH			10
#define LISTBOX_FONT_LINE_SPACING	2

#define LISTBOX_TYPE_NORMAL		1	// Normal selection list
#define LISTBOX_TYPE_CHECK		2	// Checkbox kind of lis

using std::vector;
using std::string;

/**
 * 
 */
class ListboxClass {
public:
	ListboxClass();
	~ListboxClass();
	
	SDL_Surface* ReadShpImage(char *Name, int ImageNumb, Uint8 palnum);
	void SetType(unsigned int type);
	bool IsChecked(unsigned int line); // Function for the checkbox type
	int GetSelectionIndex(void);
	SDL_Surface* geDrawingSurface(void);
	void ScrollUp();
	void ScrollDown();
	void HandleInput(SDL_Event event);
	bool MouseOverArrowUp();
	bool MouseOverArrowDown();
	void AddString(const string String);
	void DrawListBox(int Xpos, int Ypos);

private:
	unsigned int CalcSliderHeight(void);
	unsigned int CalcSliderYoffset();
	void DrawSlider();
	void CreateListBox();
	
	std::vector <std::string> MessageList; // For checkbox type of listbox
	std::vector <bool> CheckedList; // For checkbox type of listbox
	std::vector <ListBoxLine> ListboxLines;
	SDL_Surface* ListBoxSurface;
	SDL_Surface* DisplaySurface;
	SDL_Surface* btn_up[4];
	SDL_Surface* btn_down[4];
	int FontHeigth;
	int NumbLines;
	int Xpos;
	int Ypos;
	int With;
	Uint16 ScrollPos;
	Uint16 SelectPos;
	bool Recreate;
	Uint8 ListBoxType;
	Uint8 btn_up_down;
	Uint8 btn_down_down;
};

#endif //LISTEBOXCLASS_H
