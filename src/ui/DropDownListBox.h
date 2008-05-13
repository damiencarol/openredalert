#ifndef DROPDOWNLISTBOX_H_
#define DROPDOWNLISTBOX_H_

#include <list>
#include <string>
#include <vector>

#include "SDL/SDL_events.h"
#include "RA_Label.h"


class DropDownListBox{
	public:
		DropDownListBox();
		~DropDownListBox();
		bool	need_redraw(void);
		void	SetDrawingWindow(RA_WindowClass *Window);
		void	Create (void);
		void	AddEntry(std::string Entry);
		bool	MouseOver(void);
		bool 	MouseOver_button(void);
		Uint32	MouseOver_entry(void);
		Uint32	selected(void);
		Uint32	ArrowDwn;
		void	HandleInput(SDL_Event event);
		bool	Draw(int X, int Y);

	private:
		SDL_Surface* ReadShpImage (char *Name, int ImageNumb, Uint8 palnum = 1);

		RA_WindowClass 				*WindowToDrawOn;
		RA_Label					ListBoxLabel;
		Uint32						SelectedIndex;			// The entry nr. that was selected
		std::vector <std::string>	List;
		SDL_Surface* ListBoxSurface;
									SDL_Surface* DisplaySurface;
									SDL_Surface* ArrowDownImage;
		SDL_Rect					SizeAndPosition;
		Uint32 ListBoxColor;
									Uint32 ListBoxBackgroundColor;
		SDL_Color					TextColor;
		volatile bool Selected;
									volatile bool Recreate;
		Uint16						Spacing;
		bool						button_down;
};


#endif //DROPDOWNLISTBOX_H_
