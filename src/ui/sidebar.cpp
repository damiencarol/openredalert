// Sidebar.cpp
// 1.7

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
    
#include "Sidebar.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "include/config.h"
#include "Input.h"
#include "Cursor.h"
#include "SidebarButton.h"
#include "game/RadarAnimEvent.h"
#include "createmode_t.h"
#include "SidebarError.h"
#include "include/Logger.h"
#include "video/SHPImage.h"
#include "video/ImageCache.h"
#include "video/ImageCacheEntry.h"
#include "audio/SoundEngine.h"
#include "include/PlayerPool.h"
#include "include/sdllayer.h"
#include "sidebarop.h"
#include "video/ImageNotFound.h"
#include "game/pside.h"
#include "game/Player.h"
#include "game/UnitAndStructurePool.h"
#include "misc/StringTableFile.h"
#include "vfs/vfs.h"

/// @TODO Move this into config file(s)
static const char* radarnames[] =
{
//   Gold Edition radars: NOD, GDI and Dinosaur
    "hradar.nod", "hradar.gdi", "hradar.jp",
//   Dos Edition radars
    "radar.nod",  "radar.gdi",  "radar.jp",
//   Red Alert radars (We only use large sidebar for RA)
/*    "ussrradr.shp", "natoradr.shp", "natoradr.shp"*/
    "uradrfrm.shp", "nradrfrm.shp", "nradrfrm.shp"
};

class ImageCache;

using std::string;
using std::runtime_error;
using std::vector;
using std::replace;
using std::ostringstream;

extern Logger * logger;
namespace pc {
	extern ConfigType Config;
	extern ImageCache* imgcache;
}

#define S_BUTTON_NONE	255
#define S_BUTTON_REPAIR	1
#define S_BUTTON_SELL	2
#define S_BUTTON_WORLD	3

/**
 * @param pl local player object
 * @param height height of screen in pixels
 * @param theatre terrain type of the theatre
 *      TD: desert, temperate and winter
 *      RA: temperate and snow
 */
Sidebar::Sidebar(Player *pl, Uint16 height, const char *theatre)
    : radarlogo(0), tab(0), sbar(0), visible(true),
    vischanged(true), theatre(theatre), buttondown(0), bd(false),
    radaranimating(false), unitoff(0), structoff(0), player(pl), scaleq(-1)
{
    //const char* tmpname;
    Uint8 side = 0;
    SDL_Surface *tmp;

    // If we can't load these files, there's no point in proceeding, thus we let
    // the default handler for runtime_error in main() catch.
    /*if (pc::Config.gamenum == GAME_TD) {
        tmpname = VFSUtils::VFS_getFirstExisting(2,"htabs.shp","tabs.shp");
        if (tmpname == NULL) {
            throw runtime_error("Unable to find the tab images! (Missing updatec.mix?)");
        } else if (strcmp((tmpname), ("htabs.shp"))==0) {
            isoriginaltype = false;
        } else {
            isoriginaltype = true;
        }
        try {
            tab = pc::imgcache->loadImage(tmpname, scaleq);
        } catch (ImageNotFound&) {
            throw runtime_error("Unable to load the tab images!");
        }
    } else {*/
        isoriginaltype = false;

        // Load tabs images
        try {
            tab = pc::imgcache->loadImage("tabs.shp", scaleq);
        } catch (ImageNotFound&) {
            throw runtime_error("Unable to load the tab images!");
        }

        // Load powerbar images
        try {
            powerbar = pc::imgcache->loadImage("powerbar.shp", scaleq);
        } catch (ImageNotFound&) {
            throw runtime_error("Unable to load the powerbar_up images!");
        }

        // Load power indicator image
        try {
            power_indicator = pc::imgcache->loadImage("power.shp", scaleq);
        } catch (ImageNotFound&) {
            throw runtime_error("Unable to load the power image!");
        }

        // Load the sell icon image
        try {
            sell_icon = pc::imgcache->loadImage("sell.shp", scaleq);
        } catch (ImageNotFound&) {
            throw runtime_error("Unable to load the sell icon!");
        }

        // Load the repair icon image
        try {
            repair_icon = pc::imgcache->loadImage("repair.shp", scaleq);
        } catch (ImageNotFound&) {
            throw runtime_error("Unable to load the repair icon!");
        }

        // Load the map icon image
        try {
            map_icon = pc::imgcache->loadImage("map.shp", scaleq);
        } catch (ImageNotFound&) {
            throw runtime_error("Unable to load the map icon!");
        }

        //Powerbar_up	= this->ReadShpImage ("powerbar.shp", 0);
        //Powerbar_down	= this->ReadShpImage ("powerbar.shp", 1);
        //PowerIndicator	= this->ReadShpImage ("power.shp", 0);
    //}

    tmp = pc::imgcache->getImage(tab).image;

    tablocation.x = 0;
    tablocation.y = 0;
    tablocation.w = tmp->w;
    tablocation.h = tmp->h;

    spalnum = pl->getStructpalNum();
    if (!isoriginaltype) {
        spalnum = 0;
    }
    // Select a radar image based on sidebar type (DOS/Gold)
    // and player side (Good/Bad/Other)
    if ((player->getSide()&~PS_MULTI) == PS_BAD) {
        side = isoriginaltype?3:0;
    } else if ((player->getSide()&~PS_MULTI) == PS_GOOD) {
        side = isoriginaltype?4:1;
    } else {
        side = isoriginaltype?5:2;
    }

    if (pc::Config.gamenum == GAME_RA) {
        // RA follows (Gold names) * 3 and (DOS names) * 3
        side += 6;
    }

    radarname = radarnames[side];
    radaranim = 0;
    try {
        radaranimnumb = radarlogo = pc::imgcache->loadImage(radarname, scaleq);
    } catch (ImageNotFound&) {
        /// @TODO This problem should ripple up to the "game detection" layer
        // so it can try again from scratch with a different set of data files.
        logger->error("Hmm.. managed to misdetect sidebar type\n");
        try {
            // Switch between Gold and DOS
            if (side < 3) {
                side += 3;
            } else {
                side -= 3;
            }
            radarname = radarnames[side];
            radarlogo = pc::imgcache->loadImage(radarname, scaleq);
            isoriginaltype = !isoriginaltype;
        } catch (ImageNotFound&) {
            logger->error("Unable to load the radar-image! (Maybe you run c&c gold but have forgoten updatec.mix?)\n");
            throw SidebarError();
        }
    }
	if (pc::Config.gamenum == GAME_RA) {
		if ((player->getSide()&~PS_MULTI) == PS_BAD) {
			radaranimnumb = pc::imgcache->loadImage("ussrradr.shp", scaleq);
		}else
			radaranimnumb = pc::imgcache->loadImage("natoradr.shp", scaleq);
	}

    SDL_Surface *radar;
    if (pc::Config.gamenum == GAME_TD){
    	radar = pc::imgcache->getImage(radarlogo).image;
    } else {
    	radar = pc::imgcache->getImage(radarlogo,1).image;
    }
    //	radar = ReadShpImage ((char *)radarnames[side], 1);

    radarlocation.x = 0;  
    radarlocation.y = 0;
    radarlocation.w = radar->w;  
    radarlocation.h = radar->h;

    // Set steps for CLOCK ?????
    steps = 54;//:108);

    sbarlocation.x = sbarlocation.y = sbarlocation.w = sbarlocation.h = 0;

    std::fill(greyFixed, greyFixed+256, false);

    pc::sidebar = this;
    SetupButtons(height);

    // Set the states of the buttons to "UNDOWN" = 0
    repair_but_state = 0;
    sell_but_state = 0;
    map_but_state = 0;

    // Free claock images
	for (int i = 0; i < 256; i++){
		Clocks[i] = NULL;
	}



	StatusLabel.setColor(0xff, 0xff, 0xff);
	StatusLabel.SetFont("grad6fnt.fnt");
	StatusLabel.UseAntiAliasing(false);

	QuantityLabel.setColor(0xff, 0xff, 0xff);
	QuantityLabel.SetFont("grad6fnt.fnt");
	QuantityLabel.UseAntiAliasing(false);
	
	// Create the string table object
	stringFile = new StringTableFile("conquer.eng");
}

