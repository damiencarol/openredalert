#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <list>
#include <string>
#include <vector>

class SliderControl {
	public:
		SliderControl();
		SliderControl(const int w, const int steps, const bool showSliderstatus);
		~SliderControl();
		void	SetDrawingWindow(RA_WindowClass *Window);
		bool	MouseOver(void);
		void    HandleInput(SDL_Event event);
		void    HandleSliding();
		void    Create (void);
		void    CreateSlider(int x);
		bool	Draw(int X, int Y);

	private:
		RA_WindowClass 	*_windowToDrawOn;
		SDL_Surface* _sliderSurface;
				SDL_Surface* _displaySurface;
		SDL_Rect	_sizeAndPosition;
		bool _recreate; bool _selected;
				bool _showSliderstatus;
		Uint32 _position;
				Uint32 _sliderColor;
				Uint32 _sliderBackgroundColor;
				Uint32 _steps;
				Uint32 _oldPosition;

};


#endif //WIDGETS_H_
