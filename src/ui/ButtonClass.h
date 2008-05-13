#ifndef BUTTONCLASS_H_
#define BUTTONCLASS_H_

#include <list>
#include <string>
#include <vector>

#include "SDL/SDL_events.h"
#include "SDL/SDL_types.h"

#define BUTTON_TYPE_DEFAULT	1
#define BUTTON_TYPE_LOCK_UNLOCK	2
#define BUTTON_TYPE_EXTERNAL	2

#define BUTTON_STATE_UP		1
#define BUTTON_STATE_DOWN	2
#define BUTTON_STATE_OVER	3	// NOT USED YET

#include "RA_Label.h"

class RA_Label;
class RA_WindowClass;


class ButtonClass {
public:
	ButtonClass();
	~ButtonClass();
	bool handleMouseEvent(SDL_Event event);
	void SetDrawingSurface(SDL_Surface *Surface);
	void SetDrawingWindow(RA_WindowClass *Window);
	/** Set the font color in the button up state.*/
	void setFontColor_up(Uint8 red, Uint8 green, Uint8 blue);
	/**  Set the font color in the button down state.*/
	void setFontColor_down(Uint8 red, Uint8 green, Uint8 blue);
	void setcolor(Uint8 red, Uint8 green, Uint8 blue);
	void setcolor(Uint32 SDL_color);
	void setcolor(SDL_Color Color);
	Uint32 GetColor();
	void setposition(int x, int y);
	void setsize(int w, int h);
	void setText(const std::string buff);
	void setButtonState(int state);
	int getButtonState(void);
	void drawbutton(void);
	bool MouseOver(void);
	bool NeedsRedraw(void);
	void CreateSurface(void);
	void CreateSurface(const std::string& ButtonText, int Xpos, int Ypos,
			int Width, int Heigth);

private:
	RA_WindowClass *WindowToDrawOn;
	SDL_Color FontColor_up;
	SDL_Color FontColor_down;
	SDL_Color ButtonColor;
	SDL_Surface* DisplaySurface;
	SDL_Surface* ButtonImg_up;
	SDL_Surface* ButtonImg_down;
	SDL_Surface* ButtonImg_over;
	SDL_Rect SizeAndPosition;
	Uint32 color; // SDL color (of the button)
	Uint32 LightColor;
	Uint32 DarkColor;
	RA_Label ButtonUpLabel;
	RA_Label ButtonDownLabel;
	std::string ButtonText;
	/**
	 * Button state tells the class if the button is up,
	 *  down or the mouse is  over the button
	 */
	int ButtonState;
	Uint8 ButtonType;
};

#endif