/**
 * Destructor
 */
Sidebar::~Sidebar()
{
    Uint32 i;

    SDL_FreeSurface(sbar);


    for (i=0; i < uniticons.size(); ++i) {
        delete[] uniticons[i];
    }

    for (i=0; i < structicons.size(); ++i) {
        delete[] structicons[i];
    }

	for (int i = 0; i < 256; i++){
		if (Clocks[i] != NULL){
			SDL_FreeSurface(Clocks[i]);
		}
	}
	
	// Free the string table object
	delete stringFile;
}

/**
 * @returns whether the sidebar's visibility has changed.
 */
bool Sidebar::getVisChanged() {
    if (vischanged) {
        vischanged = false;
        return true;
    }
    return false;
}

void Sidebar::ToggleVisible()
{
    visible = !visible;
    vischanged = true;
}

void Sidebar::ReloadImages()
{
    // Free the sbar, so that it is redrawn on next update
    SDL_FreeSurface(sbar);

    //
    sbar = 0;

	// Reset location of the sidebar
    sbarlocation.x = 0;
    sbarlocation.y = 0;
    sbarlocation.w = 0;
    sbarlocation.h = 0;

    // Reload the font we use
    //gamefnt->reload();

    // Reload all the buttons
//    for_each(buttons.begin(), buttons.end(), boost::mem_fn(&Sidebar::SidebarButton::ReloadImage));
	for (unsigned int i = 0; i < buttons.size(); i++){
		buttons[i]->ReloadImage();
	}

    // Invalidate grey fix table
    std::fill(greyFixed, greyFixed+256, false);
}

SDL_Surface *Sidebar::getSidebarImage(SDL_Rect location)
{
    SDL_Rect dest, src, newdest;
    SDL_Surface *temp;

    if (location.w == sbarlocation.w && location.h == sbarlocation.h){
        return sbar;
    }
    
    SDL_FreeSurface(sbar);

    temp = SDL_CreateRGBSurface(SDL_SWSURFACE, location.w, location.h, 16, 0, 0, 0, 0);
    sbar = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    location.x = 0;
    location.y = 0;

    /// @TODO HACK
    if (isoriginaltype || pc::Config.gamenum == GAME_RA) {
        SDL_FillRect(sbar, &location, SDL_MapRGB(sbar->format, 1, 1, 1));
    } else {
        try {
            // Get the index of the btexture
            Uint32 idx = pc::imgcache->loadImage("btexture.shp", scaleq);

            // Get the SDL_Surface for this texture
            SDL_Surface* texture = pc::imgcache->getImage(idx).image;

            dest.x = 0;
            dest.y = 0;
            dest.w = location.w;
            dest.h = texture->h;
            src.x = 0;
            src.y = 0;
            src.w = location.w;
            src.h = texture->h;
            for (dest.y = 0; dest.y < location.h; dest.y += dest.h) {
                SDL_BlitSurface(texture, &src, sbar, &dest);
            }
        } catch (ImageNotFound&) {
            logger->error("Unable to load the background texture\n");
            SDL_FillRect(sbar, &location, SDL_MapRGB(sbar->format, 0xa0, 0xa0, 0xa0));
        }
    }
    sbarlocation = location;

	SDL_Surface* radar;
	if (!Input::isMinimapEnabled()) {

		if (pc::Config.gamenum == GAME_TD)
			radar = pc::imgcache->getImage(radarlogo).image;
		else
			radar = pc::imgcache->getImage(radarlogo, 1).image;

		SDL_SetColorKey(radar, SDL_SRCCOLORKEY, 0xffffff);

		SDL_BlitSurface(radar, 0, sbar, &radarlocation);

		// overdraw the with the correct upper part
		radar = pc::imgcache->getImage(radaranimnumb, 0).image;
		SDL_SetColorKey(radar,SDL_SRCCOLORKEY, 0xffffff);
		newdest.x = radarlocation.x;
		newdest.y = radarlocation.y;
		newdest.h = radar->h;
		newdest.w = radar->w;
		// SDL_FillRect(sbar, &newdest, SDL_MapRGB(pc::sidebar->sbar->format, 0x0a, 0x0a, 0x0a));
		SDL_BlitSurface( radar, NULL, sbar, &newdest);
	}
	else
	{
		radar = pc::imgcache->getImage(radarlogo, 1).image;
		SDL_SetColorKey(radar,SDL_SRCCOLORKEY, 0xffffff);
		SDL_BlitSurface(radar, NULL, sbar, &radarlocation);
	}

	// Draw all button
    for (Sint8 x = buttons.size()-1;x>=0;--x) {
        DrawButton(x);
    }

	// Draw Power bar
    DrawPowerbar();
    // Draw special icons
    DrawSpecialIcons();

	// Return the image builted
    return sbar;
}

