#ifndef TTEXTBOX_H
#define TTEXTBOX_H

#include "SDL/SDL_events.h"
#include "SDL/SDL_video.h"

#include "RA_Label.h"

class RA_WindowClass;

class TTextBox {
public:
	TTextBox();
	// TTextBox(const int w);
	~TTextBox();
	bool need_redraw(void);
	bool Draw(int X, int Y);
	void SetDrawingWindow(RA_WindowClass *Window);
	bool setText(const std::string& text);
	char* getText();

	bool DeleteChar(unsigned int pos);
	bool MouseOver(void);
	void HandleInput(SDL_Event event);
	void Create(void);
	void setColor(Uint32 Color);
	Uint32 getWidth(void);

private:
	std::string TextBoxString;
	RA_WindowClass *WindowToDrawOn;
	RA_Label TextBoxLabel;
	volatile bool Selected;
	volatile bool Recreate;
	volatile bool HasMarker;
	Uint32 TextColorSelected;
	Uint32 TextColorNotSelected;
	Uint32 TextColor;
	Uint32 TextBoxColor;
	Uint32 TextBoxBackgroundColor;
	SDL_Surface* TextBoxSurface;
	SDL_Surface* DisplaySurface;
	SDL_Rect SizeAndPosition;
	
	bool AddChar(char AddChar);
};

#endif //TTEXTBOX_H
