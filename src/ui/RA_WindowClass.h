#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <list>
#include <string>
#include <vector>

#include "SDL/SDL_events.h"

class Font;
class RA_Label;

class RA_WindowClass {
public:
	RA_WindowClass();
	~RA_WindowClass();
	void setPalette(Uint8 pal);
	void DrawRaBackground(void);
	void SolidFill(Uint8 red, Uint8 green, Uint8 blue);
	void ResizeWindow(int Width, int Heigth);
	void GetWindowPosition(int *Xpos, int *Ypos);
	void ChangeWindowPosition(int Xpos, int Ypos);
	SDL_Surface *GetWindowSurface(void);
	void SetupWindow(int Xpos, int Ypos, int Width, int Heigth);
	void DrawWindow(void);

private:
	SDL_Surface *DisplaySurface;
	SDL_Surface *WindowSurface;
	SDL_Rect SizeAndPosition;

	Uint32 Background;
	Uint32 LeftBorder;
	Uint32 RightBorder;
	Uint32 TopBorder;
	Uint32 BottomBorder;
	Uint32 Corner;
	Uint8 PalNr;

	SDL_Surface *ReadShpImage(char *Name, int ImageNumb, Uint8 palnum = 1);

};

#endif
