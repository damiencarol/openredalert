#ifndef TCHECKBOX_H_
#define TCHECKBOX_H_

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class TCheckBox {
public:
	TCheckBox();
	~TCheckBox();
	void Draw(int X, int Y);

private:
	bool Checked;
	int Width;
	int Heigth;
	Uint32 CheckboxColor;
	Uint32 CheckboxBackgroundColor;
	SDL_Surface* CheckBoxSurface;
	SDL_Surface* DisplaySurface;

	void Create(void);
};

#endif /* TCHECKBOX_H_ */