void Sidebar::GetButtonName(Uint8 index, char *UnitOrStructureName)
{
	Uint8 function = buttons[index]->getFunction();
	Uint8* offptr;
	std::vector<char*>* vecptr;
	Uint8 type;

	strncpy(UnitOrStructureName,"xxxx",4);

	UnitOrStructureName[5] = '\0';

	// Check that this is a build button and not a scroll button
	if ((function&0x3) != 1){
		return;
	}
	
	Uint8 VecPtrIndex = index - 3 - ((function&sbo_unit)?0:buildbut);

	type = (function&sbo_unit);

	if (type) {
		offptr = &unitoff;
		vecptr = &uniticons;
	} else {
		offptr = &structoff;
		vecptr = &structicons;
	}

	if ( (unsigned)(*vecptr).size() > ((unsigned)(*offptr)+VecPtrIndex - 1) ) {
		strncpy(UnitOrStructureName,(*vecptr)[(*offptr+VecPtrIndex-1)],13);
		UnitOrStructureName[strlen((*vecptr)[(*offptr+VecPtrIndex-1)])-8] = 0x0;
	} else {
		// Out of range
		return;
	}
}

void Sidebar::DrawButtonTooltip(Uint8 index)
{
	char 			UnitOrStructureName[20];	// should be at least 13 :)
	Uint8 			unit;
	Uint8 			function;
	//char			TooltipText[20];
	ostringstream	TipString;


	// Get the function of this button
	function = buttons[index]->getFunction();

	// Get the button name with this index
	GetButtonName(index, UnitOrStructureName);

	unit = ( function & sbo_unit );

	UnitOrStructureType* type;
	if (unit) {
		type = p::uspool->getUnitTypeByName(UnitOrStructureName);
	} else {
		type = p::uspool->getStructureTypeByName(UnitOrStructureName);
	}

	if (type == 0){
		return;
	}
	TipString.str("");
	if (type->getName() != 0)
	{
		// Get raw name of the type
		string nameOfType = string(type->getTName());
		if (nameOfType=="POWR"){
			TipString << stringFile->getString(126);
		} else if (nameOfType=="BRIK"){
			TipString << stringFile->getString(134);				
		} else if (nameOfType=="FENC"){
			TipString << stringFile->getString(135);				
		} else if (nameOfType=="SBAG"){
			TipString << stringFile->getString(132);				
		} else if (nameOfType=="TENT"){
			TipString << stringFile->getString(129);				
		} else if (nameOfType=="BARR"){
			TipString << stringFile->getString(129);				
		} else if (nameOfType=="FACF"){
			TipString << stringFile->getString(430);				
		} else if (nameOfType=="PROC"){
			TipString << stringFile->getString(85);				
		} else if (nameOfType=="SPEN"){
			TipString << stringFile->getString(412);				
		} else if (nameOfType=="APWR"){
			TipString << stringFile->getString(127);	
		} else if (nameOfType=="SYRD"){
			TipString << stringFile->getString(411);				
		} else if (nameOfType=="GUN"){
			TipString << stringFile->getString(434);				
		} else if (nameOfType=="IRON"){
			TipString << stringFile->getString(424);				
		} else {
			TipString << type->getName();
		}		
	} 
	else
	{
		TipString << "?";
	}
	
	// Complete the tooltip text with cost
	TipString << "\n" << "$"<< type->getCost();

	// Set the tooltip of the cursor
	pc::cursor->setTooltip(TipString.str());
}

Uint8 Sidebar::getButton(Uint16 x,Uint16 y)
{
    SDL_Rect tmp;

    for (Uint8 i=0;i<buttons.size();++i) {
        tmp = buttons[i]->getRect();
        if (x>=tmp.x && y>=tmp.y && x<(tmp.x+tmp.w) && y<(tmp.y+tmp.h)) {
            return i;
        }
    }
    return 255;
}

/** 
 * Event handler for clicking buttons.
 *
 * @bug This is an evil hack that should be replaced by something more flexible.
 */
void Sidebar::ClickButton(Uint8 index, char* unitname, createmode_t* createmode) 
{
    Uint8 f;
    
    f = buttons[index]->getFunction();    
    *createmode = CM_INVALID;
    
    switch (f&0x3) {
    case 0:
        return;
        break;
    case 1: // build
        Build(index - 3 - ((f&sbo_unit)?0:buildbut),(f&sbo_unit),unitname, createmode);
        break;
    case 2: // scroll
        DownButton(index);
        ScrollBuildList((f&sbo_up), (f&sbo_unit));
        break;
    default:
        logger->error("Sidebar::ClickButton. This should not happen (%i)\n",f&0x3);
        break;
    }
}

void Sidebar::ResetButton()
{
    if (!bd)
        return;
    bd = false;

    if (buttondown < 2){
        buttons[buttondown]->ChangeImage("stripup.shp",0);
    }else{
        buttons[buttondown]->ChangeImage("stripdn.shp",0);
    }
    buttondown = 0;
    UpdateIcons();
}

/**
 * Scroll the two icons list
 * 
 * @param scrollup if scrollup==true then scroll up the two icons list
 */
void Sidebar::ScrollSidebar(bool scrollup)
{
	// Scroll up the STRUCTURE icon list
    ScrollBuildList(scrollup, 0);
    // Scroll up the UNIT icon list
    ScrollBuildList(scrollup, 1);
}


void Sidebar::UpdateSidebar()
{	
	// update list of available icons
	UpdateAvailableLists();
	UpdateIcons();
	UpdatePowerbar();
}

