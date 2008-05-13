#ifndef RENDERER_H_
#define RENDERER_H_

#include "SDL/SDL_video.h"
#include "SDL/SDL_types.h"

class Renderer {
public:

    Renderer();
    ~Renderer();


    void DrawRectangle(Uint32 color, Uint16 x, Uint16 y, Uint16 width, Uint16 height, Uint8 lnpx);
    void DrawSolidRectangle(Uint32 color, Uint16 x, Uint16 y, Uint16 width, Uint16 height);
    void InitializeScreen(void);
    SDL_Surface * getScreen();

private:
    SDL_Surface * screen;
    SDL_Surface * icon;
};

#endif //RENDERER_H_
