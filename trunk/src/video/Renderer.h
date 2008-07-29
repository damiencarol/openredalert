// Renderer.h
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