void Sidebar::DrawPowerbar()
{
	SDL_Rect dest;
	SDL_Surface* Powerbar_up;
	SDL_Surface* Powerbar_down;
	//SDL_Surface* PowerIndicator;

	if (pc::Config.gamenum != GAME_RA || sbar == 0){
		return;
	}
	
	Powerbar_up = pc::imgcache->getImage(powerbar,0).image;
	dest.x	= 0;
	dest.y	= radarlocation.h;
	dest.w	= Powerbar_up->w;
	dest.h	= Powerbar_up->h;
	SDL_BlitSurface(Powerbar_up, 0, sbar, &dest);

	Powerbar_down = pc::imgcache->getImage(powerbar,1).image;
	dest.x	= 0;
	dest.y	= radarlocation.h + Powerbar_up->h;
	dest.w	= Powerbar_down->w;
	dest.h	= Powerbar_down->h;
	SDL_BlitSurface(Powerbar_down, NULL, sbar, &dest);
}

void Sidebar::UpdatePowerbar()
{
	static unsigned int	MaxPower = 500;
	SDL_Rect	 dest;
	SDL_Surface* Powerbar_up;
	SDL_Surface* Powerbar_down;
	SDL_Surface* PowerIndicator;
	Uint32		 IndicatorBarColor;
	Uint32		 IndicatorBarColorDark;
	static unsigned int 	OldPower, OldPowerused;
	static bool		LowPowerSoundPlayed = false;

	if (sbar == 0 || pc::Config.gamenum != GAME_RA)
		return;

	// Get the power status
	Player *lplayer = p::ppool->getLPlayer();
	unsigned int power = lplayer->getPower();
	unsigned int powerused = lplayer->getPowerUsed();

	// Check for change
	if (OldPower == power && OldPowerused	== powerused)
		return;

	// Handle playing the low power sound
	if (powerused > power-(power * 0.2) &&  powerused < power){
		if (!LowPowerSoundPlayed)
			pc::sfxeng->PlaySound("lopower1.aud");
		LowPowerSoundPlayed = true;
	}else{
		LowPowerSoundPlayed = false;
	}



//	printf ("Update powerbar\n");

	Powerbar_up = pc::imgcache->getImage(powerbar,0).image;
	Powerbar_down = pc::imgcache->getImage(powerbar,1).image;
	PowerIndicator = pc::imgcache->getImage(power_indicator,0).image;



	// Redraw the powerbar upper part (so it is clean again)
	dest.x	= 0;
	dest.y	= radarlocation.h;
	dest.w	= Powerbar_up->w;
	dest.h	= Powerbar_up->h;
	SDL_BlitSurface(Powerbar_up, NULL, sbar, &dest);

	// Redraw the powerbar lower part (so it is clean again)
	dest.x	= 0;
	dest.y	= radarlocation.h + Powerbar_up->h;
	dest.w	= Powerbar_down->w;
	dest.h	= Powerbar_down->h;
	SDL_BlitSurface(Powerbar_down, NULL, sbar, &dest);

	if (power > MaxPower)
		MaxPower = power * 4 + 500;

	if (powerused > MaxPower)
		MaxPower = powerused * 4 + 500;


	unsigned char PowerPercentage	= power * 100/ MaxPower;
	unsigned int MaxPowerHeight	= Powerbar_up->h + Powerbar_down->h - 31 - 46;
	unsigned int PowerHeight	= MaxPowerHeight * PowerPercentage/100;

	dest.x	= 11;
	dest.y	= radarlocation.h + 31 + MaxPowerHeight - PowerHeight;
	dest.w	= 2;
	dest.h	= PowerHeight;

	//
	// Handle the power indicator bar color
	//
	if (powerused > power-(power * 0.2) &&  powerused < power){
		// low power
		IndicatorBarColor	= SDL_MapRGB(sbar->format, 0xff, 0xa0, 0);
		IndicatorBarColorDark	= SDL_MapRGB(sbar->format, 0xff, 0x17, 0);
	}else if (powerused > power){
		// power off
		IndicatorBarColor	= SDL_MapRGB(sbar->format, 0xff, 0, 0);
		IndicatorBarColorDark	= SDL_MapRGB(sbar->format, 0xaa, 0, 0);
	}else{
		// power oke
		IndicatorBarColor	= SDL_MapRGB(sbar->format, 0, 0xff, 0);
		IndicatorBarColorDark	= SDL_MapRGB(sbar->format, 0, 0xaa, 0);
	}

	SDL_FillRect(sbar, &dest, IndicatorBarColor);
	dest.x	+= 2;
	SDL_FillRect(sbar, &dest, IndicatorBarColorDark);

	OldPower	 = power;
	OldPowerused = powerused;

	// Draw the power used indicator
	dest.x	= 2;
	dest.y	= radarlocation.h + 31 + MaxPowerHeight - (MaxPowerHeight * powerused / MaxPower) - PowerIndicator->h/2;
	dest.w	= PowerIndicator->w;
	dest.h	= PowerIndicator->h;
	SDL_BlitSurface(PowerIndicator, NULL, sbar, &dest);
}

Uint8 Sidebar::getSpecialButton(Uint16 x, Uint16 y)
{
	//RepairLoc, SellLoc, MapLoc;
	if (x>=RepairLoc.x && y>=RepairLoc.y && x<(RepairLoc.x+RepairLoc.w) && y<(RepairLoc.y+RepairLoc.h)) {
            return 1;
    }else if (x>=SellLoc.x && y>=SellLoc.y && x<(SellLoc.x+SellLoc.w) && y<(SellLoc.y+SellLoc.h)) {
            return 2;
	}else if (x>=MapLoc.x && y>=MapLoc.y && x<(MapLoc.x+MapLoc.w) && y<(MapLoc.y+MapLoc.h)) {
            return 3;
	}

    return 255;
}

void Sidebar::setSpecialButtonState(Uint8 Button, Uint8 State)
{
	//RepairLoc, SellLoc, MapLoc;
	switch (Button){
		case 1:
			if (State < 3)
				repair_but_state = State;
			break;
		case 2:
			if (State < 3)
				sell_but_state = State;
			break;
		case 3:
			if (State < 3)
				map_but_state = State;
			break;

	}
	
	// Force redraw of special Icons
	DrawSpecialIcons();
}

