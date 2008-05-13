#ifndef RA_PROGRESS_BAR_H_
#define RA_PROGRESS_BAR_H_

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class RA_WindowClass;

class RA_ProgressBar {
public:
	RA_ProgressBar();
	~RA_ProgressBar();

	void SetDrawingSurface(SDL_Surface *DwgSurface);
	void SetDrawingWindow(RA_WindowClass *Window);

	bool MouseOver(void);
	bool HandleMouseClick(void);

	void setProgressPosition(Uint8 Pos);
	Uint8 getProgressPosition(void);
	void setNumbSteps(int Steps);
	void setCurStep(Uint8 Step);
	int getCurStep(void);

	void Draw(int X, int Y);
	void Redraw(void);

private:
	RA_WindowClass *_windowToDrawOn;
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

	void Create(void);
};

#endif
