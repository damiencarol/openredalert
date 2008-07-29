// Sidebar.h
// 1.2

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

#ifndef SIDEBAR_H
#define SIDEBAR_H

#define LEFT_SIDE_BUTTON 1
#define RIGHT_SIDE_BUTTON 2

#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "RA_Label.h"
#include "SidebarGeometry.h"
#include "createmode_t.h"

class RadarAnimEvent;
class SidebarButton;
class Font;
class Player;
class StringTableFile;
class SHPImage;

using std::vector;

/**
 * Sidebar of the Interface in the game
 */
class Sidebar {
public:
    Sidebar(Player *pl, Uint16 height, const char* theatre);
    ~Sidebar();

    bool getVisChanged();
    /** Return visibility of the sidebar */
    bool getVisible() ;
    void ToggleVisible();

    SDL_Surface* getTabImage();

    SDL_Rect* getTabLocation();

    /** Reloads the SDL_Surfaces */
    void ReloadImages();

    SDL_Surface* getSidebarImage(SDL_Rect location);
    bool isOriginalType();

	void DrawButtonTooltip (Uint8 index);
    Uint8 getButton(Uint16 x, Uint16 y);
    void ClickButton(Uint8 index, char* unitname, createmode_t* createmode);
    void ResetButton();
    void ScrollSidebar(bool scrollup);
    void UpdateSidebar();

    void DrawPowerbar();
    void UpdatePowerbar();
    Uint8 getSpecialButton(Uint16 x,Uint16 y);
    void setSpecialButtonState(Uint8 button, Uint8 State);
    Uint8 getSpecialButtonState(Uint8 button);
    void DrawSpecialIcons();

    void StartRadarAnim(Uint8 mode);


    Uint8 getSteps() const;

    const SidebarGeometry& getGeom();
    
    Player* getPlayer();
    
    /** @return true if the radar anim is playing */
    bool isRadaranimating();
    
private:
    Sidebar();
    Sidebar(const Sidebar&);
    Sidebar& operator=(const Sidebar&);

    SDL_Surface *ReadShpImage (char *Name, int ImageNumb);

    friend class RadarAnimEvent;

    /** StringTable of the game. Use to set the tooltips strings */
    StringTableFile* stringFile;

    /** True: DOS, False: GOLD */
    bool isoriginaltype;
    /** Palette offset for structures (Nod's buildings are red) */
    Uint8 spalnum;

    void SetupButtons(Uint16 height);
    void ScrollBuildList(Uint8 dir, Uint8 type);
    void Build(Uint8 index, Uint8 type, char* unitname, createmode_t* createmode);
    void UpdateIcons();
    void UpdateAvailableLists();
    void DownButton(Uint8 index);
    void AddButton(Uint16 x, Uint16 y, const char* fname, Uint8 f, Uint8 pal);
    void DrawButton(Uint8 index);
    void DrawClock(Uint8 index, Uint8 imgnum);
    char* getButtonName(Uint8 index);
	
    Uint32 radarlogo;
    SDL_Rect radarlocation;

    Uint32	tab;
    SDL_Rect tablocation;

    Uint32 powerbar;
    Uint32 power_indicator;


    Uint8 repair_but_state;
    Uint8 sell_but_state;
    Uint8 map_but_state;
    
    /** Location of the "Repair" special button */ 
    SDL_Rect RepairLoc;
    /** Location of the "Sell" special button */ 
    SDL_Rect SellLoc;
    /** Location of the "map" special button */ 
    SDL_Rect MapLoc;

    SDL_Surface* sbar;
    SDL_Rect sbarlocation;

	// The build clock surface
	//SDL_Surface *Clock;

//    Font *gamefnt;

    bool visible; 
    bool vischanged;

    const char* theatre;

    Uint8 buttondown;
    bool bd;

    Uint8 buildbut;
    vector<SidebarButton*> buttons;

    vector<char*> uniticons;
    vector<char*> structicons;

    const char* radarname;
    RadarAnimEvent* radaranim;
    bool radaranimating;

    /** use for scrolling */
    Uint8 unitoff;
    /** use for scrolling */
    Uint8 structoff;

    Player* player;
    int scaleq;

    Uint8 steps;

    SidebarGeometry geom;

	RA_Label StatusLabel;
	RA_Label QuantityLabel;
	RA_Label FallbackLabel;

    bool greyFixed[256];
#if 0
    SDL_Surface* FixGrey(SDL_Surface* gr, Uint8 num);
#endif
	/** Images of the clock animation */
    SDL_Surface* Clocks[256];
    
    /** USSR logo image (below radar) */
    SHPImage* ussrAnimRadarImage;
    
    /** Repair button images of the sidebar (3 images for the three steps of the button) */
    SHPImage* repairImages;
    /** Sell button images of the sidebar (3 images for the three steps of the button) */
    SHPImage* sellImages;
    /** Map button images of the sidebar (3 images for the three steps of the button) */
    SHPImage* mapImages;
};

#endif //SIDEBAR_H