Uint8 Sidebar::getSpecialButtonState(Uint8 Button)
{
	//RepairLoc, SellLoc, MapLoc;
	switch (Button){
		case 1:
			return repair_but_state;
			break;
		case 2:
			return sell_but_state;
			break;
		case 3:
			return map_but_state;
			break;

	}
	return false;
}

void Sidebar::DrawSpecialIcons()
{
	//SDL_Rect	dest;
	SDL_Surface	*Repair;
	SDL_Surface* Sell;
	SDL_Surface* Map;
	int	IconSpacing = 45;

	if (sbar == 0) {
		/// @TODO Ensure we don't actually get called when this is the case
		return;
	}

	radarlocation.h = 160;

	if (pc::Config.gamenum == GAME_RA){

		if (repair_but_state < 3)
			Repair	= pc::imgcache->getImage(repair_icon,repair_but_state).image;
		else
			Repair	= pc::imgcache->getImage(repair_icon,0).image;
		RepairLoc.x	= 18;
		RepairLoc.y	= radarlocation.h - Repair->h +2;
		RepairLoc.w	= Repair->w;
		RepairLoc.h	= Repair->h;
		SDL_BlitSurface(Repair, NULL, sbar, &RepairLoc);

		if (sell_but_state < 3)
			Sell	= pc::imgcache->getImage(sell_icon,sell_but_state).image;
		else
			Sell	= pc::imgcache->getImage(sell_icon,0).image;

		SellLoc.x	= 18+IconSpacing;
		SellLoc.y	= radarlocation.h - Sell->h +2;
		SellLoc.w	= Sell->w;
		SellLoc.h	= Sell->h;
		SDL_BlitSurface(Sell, NULL, sbar, &SellLoc);

		if (map_but_state < 3)
			Map	= pc::imgcache->getImage(map_icon,map_but_state).image;
		else
			Map	= pc::imgcache->getImage(map_icon,3).image;

		MapLoc.x	= 18+2*IconSpacing;
		MapLoc.y	= radarlocation.h - Map->h +2;
		MapLoc.w	= Map->w;
		MapLoc.h	= Map->h;
		SDL_BlitSurface(Map, NULL, sbar, &MapLoc);
	}
}

void Sidebar::StartRadarAnim(Uint8 mode, bool* minienable)
{
    if (radaranimating == false && radaranim == 0 && sbar != 0)
    {
        radaranimating = true;
        radaranim = new RadarAnimEvent(mode, minienable, radaranimnumb);
    }
}

SDL_Surface *Sidebar::ReadShpImage(char *Name, int ImageNumb)
{
	SDL_Surface	*image;
	SDL_Surface *shadow;
	SHPImage	*TempPic;

	try {
		TempPic = new SHPImage(Name, -1);
	} catch (ImageNotFound&) {
		printf ("%s line %i: Image not found\n", __FILE__, __LINE__);
		return 0;
	}

	printf ("%s line %i: %s has %i images\n", __FILE__, __LINE__, Name, TempPic->getNumImg());

	TempPic->getImage(ImageNumb, &image, &shadow, 1);

	delete TempPic;

	SDL_FreeSurface(shadow);

	return image;
}

void Sidebar::SetupButtons(Uint16 height)
{
	const char* tmpname;
	Uint16 scrollbase;
	Uint8 t;
	int ButtonXpos;
	Uint32 startoffs;

	if (pc::Config.gamenum == GAME_TD){
		ButtonXpos	= 10;
		startoffs	= tablocation.h + radarlocation.h;
	}else{
		ButtonXpos	= 25;
		startoffs	= radarlocation.h + 5;
	}
	SHPImage *strip;

	tmpname = VFSUtils::VFS_getFirstExisting(3,"stripna.shp","hstrip.shp","strip.shp");
	if (tmpname == 0) {
		logger->error("Unable to find strip images for sidebar, exiting\n");
		throw SidebarError();
	}
	try {
		strip = new SHPImage(tmpname, -1);
	} catch (ImageNotFound&) {
		logger->error("Unable to load strip images for sidebar, exiting\n");
		throw SidebarError();
	}

	geom.bh = strip->getHeight();
	geom.bw = strip->getWidth();

	delete strip;

	if (geom.bh > 100)
		geom.bh = geom.bh>>2;

	buildbut = ((height-startoffs)/geom.bh)-2;

	// Add the scroll up/down buttons
	if (pc::Config.gamenum == GAME_TD){
		startoffs += geom.bh;
		scrollbase = startoffs + geom.bh*buildbut;
		AddButton(ButtonXpos+geom.bw,scrollbase,"stripup.shp",sbo_scroll|sbo_unit|sbo_up,0); // 0
		AddButton(ButtonXpos,scrollbase,"stripup.shp",sbo_scroll|sbo_structure|sbo_up,0); // 1

		AddButton(ButtonXpos+geom.bw+(geom.bw>>1),scrollbase,"stripdn.shp",sbo_scroll|sbo_unit|sbo_down,0); // 2
		AddButton(ButtonXpos+(geom.bw>>1),scrollbase,"stripdn.shp",sbo_scroll|sbo_structure|sbo_down,0); // 3
	}else if (pc::Config.gamenum == GAME_RA){
		scrollbase = startoffs + geom.bh*buildbut;
		AddButton(ButtonXpos+geom.bw,scrollbase,"stripup.shp",sbo_scroll|sbo_unit|sbo_up,0); // 0
		AddButton(ButtonXpos,scrollbase,"stripup.shp",sbo_scroll|sbo_structure|sbo_up,0); // 1

		AddButton(ButtonXpos+geom.bw+(geom.bw>>1),scrollbase,"stripdn.shp",sbo_scroll|sbo_unit|sbo_down,0); // 2
		AddButton(ButtonXpos+(geom.bw>>1),scrollbase,"stripdn.shp",sbo_scroll|sbo_structure|sbo_down,0); // 3
	}

    // The order in which the AddButton calls are made MUST be preserved
    // Two loops are made so that all unit buttons and all structure buttons
    // are grouped contiguously (4,5,6,7,...) compared to (4,6,8,10,...)
    for (t=0;t<buildbut;++t) {
        if (pc::Config.gamenum == GAME_RA){
		if ((player->getSide()&~PS_MULTI) == PS_BAD)
			AddButton(ButtonXpos+geom.bw,startoffs+geom.bh*t,"stripus.shp", sbo_build|sbo_unit,0);
		else
			AddButton(ButtonXpos+geom.bw,startoffs+geom.bh*t,"stripna.shp",sbo_build|sbo_unit,0);
        }else
		AddButton(ButtonXpos+geom.bw,startoffs+geom.bh*t,"strip.shp",sbo_build|sbo_unit,0);
    }
    for (t=0;t<buildbut;++t) {
        if (pc::Config.gamenum == GAME_RA){
		if ((player->getSide()&~PS_MULTI) == PS_BAD)
			AddButton(ButtonXpos,startoffs+geom.bh*t,"stripus.shp",sbo_build|sbo_structure,spalnum);
		else
			AddButton(ButtonXpos,startoffs+geom.bh*t,"stripna.shp",sbo_build|sbo_structure,spalnum);
          }else
		AddButton(ButtonXpos,startoffs+geom.bh*t,"strip.shp",sbo_build|sbo_structure,spalnum);
    }

    startoffs += geom.bh;

    // Update Available Lists of option
    UpdateAvailableLists();
    // Update icons
    UpdateIcons();
    
    // Hide the sidebars if they are no object
    if (uniticons.empty() && structicons.empty()) {
        visible = false;
        vischanged = true;
    }
}

