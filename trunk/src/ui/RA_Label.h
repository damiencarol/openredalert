// RA_Label.h
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

#ifndef RA_LABEL_H
#define RA_LABEL_H

#include <list>
#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "Font.h"

class RaWindow;

using std::string;

/**
 * 
 */
class RA_Label
{
public:
    RA_Label();
    ~RA_Label();

    Uint32 getHeight();
    Uint32 getWidth();
    Uint32 getWidth(const string text);

    void setText(const string text);
    string getText(void);

    void setColor(SDL_Color RGBcolor);
    void setColor(Uint32 color);
    void setColor(Uint8 r, Uint8 g, Uint8 b);

    void SetDrawingSurface(SDL_Surface * DwgSurface);
    void SetDrawingWindow(RaWindow* Window);

    void SetFont(const string FontName);
    void UseAntiAliasing(bool status);
    void underline(bool status);

    void Draw(int X, int Y);
    void Draw(SDL_Surface * DrawingSurface, int X, int Y);
    void Draw(const string & text, SDL_Surface * DrawingSurface, int X, int Y);
    void Draw(const string & text, SDL_Surface * DrawingSurface, SDL_Color Fcolor, int X, int Y);
    void Redraw();

private:
	void Create();
	
	string LabelText;
    bool Checked;
    bool recreate;
    int Width;
    int Heigth;
    SDL_Color LabelFontColor;
    SDL_Color ColorKeyColor;
    SDL_Surface* LabelSurface;
    SDL_Surface* DrawingSurface;
    SDL_Surface* BackgroundBackup;
    RaWindow* DrawingWindow;
    Font LabelFont;
    SDL_Rect LabelDest;
};

#endif //RA_LABEL_H
