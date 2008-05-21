// GraphicsEngine.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"


class RA_Label;
class Unit;
class ImageCache;
class CnCMap;

using std::vector;


class GraphicsEngine
{
public:
    GraphicsEngine();
    ~GraphicsEngine();
    void setupCurrentGame();
    void renderScene(bool flipscreen = true);
    Uint16 getWidth()
    {
        return width;
    }
    Uint16 getHeight()
    {
        return height;
    }
    SDL_Rect *getMapArea()
    {
        return &maparea;
    }
    void drawVQAFrame(SDL_Surface *frame);
    void clearBuffer();
    void clearScreen();
    /*void postMessage(char *msg) {
        messages->postMessage(msg);
    }*/

    void renderLoading(const std::string& buff, SDL_Surface* logo);

	SDL_Surface *get_SDL_ScreenSurface (void){
		return screen;
	}

private:
	bool MapPosToScreenXY (Uint32 MapPos, Sint16 *ScreenX, Sint16 *ScreenY, CnCMap* map);
	void DrawSelectionBox (void);
	void DrawMouse (void);
	void DrawTooltip (void);
    void DrawMinimap(void);
	void DrawRepairing(void);
//	void DrawUnits(void);
	void DrawVehicleSmoke(void);
	void DrawStructureHealthBars(SDL_Rect dest, SDL_Rect udest, Uint32 curdpos);
	void DrawGroundUnitHealthBars(SDL_Rect dest, SDL_Rect	udest, Uint32 curdpos);
	void DrawFlyingUnitHealthBars(SDL_Rect dest, SDL_Rect	udest, Uint32 curdpos);
	void DrawL2Overlays (void);
	void DrawFogOfWar(SDL_Rect dest, SDL_Rect src, SDL_Rect udest);
	void DrawMap(SDL_Rect dest, SDL_Rect src, SDL_Rect udest);
    void clipToMaparea(SDL_Rect *dest);
    void clipToMaparea(SDL_Rect *src, SDL_Rect *dest);
    void drawSidebar();
    void drawLine(Sint16 startx, Sint16 starty,
                  Sint16 stopx, Sint16 stopy, Uint16 width, Uint32 colour);
    
    
    SDL_Surface* screen;
    SDL_Surface* icon;
    Uint16 width;
    Uint16 height;
    /** Width of a tile at the screen */
    Uint16 tilewidth;
    /* Height of a tile a the screen */
    Uint16 tileheight;
    SDL_Rect maparea;
    bool drawFogOfWar;

	/** FrameRate label */
	RA_Label* FrameRateLabel;
	RA_Label* OptionsLabel;
	/** Money label */
	RA_Label* MoneyLabel;

    Uint32 repair_icon;
    Uint32 repairing_icon;
    Uint32 sell_icon;
    Uint32 map_icon;


	SDL_Rect	oldmouse;

	Uint32 whitepix;
			Uint32 greenpix;
			Uint32 yellowpix;
			Uint32 redpix;
			Uint32 blackpix;

	vector<Uint16> l2overlays;


	Uint32 firstframe;
	Uint32 frames;
	bool clearBack;

	// Surfaces needed to draw text
	SDL_Surface* FrameRateSurface;
				SDL_Surface* MoneySurface;
				SDL_Surface* OptionsSurface;

    /** Minimap zoom factor*/
    struct minizoom {
        Uint8 normal;
        Uint8 max;
    } minizoom;
    
    /** Which zoom is currently used? */
    Uint8* mz;
    /** Used to avoid SDL_MapRGB in the radar render step. */
    vector<Uint32> playercolours;
};

#endif //GRAPHICSENGINE_H