/**
 * Scroll icons 
 * 
 * @param dir direction to scroll (0==down 0!=up)
 * @param type which icon list (0==Structure(first) 0!=Units(second))
 */
void Sidebar::ScrollBuildList(Uint8 dir, Uint8 type) 
{
    Uint8* offptr;
    vector<char*>* vecptr;

    if (type) {
        offptr = &unitoff;
        vecptr = &uniticons;
    } else {
        offptr = &structoff;
        vecptr = &structicons;
    }

    if (dir) { //up
        if (*offptr>0) {
            --(*offptr);
        }
    } else { // down
        if ((unsigned)(*offptr+buildbut) < (unsigned)vecptr->size()) {
            ++(*offptr);
        }
    }

    // Updates Icons 
    // (to draw only icons which are in scrolling area)
    UpdateIcons();
}

void Sidebar::Build(Uint8 index, Uint8 type, char* unitname, createmode_t* createmode)
{
    Uint8* offptr;
    vector<char*>* vecptr;

    if (type) {
        offptr = &unitoff;
        vecptr = &uniticons;
    } else {
        offptr = &structoff;
        vecptr = &structicons;
        *createmode = CM_STRUCT;
    }

    if ( (unsigned)(*vecptr).size() > ((unsigned)(*offptr)+index - 1) ) {
        strncpy(unitname,(*vecptr)[(*offptr+index-1)],13);
        unitname[strlen((*vecptr)[(*offptr+index-1)])-8] = 0x0;
    } else {
        // Out of range
        return;
    }
        
    if (*createmode != CM_STRUCT) {
        // createmode was set to CM_INVALID in the caller of this function
        UnitType* utype = p::uspool->getUnitTypeByName(unitname);
        if (0 != utype) {
            *createmode = (createmode_t)utype->getType();
        }
    }
}

/** 
 * Sets the images of the visible icons, having scrolled.
 * 
 * @TODO Provide a way to only update certain icons
 */
void Sidebar::UpdateIcons() 
{
    Uint8 i;

	// Unit buttons
	for (i=0;i<buildbut;++i) {
		if ((unsigned)(i+unitoff)>=(unsigned)uniticons.size()) {
			if (pc::Config.gamenum == GAME_RA){
				if ((player->getSide()&~PS_MULTI) == PS_BAD)
					buttons[4+i]->ChangeImage("stripus.shp", 0, 2);
				else
					buttons[4+i]->ChangeImage("stripna.shp", 0, 2);
			}else
				buttons[4+i]->ChangeImage("strip.shp");
		} else {
			buttons[4+i]->ChangeImage(uniticons[i+unitoff]);
		}
	}

	// Structure buttons
	for (i=0;i<buildbut;++i) {
		if ((unsigned)(i+structoff)>=(unsigned)structicons.size()) {
			if (pc::Config.gamenum == GAME_RA){
				if ((player->getSide()&~PS_MULTI) == PS_BAD)
					buttons[buildbut+4+i]->ChangeImage("stripus.shp", 0, 1);
				else
					buttons[buildbut+4+i]->ChangeImage("stripna.shp", 0, 1);
			}else
				buttons[buildbut+4+i]->ChangeImage("strip.shp");
		} else {
			buttons[buildbut+4+i]->ChangeImage(structicons[i+structoff]);
		}
	}

	// Draw the others button
	for (Sint8 x=buttons.size()-1;x>=0;--x) {
		DrawButton(x);
	}
}

/** 
 * Rebuild the list of icons that are available.
 *
 * - first get list of availlable
 * - control and reset offsets
 * - play "new" sound if good
 * - add icon in fonction
 * 
 * @bug Newer items should be appended, although with some grouping (i.e. keep
 * infantry at top, vehicles, aircraft, boats, then superweapons).
 * TODO implement superweapons
 */
