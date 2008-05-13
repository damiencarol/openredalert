#ifndef RA_LABEL_H
#define RA_LABEL_H

#include <list>
#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "ui/Font.h"
//#include "RA_WindowClass.h"

class RA_WindowClass;
using std::string;


class RA_Label {
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
    void SetDrawingWindow(RA_WindowClass * Window);

    void SetFont(const string FontName);
    void UseAntiAliasing(bool status);
    void underline(bool status);

    void Draw(int X, int Y);
    void Draw(SDL_Surface * DrawingSurface, int X, int Y);
    void Draw(const string & text, SDL_Surface * DrawingSurface, int X, int Y);
    void Draw(const string & text, SDL_Surface * DrawingSurface, SDL_Color Fcolor, int X, int Y);
    void Redraw(void);

private:
    string LabelText;
    bool Checked;
    bool recreate;
    int Width;
    int Heigth;
    SDL_Color LabelFontColor;
    SDL_Color ColorKeyColor;
    SDL_Surface * LabelSurface;
    SDL_Surface * DrawingSurface;
    SDL_Surface * BackgroundBackup;
    RA_WindowClass * DrawingWindow;
    Font LabelFont;
    SDL_Rect LabelDest;

    void Create(void);
};

#endif //RA_LABEL_H