void Sidebar::UpdateAvailableLists() 
{
	vector<const char*> units_avail; // list of units available (temp)
	vector<const char*> structs_avail; // list of structures available (temp)
	vector<const char*> superWeapons_avail; // list of superWeapons available (temp)
	char* nametemp;    
	Uint32 i;

    // Get UNITS availlable (like "3tnk","typ2" ... etc)
    units_avail = p::uspool->getBuildableUnits(player);
    
    // Get STRUCTURES availlable
    structs_avail = p::uspool->getBuildableStructures(player);
    
    // Get SUPERWEAPONS availlable
    // TODO get super weapon like that superWeapon_avail = p::uspool->getBuildableStructures(player);
    //superWeapons_avail.push_back("gpss"); // emulate TODO
    
    //
    // CONTROL AND RESETS OFFSETS
    //
    // if their are less options available then offset = 0 
    // LIKE IN ORIGINAL GAME !!!
    if (  (units_avail.size() + superWeapons_avail.size()) < uniticons.size() ) {
    	// OFFSET = 0
    	unitoff = 0;
    }
    // if their are less options available then offset = 0 
    if (structs_avail.size() < structicons.size()){
    	structoff = 0;
    }
    
    //
    // PLAY "NEW" SOUND IF GOOD
    //
    if ((units_avail.size() + superWeapons_avail.size()) > uniticons.size() ||
    	(structs_avail.size() > structicons.size()) ) 
    {
 		// Play new sound
 		pc::sfxeng->PlaySound("newopt1.aud");
 	}
 	
    //
    // FREE LIST OF AVAILABLE
    //    
    // Free list of availlable icons
    // Delete all icons in the second list (unit)
    for (i=0; i<uniticons.size(); ++i) {
    	delete[] uniticons[i];
    }
    // resize the vector
    uniticons.resize(0);
        	
    // Free list of availlable icons
    // Delete all icons in the first list (structure)
    for (i=0;i<structicons.size();++i) {
    	delete[] structicons[i];
    }
    // resize the vector
    structicons.resize(0);
    
    
    //  
    // ADD UNITS at icons
    //
    // Add icons
    for (i=0;i<units_avail.size();++i) {
        nametemp = new char[13];
        memset(nametemp,0x0,13);
        sprintf(nametemp,"%sICON.SHP",units_avail[i]);
        uniticons.push_back(nametemp);    
    }
    
    //
    // ADD STRUCTURES at icons
    //
    // Add icons
    for (i=0;i<structs_avail.size();++i) {
    	nametemp = new char[13];
    	memset(nametemp,0x0,13);
    	sprintf(nametemp, "%sICON.SHP", structs_avail[i]);
    	structicons.push_back(nametemp);
    }
    
    // 
    // ADD SUPERWEAPONS at icons
    //
    // Add icon
    for (i=0; i<superWeapons_avail.size(); ++i) {
    	nametemp = new char[13];
    	memset(nametemp,0x0,13);
    	sprintf(nametemp,"%sICON.SHP", superWeapons_avail[i]);
    	uniticons.push_back(nametemp);
    }
    
    
    // if all stack are empty then sidebar = hide
    if (uniticons.empty() && structicons.empty()) {
        visible = false;
    } else {
    	visible = true;    	
    }
    
    // Notify that something had changed
    vischanged = true;
}

void Sidebar::DownButton(Uint8 index)
{
    if (index>3){
        return; // not a scroll button
    }
    buttondown = index;

    bd = true;
    if (index == 0 || index == 1){
        buttons[index]->ChangeImage("stripup.shp",1);
    }
    else
    {
        buttons[index]->ChangeImage("stripdn.shp",1);
    }
    
    // Update the button because some changes
    UpdateIcons();
}

void Sidebar::AddButton(Uint16 x, Uint16 y, const char* fname, Uint8 f, Uint8 pal)
{
    SidebarButton* t;
    
    // Create the button
    t = new SidebarButton(x, y, fname, f, theatre, pal);
    
    // Add it to the buttons list
    buttons.push_back(t);
    
    // Notify that their was changes ????
    vischanged = true;
}

void Sidebar::DrawButton(Uint8 index)
{
	SDL_Rect src;

	if (sbar == 0) {
		/// @TODO Ensure we don't actually get called when this is the case
		return;
	}
	SDL_Rect dest = buttons[index]->getRect();
//	dest.y += 10;
	SDL_FillRect(sbar, &dest, 0x0);
	Uint8 func = buttons[index]->getFunction();

	// Blit the button's image onto the sidebar surface
	SDL_Surface *temp = buttons[index]->getSurface();
 	if (temp->h > 100) {
		int ButtonYpos = (dest.y - radarlocation.h - 5)/*/dest.h*/;
		//printf ("ButtonYpos = %i\n", ButtonYpos);

		src.x = 0;
		src.y = ButtonYpos/* * dest.h*/;
		src.w = temp->w;
		src.h = dest.h;
		SDL_BlitSurface(temp, &src, sbar, &dest);
	}else{
    	SDL_BlitSurface(temp, NULL, sbar, &dest);
	}
	vischanged = true;
	// Skip scroll buttons
	if (index < 4) {
		return;
	}
	// Calculate which icon was clicked on
	Uint8 offset = index - 4 + // First four buttons are scroll buttons
			((func&sbo_unit)
			? unitoff  // Unit buttons are created first
			: (structoff-buildbut) // See comment around line 265
			);
	vector<char*>& icons = ((func&sbo_unit)
				?(uniticons)
				:(structicons));

	if (offset >= icons.size()) {
		return;
	}

    // Extract type name from icon name, e.g. NUKE from NUKEICON.SHP
    Uint32 length = strlen(icons[offset])-8; 
    if (length>13) length = 13;
    string name(icons[offset], length);

    static const char* stat_mesg[] = {
        "???",		// BQ_INVALID
        "",			// BQ_EMPTY
        "",			// BQ_RUNNING
        "hold",		// BQ_PAUSED
        "ready",	// BQ_READY
		"",			// BQ_CANCELLED
		""			// BQ_ALL_PAUSED
    };
    static struct {Uint32 x,y;} stat_pos[5];
    static bool posinit = false;
    if (!posinit) {
        for (Uint8 i = 0; i < 5; ++i) {
            // Here we assume that all sidebar icons are the same size
            // (which is pretty safe)
            stat_pos[i].x = (dest.w - StatusLabel.getWidth(stat_mesg[i])) >> 1;
            stat_pos[i].y = dest.h - 15;
        }
        posinit = true;
    }
    ConStatus status;
    Uint8 quantity, progress, imgnum;

    UnitOrStructureType* type;
    if (func&sbo_unit) {
        type = p::uspool->getUnitTypeByName(name.c_str());
    } else {
        type = (UnitOrStructureType*)p::uspool->getStructureTypeByName(name.c_str());
    }
    
    // TODO REFACTOR THIS
    if (0 == type) {
//        getFont()->drawText(stat_mesg[status], sbar, dest.x + stat_pos[status].x, dest.y + stat_pos[status].y);
   		//StatusLabel.Draw(stat_mesg[status], sbar, dest.x + stat_pos[status].x, dest.y + stat_pos[status].y);
        // Grey out invalid items for prettyness
        DrawClock(index, 0);        
        // return instead of trowing
        return;
        //throw runtime_error("Asking for \""+name+"\" resulted in a null pointer");
    }
    
    status = player->getStatus(type, &quantity, &progress);
    if (BQ_INVALID == status) {
//        getFont()->drawText(stat_mesg[status], sbar, dest.x + stat_pos[status].x, dest.y + stat_pos[status].y);
		StatusLabel.Draw(stat_mesg[status], sbar, dest.x + stat_pos[status].x, dest.y + stat_pos[status].y);
        // Grey out invalid items for prettyness
        DrawClock(index, 0);
        return;
    }
    imgnum = (steps*progress)/100;
    if (100 != progress) {
        DrawClock(index, imgnum);
    }
    if (quantity > 0) {
        char tmp[4];
        sprintf(tmp, "%i", quantity);
        //getFont()->drawText(tmp, sbar, dest.x+3, dest.y+3);
		QuantityLabel.Draw(tmp, sbar, dest.x+3, dest.y+3);
    }
    /// @TODO This doesn't work when you immediately pause after starting to
    // build (but never draws the clock for things not being built).
    if (progress > 0) {
		StatusLabel.Draw(stat_mesg[status], sbar, dest.x + stat_pos[status].x, dest.y + stat_pos[status].y - 10);
    }

}

SDL_Surface* Sidebar::getTabImage() 
{
	// Get image from the cache
	return pc::imgcache->getImage(tab).image;
}

void Sidebar::DrawClock(Uint8 index, Uint8 imgnum) 
{
    Uint32 num = 0;
	SDL_Rect dest;

#if 0
    try {
        // @TODO Move this check to config object
        if (pc::Config.gamenum == GAME_RA || isoriginaltype) {
            num = pc::imgcache->loadImage("clock.shp");
        } else {
            num = pc::imgcache->loadImage("hclock.shp");
        }
    } catch (ImageNotFound& e) {
        logger->error("Unable to load clock image!\n");
        return;
    }
    num += imgnum;
    num |= spalnum<<11;

    ImageCacheEntry& ice = pc::imgcache->getImage(num);
    SDL_Surface* gr = ice.image;

    FixGrey(gr, imgnum);

    SDL_SetAlpha(gr, SDL_SRCALPHA, 128);

    SDL_Rect dest = buttons[index]->getRect();

    SDL_BlitSurface(gr, NULL, sbar, &dest);
#else

	//
	// This had to be done differently because of the use of glSDL
	//
	///TODO: the clocks cache should be reset after losing focus (so free all images and set all pointers to NULL)
	if (Clocks[imgnum] == NULL){
		try {
			// @TODO Move this check to config object
			if (pc::Config.gamenum == GAME_RA || isoriginaltype) {
				num = pc::imgcache->loadImage("clock.shp");
			} else {
				num = pc::imgcache->loadImage("hclock.shp");
			}
		} catch (ImageNotFound& e) {
			logger->error("Unable to load clock image!\n");
			return;
		}
		num += imgnum;
		num |= spalnum<<11;

		ImageCacheEntry& ice = pc::imgcache->getImage(num);
		SDL_Surface* gr = ice.image;

		Clocks[imgnum] = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, gr->w, gr->h, 32, 0, 0, 0, 0);
		Uint32 ColorKey = SDL_MapRGB(Clocks[imgnum]->format, 12, 255, 12 );
		SDL_Rect dest;
		dest.x = 0;
		dest.y = 0;
		dest.w = Clocks[imgnum]->w;
		dest.h = Clocks[imgnum]->h;

		Uint32 color;

		for (int x =0; x < gr->w; x++){
			for (int y = 0; y < gr->h; y++){
				SDLLayer::get_pixel(gr, color, x, y);
				if (color != 1344){
					SDLLayer::set_pixel(Clocks[imgnum], ColorKey, x, y);
				}else{
					SDLLayer::set_pixel(Clocks[imgnum], 0x101010U, x, y);
				}
			}
		}

		SDL_SetColorKey(Clocks[imgnum],SDL_SRCCOLORKEY|SDL_RLEACCEL, ColorKey);
		SDL_SetAlpha(Clocks[imgnum], SDL_SRCALPHA|SDL_RLEACCEL, 128);

		SDL_Surface * tmp;
		tmp = SDL_DisplayFormatAlpha(Clocks[imgnum]);
		SDL_FreeSurface(Clocks[imgnum]);
		Clocks[imgnum] = tmp;


	}
	dest = buttons[index]->getRect();
	SDL_BlitSurface(Clocks[imgnum], 0, sbar, &dest);
#endif
}

SDL_Rect* Sidebar::getTabLocation()
{
	return &tablocation;
}

bool Sidebar::isOriginalType()
{
	return isoriginaltype;
}

Uint8 Sidebar::getSteps() const
{
	return steps;
}

const SidebarGeometry& Sidebar::getGeom() 
{
	return geom;
}

Sidebar::Sidebar()
{
}

Sidebar::Sidebar(const Sidebar&) 
{
}

Sidebar& Sidebar::operator=(const Sidebar&) 
{
	return *this;
}

bool Sidebar::getVisible()
{
	return visible;
}

#if 0
SDL_Surface* Sidebar::FixGrey(SDL_Surface* gr, Uint8 imgnum)
{
    if (greyFixed[imgnum]) {
        return NULL;
    }

    SDL_LockSurface(gr);

    if(gr->format->BytesPerPixel == 4) {
        Uint32 *pixels = (Uint32 *)gr->pixels;
        Uint32 size = ((gr->h-1) * (gr->pitch >> 2)) + gr->w-1;
        replace(pixels, pixels+size, 0xa800U, 0x10101U);
    } else if(gr->format->BytesPerPixel == 2) {
        Uint16 *pixels = (Uint16 *)gr->pixels;
        Uint32 size = ((gr->h-1) * (gr->pitch >> 1)) + gr->w-1;
        replace(pixels, pixels+size, 0x540U, 0x21U);
    }
    SDL_UnlockSurface(gr);

    greyFixed[imgnum] = true;
	return NULL;
}
#endif

