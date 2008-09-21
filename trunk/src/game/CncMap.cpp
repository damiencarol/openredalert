// CnCMap.cpp
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

#include "CnCMap.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <stdexcept>

#include "Game.h"
#include "Player.h"
#include "UnitAndStructurePool.h"
#include "MissionData.h"
#include "CellTrigger.h"
#include "Unit.h"
#include "LoadMapError.h"
#include "GameMode.h"
#include "TriggerAction.h"
#include "NoActionTriggerAction.h"
#include "GlobalSetTriggerAction.h"
#include "GlobalClearTriggerAction.h"
#include "TextTriggerAction.h"
#include "RawTriggerAction.h"
#include "misc/Compression.hpp"
#include "misc/KeyNotFound.h"
#include "misc/INIFile.h"
#include "misc/StringTableFile.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "video/ImageNotFound.h"
#include "video/ImageCache.h"
#include "video/LoadingScreen.h"
#include "video/SHPImage.h"
#include "video/TemplateImage.h"
#include "include/common.h"
#include "include/config.h"
#include "include/imageproc.h"
#include "include/Logger.h"
#include "PlayerPool.h"
#include "triggers.h"
#include "AiCommand.h"

using std::vector;
using std::string;
using std::stringstream;
using std::map;
using std::runtime_error;
using std::ostringstream;

using INI::KeyNotFound;

namespace pc
{
	extern ImageCache* imgcache;
	extern ConfigType Config;
	extern vector<SHPImage*>* imagepool;
	extern MessagePool* msg;
}
namespace p {
	extern PlayerPool* ppool;
}
extern Logger * logger;

//-----------------------------------------------------------------------------
// Functors
//-----------------------------------------------------------------------------

struct TemplateCacheCleaner :
	public std::unary_function<TemplateCache::value_type, void>
{
	void operator()(const TemplateCache::value_type& p)
	{
		if (p.second != NULL)
		{
			delete p.second;
		}
		//p.second = NULL;
	}
};

struct TemplateTileCacheCleaner :
	public std::unary_function<TemplateTileCache::value_type, void>
{
	void operator()(const TemplateTileCache::value_type& p)
	{
		if (p != NULL)
			delete p;
		//p = NULL;
	}
};

//-----------------------------------------------------------------------------
// CnCMap Methods
//-----------------------------------------------------------------------------

Uint32 CnCMap::getOverlay(Uint32 pos)
{
	if (overlaymatrix[pos] & HAS_OVERLAY)
		return overlays[pos]<<16;
	return 0;
}

Uint32 CnCMap::getTerrain(Uint32 pos, Sint16* xoff, Sint16* yoff)
{
	Uint32 terrain = 0;

	if (overlaymatrix[pos] & HAS_TERRAIN)
	{
		terrain = terrains[pos].shpnum;
		*xoff = terrains[pos].xoffset;
		*yoff = terrains[pos].yoffset;
	}
	return terrain;
}

/**
 * Sets up things that don't depend on the map being loaded.
 */
CnCMap::CnCMap()
{
	/**		Somehow the imagepool gets corrupted by the menu (widgets)
	 --> create a new one
	 pc::imgcache->flush(); doesn't help
	 */
	//    pc::imagepool = new std::vector<SHPImage*>();
	//    pc::imgcache->setImagePool(pc::imagepool);
	for (int i = 0; i < 100; i++)
	{
		waypoints[i] = 0;
	}
	//    this->InitCnCMap ();
	fmax = (double)maxscroll/100.0;

	// Build blank MissionData
	this->missionData = new MissionData();
}

void CnCMap::Init(gametypes gameNumber, Uint8 gameMode)
{
	// notify
	loaded = false;


	//this->maptype = pc::Config.gamenum;
	this->maptype = gameNumber;
	//this->gamemode = pc::Config.gamemode;
	this->gamemode = gameMode;

	scrollstep = pc::Config.scrollstep;
	scrolltime = pc::Config.scrolltime;
	maxscroll = pc::Config.maxscroll;

	// start at top right corner of map.
	// the startpos for the map is stored in position 0
	scrollpos.curx = 0;
	scrollpos.cury = 0;
	scrollpos.curxtileoffs = 0;
	scrollpos.curytileoffs = 0;

	scrollvec.x = 0;
	scrollvec.y = 0;
	scrollvec.t = 0;
	toscroll = false;
	for (Uint8 i=0; i<NUMMARKS; ++i)
	{
		scrollbookmarks[i].x = 0;
		scrollbookmarks[i].y = 0;
		scrollbookmarks[i].xtile = 0;
		scrollbookmarks[i].ytile = 0;
	}
	minimap = 0;
	oldmmap = 0;
	loading = false;

	// If game == RA   =>     translate_64 = false
	//if (pc::Config.gamenum == GAME_TD){
	//	translate_64 = true;
	//} else {
	// WITH RED ALERT translate_64 = false
	// (in fact we must translate in 128 (see
	translate_64 = false;
	//}

}

/**
 * Destructor, free up some memory
 */
CnCMap::~CnCMap()
{
	Uint32 i;

	for (i = 0; i < tileimages.size(); i++)
	{
		if (tileimages[i] != NULL)
			SDL_FreeSurface(tileimages[i]);
		tileimages[i] = NULL;
	}
	tileimages.clear();

	// Empty the cache of TemplateImage* & Tile
	for_each(templateTileCache.begin(), templateTileCache.end(),
			TemplateTileCacheCleaner());

	// Empty the pool of TemplateImage*
	for_each(templateCache.begin(), templateCache.end(), TemplateCacheCleaner());

	if (p::uspool != NULL)
		delete p::uspool;
	p::uspool = NULL;

	if (p::ppool != NULL)
		delete p::ppool;
	p::ppool = NULL;
	if (minimap != NULL)
		SDL_FreeSurface(minimap);
	minimap = NULL;
	for (i = 0; i < numShadowImg; i++)
	{
		if (shadowimages[i] != NULL)
			SDL_FreeSurface(shadowimages[i]);
		shadowimages[i] = NULL;
	}
}

/**
 * @todo Map loading goes here.
 */
void CnCMap::loadMap(const char* mapname, LoadingScreen* lscreen)
{
	// Copy the map name
	missionData->mapname = string(mapname);

	string message = "Reading ";
	message += mapname;
	message += ".ini";
	// Log it
	logger->debug("%s\n", message.c_str());

	loading = true;
	// Load the ini part of the map
	if (lscreen != 0)
	{
		lscreen->setCurrentTask(message);
	}

	// Load the ini part of the map
	loadIni();

	// Set the alliances between players
	p::ppool->setAlliances();

	loading = false;
	loaded = true;
}

/**
 * Sets the scroll to the specified direction.
 *
 * @param direction to scroll in.
 */
Uint8 CnCMap::accScroll(Uint8 direction)
{
	bool validx = false;
	bool validy = false;

	if (direction & s_up)
	{
		if (scrollvec.y >= 0)
			scrollvec.y = -scrollstep;
		else if (scrollvec.y > -maxscroll)
			scrollvec.y -= scrollstep;
		validy = (valscroll & s_up);
		if (!validy)
		{
			scrollvec.y = 0;
			direction ^= s_up;
		}
	}
	if (direction & s_down)
	{
		if (scrollvec.y <= 0)
			scrollvec.y = scrollstep;
		else if (scrollvec.y < maxscroll)
			scrollvec.y += scrollstep;
		validy = (valscroll & s_down);
		if (!validy)
		{
			scrollvec.y = 0;
			direction ^= s_down;
		}
	}
	if (direction & s_left)
	{
		if (scrollvec.x >= 0)
			scrollvec.x = -scrollstep;
		else if (scrollvec.x > -maxscroll)
			scrollvec.x -= scrollstep;
		validx = (valscroll & s_left);
		if (!validx)
		{
			scrollvec.x = 0;
			direction ^= s_left;
		}
	}
	if (direction & s_right)
	{
		if (scrollvec.x <= 0)
			scrollvec.x = scrollstep;
		else if (scrollvec.x < maxscroll)
			scrollvec.x += scrollstep;
		validx = (valscroll & s_right);
		if (!validx)
		{
			scrollvec.x = 0;
			direction ^= s_right;
		}
	}
	if (validx || validy)
	{
		scrollvec.t = 0;
		toscroll = true;
	}
	return direction;
}

Uint8 CnCMap::absScroll(Sint16 dx, Sint16 dy, Uint8 border)
{
	Uint8 direction = s_none;
	bool validx = false, validy = false;
	if (dx <= -border)
	{
		validx = (valscroll & s_left);
		if (validx)
		{
			scrollvec.x = (Sint8)(min(dx, (Sint16)100) * fmax);
			direction |= s_left;
		}
		else
		{
			scrollvec.x = 0;
		}
	}
	else if (dx >= border)
	{
		validx = (valscroll & s_right);
		if (validx)
		{
			scrollvec.x = (Sint8)(min(dx, (Sint16)100) * fmax);
			direction |= s_right;
		}
		else
		{
			scrollvec.x = 0;
		}
	}
	if (dy <= -border)
	{
		validy = (valscroll & s_up);
		if (validy)
		{
			scrollvec.y = (Sint8)(min(dy, (Sint16)100) * fmax);
			direction |= s_up;
		}
		else
		{
			scrollvec.y = 0;
		}
	}
	else if (dy >= border)
	{
		validy = (valscroll & s_down);
		if (validy)
		{
			scrollvec.y = (Sint8)(min(dy, (Sint16)100) * fmax);
			direction |= s_down;
		}
		else
		{
			scrollvec.y = 0;
		}
	}
	toscroll = (validx || validy);
	return direction;
}

/**
 * Scrolls according to the scroll vector.
 */
void CnCMap::doscroll()
{
	Sint32 xtile, ytile;
	if (scrollpos.curx*scrollpos.tilewidth+scrollpos.curxtileoffs
			<= -scrollvec.x && scrollvec.x < 0)
	{
		scrollvec.t = 0;
		scrollvec.x = 0;
		scrollpos.curx = 0;
		scrollpos.curxtileoffs = 0;
	}
	if (scrollpos.cury*scrollpos.tilewidth+scrollpos.curytileoffs
			<= -scrollvec.y && scrollvec.y < 0)
	{
		scrollvec.t = 0;
		scrollvec.y = 0;
		scrollpos.cury = 0;
		scrollpos.curytileoffs = 0;
	}
	if (scrollpos.curx*scrollpos.tilewidth+scrollpos.curxtileoffs+scrollvec.x
			>= scrollpos.maxx*scrollpos.tilewidth+scrollpos.maxxtileoffs
			&& scrollvec.x > 0)
	{
		scrollvec.t = 0;
		scrollvec.x = 0;
		scrollpos.curx = scrollpos.maxx;
		scrollpos.curxtileoffs = scrollpos.maxxtileoffs;
	}
	if (scrollpos.cury*scrollpos.tilewidth+scrollpos.curytileoffs+scrollvec.y
			>= scrollpos.maxy*scrollpos.tilewidth+scrollpos.maxytileoffs
			&& scrollvec.y > 0)
	{
		scrollvec.t = 0;
		scrollvec.y = 0;
		scrollpos.cury = scrollpos.maxy;
		scrollpos.curytileoffs = scrollpos.maxytileoffs;
	}

	if ((scrollvec.x == 0) && (scrollvec.y == 0))
	{
		toscroll = false;
		setValidScroll();
		return;
	}
	xtile = scrollpos.curxtileoffs+scrollvec.x;
	while (xtile < 0)
	{
		scrollpos.curx--;
		xtile += scrollpos.tilewidth;
	}
	while (xtile >= scrollpos.tilewidth)
	{
		scrollpos.curx++;
		xtile -= scrollpos.tilewidth;
	}
	scrollpos.curxtileoffs = xtile;

	ytile = scrollpos.curytileoffs+scrollvec.y;
	while (ytile < 0)
	{
		scrollpos.cury--;
		ytile += scrollpos.tilewidth;
	}
	while (ytile >= scrollpos.tilewidth)
	{
		scrollpos.cury++;
		ytile -= scrollpos.tilewidth;
	}
	scrollpos.curytileoffs = ytile;

	++scrollvec.t;

	// scrolling continues at current rate for scrolltime
	// passes then decays quickly
	if (scrollvec.t >= scrolltime)
	{
		scrollvec.x /=2;
		scrollvec.y /=2;
	}
	setValidScroll();
}

/**
 * Sets the maximum value the scroll can take on.
 *
 * @param x the maximum x scroll.
 * @param y the maximum y scroll.
 */
void CnCMap::setMaxScroll(Uint32 x, Uint32 y, Uint32 xtile, Uint32 ytile,
		Uint32 tilew)
{
	scrollpos.maxx = 0;
	scrollpos.maxy = 0;
	scrollpos.maxxtileoffs = 0;
	scrollpos.maxytileoffs = 0;
	if (xtile > 0)
	{
		x++;
		xtile = tilew-xtile;
	}
	if (ytile > 0)
	{
		y++;
		ytile = tilew-ytile;
	}

	if (width > x)
	{
		scrollpos.maxx = width - x;
		scrollpos.maxxtileoffs = xtile;
	}
	if (height > y)
	{
		scrollpos.maxy = height - y;
		scrollpos.maxytileoffs = ytile;
	}

	scrollpos.tilewidth = tilew;

	if (scrollpos.curx > scrollpos.maxx)
	{
		scrollpos.curx = scrollpos.maxx;
		scrollpos.curxtileoffs = scrollpos.maxxtileoffs;
	}
	else if (scrollpos.curx == scrollpos.maxx && scrollpos.curxtileoffs
			> scrollpos.maxxtileoffs)
	{
		scrollpos.curxtileoffs = scrollpos.maxxtileoffs;
	}

	if (scrollpos.cury > scrollpos.maxy)
	{
		scrollpos.cury = scrollpos.maxy;
		scrollpos.curytileoffs = scrollpos.maxytileoffs;
	}
	else if (scrollpos.cury == scrollpos.maxy && scrollpos.curytileoffs
			> scrollpos.maxytileoffs)
	{
		scrollpos.curytileoffs = scrollpos.maxytileoffs;
	}

	if (scrollpos.curxtileoffs > scrollpos.tilewidth)
	{
		scrollpos.curxtileoffs = scrollpos.tilewidth;
	}
	if (scrollpos.curytileoffs > scrollpos.tilewidth)
	{
		scrollpos.curytileoffs = scrollpos.tilewidth;
	}

	setValidScroll();
}

/**
 *
 */
void CnCMap::setValidScroll()
{
	Uint8 temp = s_all;
	if (scrollpos.curx == 0 && scrollpos.curxtileoffs == 0)
	{
		temp &= ~s_left;
	}
	if (scrollpos.cury == 0 && scrollpos.curytileoffs == 0)
	{
		temp &= ~s_up;
	}
	if (scrollpos.curx == scrollpos.maxx && scrollpos.curxtileoffs
			== scrollpos.maxxtileoffs)
	{
		temp &= ~s_right;
	}
	if (scrollpos.cury == scrollpos.maxy && scrollpos.curytileoffs
			== scrollpos.maxytileoffs)
	{
		temp &= ~s_down;
	}
	valscroll = temp;
}

/**
 * Test if a unit can spawn at a position
 *
 * @param pos Position in the map
 */
bool CnCMap::canSpawnAt(Uint16 pos) const
{
	//    UnitOrStructure* uos = 0;

	// Can't build on tiberium
	if (getTiberium(pos) != 0)
	{
		return false;
	}

	// Check if the terrain would allow units to spawn here
	switch (terraintypes[pos])
	{
	case t_rock:
	case t_tree:
	case t_water_blocked:
	case t_other_nonpass:
		return false;
	case t_water:
		// Eventually will check if building is supposed to be in the water
		return false;
	case t_road:
	case t_land:
		if (p::uspool->tileAboutToBeUsed(pos))
		{
			return false;
		}
		//uos = p::uspool->getUnitOrStructureAt(pos,0x80,true);
		//if (uos == 0)
		if (!p::uspool->cellOccupied(pos))
			return true;
		return false;
	default:
		return false;
	}
	// Unreachable
}

/**
 * The excpUn is used for units the can be deployed
 */
bool CnCMap::isBuildableAt(Uint32 PlayerNumb, Uint16 pos, bool WaterBound) const
{
	//    UnitOrStructure* uos = 0;

	// Can't build where you haven't explored
	if (PlayerNumb == p::ppool->getLPlayerNum())
	{
		if (!p::ppool->getLPlayer()->getMapVis()[pos])
		{
			return false;
		}
	}

	// Can't build on tiberium
	//    if (getTiberium(pos) != 0) {
	if (getResourceFrame(pos) != 0)
	{
		return false;
	}

	// Check with terrain type
	switch (terraintypes[pos])
	{
	case t_rock:
	case t_tree:
	case t_water_blocked:
	case t_other_nonpass:
		return false;
	case t_water:
		if (WaterBound)
		{
			return true;
		}
		return false;
	case t_road:
	case t_land:
		if (p::uspool->tileAboutToBeUsed(pos))
		{
			return false;
		}
		//        uos = p::uspool->getUnitOrStructureAt(pos,0x80,true);
		//        if (uos == 0 && !WaterBound)
		if (!p::uspool->cellOccupied(pos) && !WaterBound)
			return true;
		return false;
	default:
		return false;
	}
	// Unreachable
}

/**
 * The excpUn is used for units the can be deployed
 */
bool CnCMap::isBuildableAt(Uint16 pos, Unit* excpUn) const
{
	Unit* uos = 0;
	// Can't build where you haven't explored
	if (!p::ppool->getLPlayer()->getMapVis()[pos])
	{
		return false;
	}
	// Can't build on tiberium
	//    if (getTiberium(pos) != 0) {
	if (getResourceFrame(pos) != 0)
	{
		return false;
	}
	switch (terraintypes[pos])
	{
	case t_rock:
	case t_tree:
	case t_water_blocked:
	case t_other_nonpass:
		return false;
	case t_water:
		// Eventually will check if building is supposed to be in the water
		return false;
	case t_road:
	case t_land:
		if (p::uspool->tileAboutToBeUsed(pos))
		{
			return false;
		}
		uos = p::uspool->getGroundUnitAt(pos, 0x80);
		if (uos == excpUn)
			return true;
		//        if (uos == 0)
		if (!p::uspool->cellOccupied(pos))
			return true;
		return false;
	default:
		return false;
	}
	// Unreachable
}

//
// The excpUn is used for units the can be deployed
//
bool CnCMap::isBuildableAt(Uint32 PlayerNumb, Uint16 pos, Unit* excpUn) const
{
	Unit* uos= NULL;

	// Can't build where you haven't explored
	if (PlayerNumb == p::ppool->getLPlayerNum())
	{
		if (!p::ppool->getLPlayer()->getMapVis()[pos])
		{
			return false;
		}
	}
	// Can't build on tiberium
	//    if (getTiberium(pos) != 0) {
	if (getResourceFrame(pos) != 0)
	{
		return false;
	}
	switch (terraintypes[pos])
	{
	case t_rock:
	case t_tree:
	case t_water_blocked:
	case t_other_nonpass:
		return false;
	case t_water:
		// Eventually will check if building is supposed to be in the water
		return false;
	case t_road:
	case t_land:
		if (p::uspool->tileAboutToBeUsed(pos))
		{
			return false;
		}
		/// By setting subpos to 0x80 (a actual invalid subpos) we indicate that we want the nearest infanty
		uos = p::uspool->getGroundUnitAt(pos, 0x80);
		if (uos == excpUn)
			return true;
		//        if (uos == 0)
		if (!p::uspool->cellOccupied(pos))
			return true;
		return false;
	default:
		return false;
	}
	// Unreachable
}

Uint16 CnCMap::getCost(Uint16 pos, Unit* excpUn) const
{
	Uint16 cost;
	bool WaterBound = false;
	bool AirBound = false;

	if (pos >= p::ccmap->getSize())
		return 0xffff;

#if 1
	if (excpUn != NULL)
	{
		if (excpUn->getOwner() == p::ppool->getLPlayerNum())
		{
			if ( !p::ppool->getLPlayer()->getMapVis()[pos] && (excpUn == 0 || excpUn->getDist(pos)>1 ))
			{
				if (!excpUn->IsAirBound())
					return 0;
				else
					return 0xffff;
			}
		}
		WaterBound = excpUn->IsWaterBound();
		AirBound = excpUn->IsAirBound();
	}
	else
	{
		if ( !p::ppool->getLPlayer()->getMapVis()[pos] /*&& (excpUn == 0 || excpUn->getDist(pos)>1 )*/)
		{
			return 0;
		}
	}
#endif

	/** @todo: Tile cost should worked out as follows
	 * if tmp == 1 then impassible
	 * else unitspeed * tmp
	 * where "tmp" is the terrain movement penalty.  This is a percentage of how much
	 * of the unit's speed is lost when using this terrain type for the
	 * unit's type of movement (foot, wheel, track, boat, air.
	 *
	 * Unitspeed is used as it might be of use if more heuristics are used
	 * when moving groups of units (e.g. either put slower moving units on
	 * terrain that lets them move faster to get mixed units to stick
	 * together or let faster moving units through a chokepoint first)
	 */
	switch (terraintypes[pos])
	{
	case t_rock:
	case t_tree:
	case t_water_blocked:
	case t_other_nonpass:
		if (AirBound)
			cost = p::uspool->getTileCost(pos, excpUn);
		else
			cost = 0xffff;
		break;
	case t_water:
		if (WaterBound || AirBound)
			cost = p::uspool->getTileCost(pos, excpUn);
		else
			cost = 0xffff;
		break;
	case t_road:
		if (WaterBound)
			cost = 0xffff;
		else
			cost = p::uspool->getTileCost(pos, excpUn);
		break;
	case t_land:
	default:
		if (WaterBound)
		{
			cost = 0xffff;
		}
		else
		{
			if (excpUn == NULL)
				cost = 1 + p::uspool->getTileCost(pos);
			else
				cost = 1 + p::uspool->getTileCost(pos, excpUn);
		}
		break;
	}
	/** @todo If unit prefers to be near tiberium (harvester) or should avoid
	 * it at all costs (infantry except chem.) apply appropriate bonus/penatly
	 * to cost.
	 */
	return cost;
}

#define RA_OFFSET	30

SDL_Surface* CnCMap::getMiniMap(Uint8 pixsize)
{
	static ImageProc ip;
	Uint16 tx, ty;
	SDL_Rect pos =
	{ 0, 0, pixsize, pixsize };
	SDL_Surface *cminitile;
	if (pixsize == 0)
	{
		// Argh
		logger->error("CnCMap::getMiniMap: pixsize is zero, resetting to one\n");
		pixsize = 1;
	}
	if (minimap != NULL)
	{
		if (minimap->w == width*pixsize)
		{
			return minimap;
		}
		else
		{
			// Each minimap surface is about 250k, so caching a lot of zooms
			// would be somewhat expensive.  Could make how much memory to set aside
			// for this customizable so people with half a gig of RAM can see some
			// usage :-)
			// For now, just keep the previous one.
			if (oldmmap != NULL)
				SDL_FreeSurface(oldmmap);
			oldmmap = minimap;
			minimap = NULL;
		}
	}
	if (oldmmap != NULL && (oldmmap->w == pixsize*width))
	{
		minimap = oldmmap;
		oldmmap = NULL;
		return minimap;
	}
	minimap
			= SDL_CreateRGBSurface(SDL_SWSURFACE, width*pixsize,height*pixsize, 16,
			0xff, 0xff, 0xff, 0);
	SDL_Surface *maptileOne = getMapTile(0);
	minimap->format->Rmask = maptileOne->format->Rmask;
	minimap->format->Gmask = maptileOne->format->Gmask;
	minimap->format->Bmask = maptileOne->format->Bmask;
	minimap->format->Amask = maptileOne->format->Amask;
	if (maptileOne->format->palette != NULL)
	{
		SDL_SetColors(minimap, maptileOne->format->palette->colors, 0,
				maptileOne->format->palette->ncolors);
	}
	int lineCounter = 0;
	for (Uint32 i = 0; i < (Uint32) width*height; i++, pos.x += pixsize,
			lineCounter++)
	{
		if (lineCounter == width)
		{
			pos.y += pixsize;
			pos.x = 0;
			lineCounter = 0;
		}
		cminitile = ip.minimapScale(getMapTile(i), pixsize);
		SDL_SetColorKey(cminitile, SDL_SRCCOLORKEY, 0xffffff);
		SDL_BlitSurface(cminitile, NULL, minimap, &pos);
		if (cminitile != NULL)
			SDL_FreeSurface(cminitile);
		cminitile = NULL;
	}
	// Now fill in clipping details for renderer and UI.
	// To make things easier, ensure that the geometry
	// is divisable by the specified width and height.
	tx = min(miniclip.sidew, (Uint16)minimap->w);
	ty = min(tx, (Uint16)minimap->h);
	// w == width in pixels of the minimap

	miniclip.w = pixsize*(int)floor((double)tx/(double)pixsize);
	miniclip.h = pixsize*(int)floor((double)ty/(double)pixsize);
	if (pc::Config.gamenum == GAME_RA && miniclip.w -RA_OFFSET > 0
			&& miniclip.h - RA_OFFSET > 0)
	{
		miniclip.w = miniclip.w - RA_OFFSET;
		miniclip.h = miniclip.h - RA_OFFSET - 20;
	}
	// x == offset in pixels from the top-left hand corner of the sidebar under
	// the tab.
	miniclip.x = abs(miniclip.sidew-miniclip.w)/2;
	if (this->maptype == GAME_RA){
		miniclip.y = abs(miniclip.sidew - 20 - miniclip.h)/2;
	}else{
		miniclip.y = abs(miniclip.sidew - miniclip.h)/2;
	}

	// Tilew == number of tiles visible in minimap horizontally
	miniclip.tilew = miniclip.w/pixsize;
	miniclip.tileh = miniclip.h/pixsize;
	// pixsize == number of pixels wide and high a minimap tile is
	miniclip.pixsize = pixsize;
	return minimap;
}

void CnCMap::storeLocation(Uint8 loc)
{
	if (loc >= NUMMARKS)
	{
		return;
	}
	scrollbookmarks[loc].x = scrollpos.curx;
	scrollbookmarks[loc].y = scrollpos.cury;
	scrollbookmarks[loc].xtile = scrollpos.curxtileoffs;
	scrollbookmarks[loc].ytile = scrollpos.curytileoffs;
}

void CnCMap::restoreLocation(Uint8 loc)
{
	if (loc >= NUMMARKS)
	{
		return;
	}
	scrollpos.curx = scrollbookmarks[loc].x;
	scrollpos.cury = scrollbookmarks[loc].y;
	scrollpos.curxtileoffs = scrollbookmarks[loc].xtile;
	scrollpos.curytileoffs = scrollbookmarks[loc].ytile;

	if (scrollpos.curxtileoffs >= scrollpos.tilewidth)
	{
		scrollpos.curxtileoffs = scrollpos.tilewidth-1;
	}
	if (scrollpos.curytileoffs >= scrollpos.tilewidth)
	{
		scrollpos.curytileoffs = scrollpos.tilewidth-1;
	}
	if (scrollpos.curx > scrollpos.maxx)
	{
		scrollpos.curx = scrollpos.maxx;
		scrollpos.curxtileoffs = scrollpos.maxxtileoffs;
	}
	else if (scrollpos.curx == scrollpos.maxx && scrollpos.curxtileoffs
			> scrollpos.maxxtileoffs)
	{
		scrollpos.curxtileoffs = scrollpos.maxxtileoffs;
	}
	if (scrollpos.cury > scrollpos.maxy)
	{
		scrollpos.cury = scrollpos.maxy;
		scrollpos.curytileoffs = scrollpos.maxytileoffs;
	}
	else if (scrollpos.cury == scrollpos.maxy && scrollpos.curytileoffs
			> scrollpos.maxytileoffs)
	{
		scrollpos.curytileoffs = scrollpos.maxytileoffs;
	}

	setValidScroll();
}

Uint32 CnCMap::translateToPos(Uint16 x, Uint16 y) const
{
	return y*width+x;
}

void CnCMap::translateFromPos(Uint32 pos, Uint16 *x, Uint16 *y) const
{
	*y = pos/width;
	*x = pos-((*y)*width);
}

bool CnCMap::isLoading() const
{
	return loading;
}

/**
 * True if this map is in snow theme
 */
bool CnCMap::SnowTheme(void)
{
	if (strcmp(missionData->theater, "SNOW") == 0)
	{
		return true;
	}
	return false;
}

Uint8 CnCMap::getTerrainType(Uint32 pos) const
{
	return terraintypes[pos];
}

void CnCMap::setScrollPos(Uint32 x, Uint32 y)
{
	scrollpos.curx = x;
	scrollpos.cury = y;
}

void CnCMap::prepMiniClip(Uint16 sidew, Uint16 sideh)
{
	miniclip.sidew = sidew;
	miniclip.sideh = sideh;
}

bool CnCMap::toScroll()
{
	return toscroll;
}

Uint32 CnCMap::setSmudge(Uint32 pos, Uint8 value)
{
	// clear the existing smudge bits first
	overlaymatrix[pos] &= ~(0xF0);
	return (overlaymatrix[pos] |= (value<<4));
}

Uint32 CnCMap::setTiberium(Uint32 pos, Uint8 value)
{
	// clear the existing tiberium bits first
	overlaymatrix[pos] &= ~0xF;
	return (overlaymatrix[pos] |= value);
}

/**
 * @param TriggerName Name of the trigger
 * @param Trig Pointer to the data of the trigger
 */
void CnCMap::setTriggerByName(string TriggerName, RA_Tiggers *Trig)
{
	for (unsigned int i = 0; i < RaTriggers.size(); i++)
	{
		if (TriggerName.size() != RaTriggers[i].name.size())
			continue;
		unsigned int j;
		for (j = 0; j < TriggerName.size(); j++)
		{
			if (toupper(TriggerName[j]) != toupper(RaTriggers[i].name[j]))
				break;
		}
		//printf ("String1 = %s, String2 = %s, j = %i, size1 = %i, size2 = %i\n", TriggerName.c_str(), RaTriggers[i].name.c_str(), j, TriggerName.size(), RaTriggers[i].name.size());
		if (j == TriggerName.size())
		{
			memcpy(&RaTriggers[i], Trig, sizeof(RA_Tiggers));
		}
	}
}

/**
 * Set a waypoint to a position in the map
 *
 * @param pointnr Number of the waypoint
 * @param mappos position of the waypoint in the map
 */
void CnCMap::setWaypoint(Uint8 pointnr, Uint32 mappos)
{
	if (pointnr > 99){
		return;
	}
	waypoints[pointnr] = mappos;
}

/**
 * Transform coordinates from number TO x,y pos
 */
void CnCMap::translateCoord(Uint32 linenum, Uint16* tx, Uint16* ty) const
{
	if (translate_64)
	{
		*tx = linenum%64;
		*ty = linenum/64;
	}
	else
	{
		*tx = linenum%128;
		*ty = linenum/128;
	}
}

bool CnCMap::validCoord(Uint16 tx, Uint16 ty) const
{
	return (!(tx < x || ty < y ||
			tx>	x+width || ty> height+y));
}

Uint32 CnCMap::getWaypoint(Uint8 pointnr)
{
	if (pointnr > 99)
	{
		logger->error("%s line %i ERROR: invalid pointnr\n",__FILE__ , __LINE__);
		return 0;
	}
	return waypoints[pointnr];
}

Uint32 CnCMap::normaliseCoord(Uint32 linenum) const
{
	Uint16 tx, ty;
	translateCoord(linenum, &tx, &ty);
	return normaliseCoord(tx, ty);
}

Uint32 CnCMap::normaliseCoord(Uint16 tx, Uint16 ty) const
{
	return (ty-y)*width + tx - x;
}

void CnCMap::decreaseResource(Uint32 pos, Uint8 amount)
{
	// Recourse type 7 = tiberium, all resources lower are more expansife
	//        Uint8 type = (resourcematrix[pos] & 0xFF);
	//	printf ("Recource type is: %i\n", type);
	Uint8 TempAmount = resourcematrix[pos] >> 8;
	//	printf ("Recource amount is: %i\n", TempAmount);
	if ((TempAmount - amount) > 0)
	{
		TempAmount -= amount;
		resourcematrix[pos] = resourcematrix[pos] & 0xFF;
		resourcematrix[pos] |= (TempAmount << 8);
	}
	else
	{
		resourcematrix[pos] = 0;
	}
}

/**
 *
 */
RA_Tiggers* CnCMap::getTriggerByNumb(int TriggerNumb)
{
	int size = RaTriggers.size();

	// If number = -1
	if (TriggerNumb == -1)
	{
		// Return a blank trigger
		RA_Tiggers* toto = new RA_Tiggers();
		toto->name = "None";
		return toto;
	}

	if (TriggerNumb >= size ||	TriggerNumb < -1) {
		return 0;
	}

	// Return the right trigger
	return &RaTriggers[TriggerNumb];
}

/**
 * Get a trigger by this name
 */
RA_Tiggers* CnCMap::getTriggerByName(string TriggerName)
{
	vector<RA_Tiggers>::iterator i = RaTriggers.begin();
	int index = 0;

	string name = TriggerName;

	// Upper the string
    transform(name.begin(), name.end(), name.begin(), toupper);


	while (i != RaTriggers.end())
	{
		string UpTrig = i->name;
		// Upper the string
		transform(UpTrig.begin(), UpTrig.end(), UpTrig.begin(), toupper);

		if (UpTrig == name)
		{
			printf("%s = %s \n", UpTrig.c_str(), name.c_str());
			return &RaTriggers[index];
		}
		i++;
		index++;
	}

	return 0;
	/*
	unsigned int j;
	for (unsigned int i = 0; i < RaTriggers.size(); ++i)
	{
		printf ("%s line %i: String1 = %s, String2 = %s, j = %i, size1 = %i, size2 = %i\n",  __FILE__, __LINE__, TriggerName.c_str(), RaTriggers[i].name.c_str(), j, TriggerName.size(), RaTriggers[i].name.size());

		if (TriggerName.size() != RaTriggers[i].name.size())
			continue;

		for (j = 0; j < TriggerName.size(); ++j)
		{
			printf ("%s line %i: TriggerName.size() = %i, RaTriggers[i].name.size() = %i, j = %i\n", __FILE__, __LINE__, TriggerName.size(), RaTriggers[i].name.size(), j);
			if (toupper(TriggerName[j]) != toupper(RaTriggers[i].name[j]))
				break;
		}
		printf ("%s line %i: String1 = %s, String2 = %s, j = %i, size1 = %i, size2 = %i\n",  __FILE__, __LINE__, TriggerName.c_str(), RaTriggers[i].name.c_str(), j, TriggerName.size(), RaTriggers[i].name.size());
		if (j == TriggerName.size())
		{
			//trigger = &RaTriggers[i];
			return &RaTriggers[i];
		}
	}
	return 0;*/
}

RA_Teamtype *CnCMap::getTeamtypeByNumb(unsigned int TeamNumb)
{
	if (TeamNumb >= RaTeamtypes.size())
		return NULL;
	return &RaTeamtypes[TeamNumb];
}

/**
 * @param teamNameString Name of the team
 * @return Data type of the team
 */
RA_Teamtype* CnCMap::getTeamtypeByName(string teamNameString)
{
	for (unsigned int i = 0; i < RaTeamtypes.size(); i++)
	{
		if (teamNameString.size() != RaTeamtypes[i].tname.size())
			continue;
		unsigned int j;
		for (j = 0; j < teamNameString.size(); j++)
		{
			if (toupper(teamNameString[j]) != toupper(RaTeamtypes[i].tname[j]))
				break;
		}
		if (j == teamNameString.size())
		{
			return &RaTeamtypes[i];
		}
	}

	// Return NULL
	return 0;
}

SDL_Surface *CnCMap::getShadowTile(Uint8 shadownum)
{
	// ra has 48 shadow images...
	if (shadownum >= numShadowImg)
	{
		return NULL;
	}

	return shadowimages[shadownum];
}

SDL_Surface* CnCMap::getMapTile(Uint32 pos)
{
	return tileimages[tilematrix[pos]];
}

/**
 * Get the terrain overlay
 */
Uint32 CnCMap::getTerrainOverlay(Uint32 pos)
{
	// Check if the pos(ition) is in the map
	if (pos < terrainoverlay.size())
	{
		// return the overlay
		return terrainoverlay[pos];
	}
	// Return the index zero by default
	return 0;
}

/**
 * Set the terrain overlay to an Image/Frame
 */
void CnCMap::setTerrainOverlay(Uint32 pos, Uint32 ImgNum, Uint16 Frame)
{
	// Check if the pos(ition) is in the map
	if (pos < terrainoverlay.size())
	{
		// Set the overlay
		terrainoverlay[pos] = ImgNum | (Frame &0x7FF);
	}
}

/**
 * Return width of the map
 */
Uint16 CnCMap::getWidth() const
{
	return width;
}

Uint16 CnCMap::getHeight() const
{
	return height;
}

Uint32 CnCMap::getSize() const
{
	return width*height;
}

MissionData* CnCMap::getMissionData()
{
	return missionData;
}

Uint8 CnCMap::getGameMode() const
{
	return gamemode;
}

Uint16 CnCMap::getYScroll() const
{
	return scrollpos.cury;
}

Uint16 CnCMap::getXScroll() const
{
	return scrollpos.curx;
}

Uint32 CnCMap::getScrollPos() const
{
	return scrollpos.cury*width+scrollpos.curx;
}

Uint32 CnCMap::getSmudge(Uint32 pos) const
{
	return ((overlaymatrix[pos] & 0xF0) << 12);
}

Uint32 CnCMap::getTiberium(Uint32 pos) const
{
	return (overlaymatrix[pos] & 0xF);
}

Uint32 CnCMap::getResourceFrame(Uint32 pos) const
{
	if (string(missionData->theater).substr(0, 3) == "INT")
	{
		// no image
		return 0;
	}

	return ((resourcebases[resourcematrix[pos] & 0xFF]<<16) + (resourcematrix[pos]>>8));
}

bool CnCMap::getResource(Uint32 pos, Uint8* type, Uint8* amount) const
{
	if (0 == type || 0 == amount)
	{
		return (0 != resourcematrix[pos]);
	}
	*type = resourcematrix[pos] & 0xFF;
	*amount = resourcematrix[pos] >> 8;
	return (0 != resourcematrix[pos]);
}

Uint16 CnCMap::getYTileScroll() const
{
	return (Uint16)scrollpos.curytileoffs;
}

Uint16 CnCMap::getXTileScroll() const
{
	return (Uint16)scrollpos.curxtileoffs;
}

const MiniMapClipping& CnCMap::getMiniMapClipping() const
{
	return miniclip;
}

SHPImage* CnCMap::getPips()
{
	return pips;
}

Uint32 CnCMap::getPipsNum() const
{
	return pipsnum;
}

SHPImage* CnCMap::getMoveFlash()
{
	return moveflash;
}

Uint32 CnCMap::getMoveFlashNum() const
{
	return flashnum;
}

Uint16 CnCMap::getX() const
{
	return x;
}

Uint16 CnCMap::getY() const
{
	return y;
}

/**
 * Loads the maps ini file containing info on dimensions, units, trees
 * and so on.
 */
void CnCMap::loadIni()
{
	ostringstream TempString;
	INIFile *inifile;
	string tmpName;

	// Get the ini file of the map
	tmpName = missionData->mapname;
	tmpName += ".ini";

	// Load the INIFile
	try
	{
		inifile = GetConfig(tmpName);
	}
	catch (runtime_error&)
	{
		// Log the error
		logger->error("Map \"%s\" not found.  Check your installation.\n", tmpName.c_str());
		// Throw an error
		throw LoadMapError("Error in loading the ini file [" + tmpName + "]");
	}

	// Build the player list
	p::ppool = new PlayerPool(inifile, gamemode);

	// WARN if the ini format is not supported
	if (inifile->readInt("Basic", "NewINIFormat", 0) != 3)
	{
		// Log the error
		logger->error("Only Red Alert maps are fully supported\nThe format of this Map is not supported\n");
		// Trow an error
		throw LoadMapError("Error in loading the ini file [" + tmpName + "], the version of the ini (NewINIFormat) is not equal to 3");
	}

	// Read simple ini section
	simpleSections(inifile);

	// Try to create UnitAndStructurePool
	try
	{
		// Create the UnitAndStructurePool
		p::uspool = new UnitAndStructurePool(this->missionData->theater);
	}
	catch (...)
	{
		// Log it
		logger->error("Error in creating UnitAndStructurePool (p::uspool)\n");
		// Throw error
		throw LoadMapError("Error in creating UnitAndStructurePool (p::uspool)\n");
	}


	if (gamemode == GAME_MODE_SINGLE_PLAYER)
	{
		// Set the local player with House name in the BASIC section
		// of the .ini of the map
		p::ppool->setLPlayer(missionData->player);
		// @todo modify that to set the colour
		//p::ppool->getLPlayer()->setMultiColour(pc::Config.side_colour.c_str());
		//			printf ("%s line %i: Set local player\n", __FILE__, __LINE__);
	}

	terraintypes.resize(width*height, 0);
	resourcematrix.resize(width*height, 0);

	// Load advanced section of the ini
	advancedSections(inifile);

	// Unpack the section "MapPack"
	unMapPack(inifile);

	// spawn player starts for non single player games.
	if (gamemode != GAME_MODE_SINGLE_PLAYER)
	{
		int LplayerColorNr = p::ppool->MultiColourStringToNumb(pc::Config.side_colour.c_str());
		int offset = 0;
		for (Uint8 i=0;i<pc::Config.totalplayers;++i)
		{

			//			sprintf (TempString, "%i", i+1);
			TempString.str("");
			TempString << (unsigned int)(i+1);
			printf ("Spawning player %s\n", TempString.str().c_str());
			tmpName = "multi";
			//tmpname += TempString;
			tmpName += TempString.str();
			p::ppool->getPlayerNum(tmpName.c_str());
			if ((i+1) != pc::Config.playernum)
			{
				string Nick;
				TempString.str("");
				TempString<<(unsigned int)(i+1);
				Nick = "Comp";
				//Nick += TempString;
				Nick += TempString.str();
				if (i == LplayerColorNr)
				{
					offset = 1;
				}

				p::ppool->setPlayer(i+1, Nick.c_str(), i+offset, "nod" /* pc::Config.mside.c_str() */);
			}
			else
			{
				//
				// This is the local player ;)
				//
				p::ppool->setLPlayer(i+1, pc::Config.nick.c_str(),pc::Config.side_colour.c_str(), pc::Config.mside.c_str());
			}
			p::ppool->getPlayer(i)->setMoney(pc::Config.startMoney);
		}
		printf ("player side is %i\n", p::ppool->getPlayer(p::ppool->getLPlayerNum())->getSide());
		p::ppool->placeMultiUnits();
	}

	// Load the images of pips and save the number
	pipsnum = pc::imgcache->loadImage("pips.shp");


	// Load the animation of mouvement graphics
	// (the circle when clicking for movement)
	char moveflsh[13] = "moveflsh.";
	strncat(moveflsh, missionData->theater, 3);
	flashnum = 0;
	// Load the images and save the number
	flashnum = pc::imgcache->loadImage(moveflsh);

	printf("moveflsh = %s   nmu = %d\n", moveflsh, flashnum);
	printf("pips =    nmu = %d\n", pipsnum);
}

/**
 * Function to load all vars in the simple sections of the inifile
 *
 * @todo add key name in the log (to throw LoadMapError
 *
 * @param pointer to the inifile
 */
void CnCMap::simpleSections(INIFile *inifile)
{
	try
	{
		// Try to read Basic/BRIEF
		missionData->brief = inifile->readString("Basic", "Brief");

		// Try to read Basic/ACTION
		missionData->action = inifile->readString("Basic", "Action");

		// Try to read Basic/PLAYER
		missionData->player = inifile->readString("Basic", "Player");

		// Try to read Basic/THEME
		missionData->theme = inifile->readString("Basic", "Theme");

		// Try to read Basic/WIN
		missionData->winmov = inifile->readString("Basic", "Win");

		// Try to read Basic/LOSE
		missionData->losemov = inifile->readString("Basic", "Lose");

		// Try to read MAP/HEIGHT
		height = inifile->readInt("Map", "Height");

		// Try to read Width
		width = inifile->readInt("Map", "Width");

		// Try to read x coordinates in the map
		x = inifile->readInt("Map", "X");

		// Try to read y coordinates in the map
		y = inifile->readInt("Map", "Y");

		// Try to read the theme
		missionData->theater = inifile->readString("Map", "Theater");

		// Save if it's the last mission
		if (inifile->readYesNo("Basic", "EndOfGame", "no") == 1) {
			missionData->endOfGame = true;
		} else {
			missionData->endOfGame = false;
		}
	}
	catch (KeyNotFound& ex)
	{
		logger->error("Error loading map: %s\n", ex.what());
		throw LoadMapError("Error loading map: " + string (ex.what()));
	}
	catch(...)
	{
		logger->error("Error loading map\n");
		throw LoadMapError("Error loading map");
	}
}

/**
 */
Uint8 CnCMap::UnitActionToNr(const string action)
{
	if (action == "Sleep")
	{
		return COMMAND_SLEEP;
	}
	if (action == "Attack")
	{
		return COMMAND_ATTACK;
	}
	if (action == "Move")
	{
		return COMMAND_MOVE;
	}
	if (action == "Qmove")
	{
		return COMMAND_QMOVE;
	}
	if (action == "Retreat")
	{
		return COMMAND_REPEAT;
	}
	if (action == "Guard")
	{
		return COMMAND_GUARD;
	}
	if (action == "Sticky")
	{
		return COMMAND_STICKY;
	}
	if (action == "Enter")
	{
		return COMMAND_ENTER;
	}
	if (action == "Capture")
	{
		return COMMAND_CAPTURE;
	}
	else if (action == "Harvest")
	{
		return COMMAND_HARVEST;
	}
	else if (action == "Area Guard")
	{
		return COMMAND_AREA_GUARD;
	}
	else if (action == "Return")
	{ // (unused)
		return COMMAND_RETURN;
	}
	else if (action == "Stop")
	{
		return COMMAND_STOP;
	}
	else if (action == "Ambush")
	{ // (unused)
		return COMMAND_AMBUSH;
	}
	else if (action == "Hunt")
	{
		return COMMAND_HUNT;
	}
	else if (action == "Unload")
	{
		return COMMAND_UNLOAD;
	}
	else if (action == "Sabotage")
	{
		return COMMAND_SABOTAGE;
	}
	else if (action == "Construction")
	{
		return COMMAND_CONSTRUCT;
	}
	else if (action == "Selling")
	{
		return COMMAND_SELLING;
	}
	else if (action == "Repair")
	{
		return COMMAND_REPAIR;
	}
	else if (action == "Rescue")
	{
		return COMMAND_RESCUE;
	}
	else if (action == "Missile")
	{
		return COMMAND_MISSILE;
	}
	else if (action == "Harmless")
	{
		return COMMAND_HARMLESS;
	}

	return COMMAND_SLEEP;
}

/**
 * Function to load all the advanced sections in the inifile.
 *
 * @param a pointer to the inifile
 */
void CnCMap::advancedSections(INIFile *inifile)
{
	Uint32 keynum;
	int i = 0;
	//char *line;
	char shpname[128];
	char trigger[128];
	char action[128];
	char type[128];
	char owner[128];
	int facing, health, subpos;
	int linenum, smudgenum, tmpval;
	Uint16 tx = 0;
	Uint16 ty = 0;
	Uint16 xsize = 0;
	Uint16 ysize = 0;
	Uint16 tmp2 = 0;
	map<string, Uint32> imagelist;
	map<string, Uint32>::iterator imgpos;
	SHPImage *image;
	TerrainEntry tmpterrain;
	INIKey key;

	Uint16 xwalk = 0;
	Uint16 ywalk = 0;
	Uint16 ttype = 0;

	try
	{
		for (keynum = 0;; keynum++)
		{
			key = inifile->readKeyValue("WAYPOINTS", keynum);
			if (sscanf(key->first.c_str(), "%d", &tmpval) == 1)
			{
				if (maptype == GAME_RA)
				{
					// waypoints 0-7 are the starting locations in multiplayer maps
					tmp2 = (Uint16)atoi(key->second.c_str());
					translateCoord(tmp2, &tx, &ty);
					tmp2 = translateToPos(tx-x, ty-y);
					setWaypoint (tmpval, tmp2);
					if (tmpval == 98)
					{ // waypoint 98 is the startpos of the map in red alert
						scrollbookmarks[0].x = tx-x;
						scrollbookmarks[0].y = ty-y;
					}

				}
				else if (maptype == GAME_TD)
				{
					if (tmpval == 26)
					{ // waypoint 26 is the startpos of the map
						tmp2 = (Uint16)atoi(key->second.c_str());
						//waypoints.push_back(tmp2);
						translateCoord(tmp2, &tx, &ty);
						scrollbookmarks[0].x = tx-x;
						scrollbookmarks[0].y = ty-y;
					}
					if (tmpval < 8)
					{
						tmp2 = (Uint16)atoi(key->second.c_str());
						setWaypoint (tmpval, tmp2);
					}
				}
			}
		}
	}
	catch(...)
	{}

	// Log it (end of waypoint decode
	logger->note("Waypoint loaded...\n");


	// set player start locations
	for (int k = 0; k < 8; k++)
	{
		p::ppool->setPlayerStarts(k, getWaypoint(k));
	}

	// load the shadowimages
	try
	{
		image = new SHPImage("shadow.shp", -1);
		numShadowImg = image->getNumImg();
		shadowimages.resize(numShadowImg);
		for( i = 0; i < 48; i++)
		{
			// decode the SHP and get a SDL_Surface
			image->getImageAsAlpha(i, &shadowimages[i]);
		}
		if (image != 0){
			delete image;
		}
		image = 0;
	}
	catch(ImageNotFound&)
	{
		logger->warning("Unable to load \"shadow.shp\"\n");
		numShadowImg = 0;
	}
	// Log it (end of Shadow decode
	logger->note("Shadow images loaded...\n");

	// load the smudge marks and the tiberium to the imagepool
	if (string(missionData->theater).substr(0,3) != "INT")
	{
		logger->note("smudge marks and the tiberium loading...\n");

		string sname;
		//if (maptype == GAME_TD)
		//{
		//	sname = "TI1";
		//}
		//else
		if (maptype == GAME_RA)
		{
			sname = "GOLD01";
		}
		else
		{
			logger->error("Unsuported maptype\n");
			throw LoadMapError("Unsuported maptype\n");
		}

		resourcenames[sname] = 0;
		sname += "." + string(missionData->theater, 3);
		try
		{
			image = new SHPImage(sname.c_str(), -1);
			resourcebases.push_back(pc::imagepool->size());
			pc::imagepool->push_back(image);
		}
		catch (ImageNotFound&)
		{
			logger->error("Could not load \"%s\"\n",sname.c_str());
			throw LoadMapError("Could not load " + sname);
		}
		// No craters or scorch marks for interior?
		for (i = 1; i <= 6; i++)
		{
			sprintf(shpname, "SC%d.", i);
			strncat(shpname, missionData->theater, 3);
			try
			{
				image = new SHPImage(shpname, -1);
			}
			catch (ImageNotFound&)
			{	continue;}
			pc::imagepool->push_back(image);
		}
		for (i = 1; i <= 6; i++)
		{
			sprintf(shpname, "CR%d.", i);
			strncat(shpname, missionData->theater, 3);
			try
			{
				image = new SHPImage(shpname, -1);
			}
			catch (ImageNotFound&)
			{	continue;}
			pc::imagepool->push_back(image);
		}

		// Log it (end of waypoint decode
		logger->note("smudge images loaded...\n");
	}

	// Resize overlaymatrix (use in TERRAIN loading)
	overlaymatrix.resize(width*height, 0);

	// Loading of art.ini (use in TERRAIN loading)
	INIFile* arts = 0;
	arts = GetConfig("art.ini");

	// Loading of terrain
	try
	{
		for( keynum = 0;;keynum++ )
		{
			bool bad = false;

			// Try to load the number ?keynum? section
			key = inifile->readKeyValue("TERRAIN", keynum);
			// , is the char which separate terraintype from action.

			if( sscanf(key->first.c_str(), "%d", &linenum) == 1 &&
					sscanf(key->second.c_str(), "%[^,],", shpname) == 1 )
			{
				// Set the next entry in the terrain vector to the correct values.
				// the map-array and shp files vill be set later
				translateCoord(linenum, &tx, &ty);

				if( tx < x || ty < y || tx> x+width || ty> height+y )
				{
					continue;
				}

				if( shpname[0] == 't' || shpname[0] == 'T' )
				ttype = t_tree;
				else if( shpname[0] == 'r' || shpname[0] == 'R' )
				ttype = t_rock;
				else
				ttype = t_other_nonpass;

				// calculate the new pos based on size and blocked
				xsize = arts->readInt(shpname, "XSIZE",1);
				ysize = arts->readInt(shpname, "YSIZE",1);

				for( ywalk = 0; ywalk < ysize && ywalk + ty < height+y; ywalk++ )
				{
					for( xwalk = 0; xwalk < xsize && xwalk + tx < width + x; xwalk++ )
					{
						sprintf(type, "NOTBLOCKED%d", ywalk*xsize+xwalk);
						try
						{
							arts->readInt(shpname, type);
						}
						catch (INI::KeyNotFound&)
						{
							terraintypes[(ywalk+ty-y)*width+xwalk+tx-x] = ttype;
						}
					}
				}

				linenum = xsize*ysize;
				int done = 0;
				do
				{
					if (linenum == 0)
					{
						logger->error("BUG: Could not find an entry in art.ini for %s\n",shpname);
						bad = true;
						break;
					}
					linenum--;
					sprintf(type, "NOTBLOCKED%d", linenum);

					// Try to read
					try
					{
						arts->readInt(shpname, type);
					}
					catch (...)
					{
						done = 1;
					}
				}while(done == 0);

				if (bad)
				{
					continue;
				}

				tmpterrain.xoffset = -(linenum%ysize)*24;
				tmpterrain.yoffset = -(linenum/ysize)*24;

				tx += linenum%ysize;
				if( tx >= width+x )
				{
					tmpterrain.xoffset += 1+tx-(width+x);
					tx = width+x-1;
				}
				ty += linenum/ysize;
				if( ty >= height+y )
				{
					tmpterrain.yoffset += 1+ty-(height+y);
					ty = height+y-1;
				}

				linenum = normaliseCoord(tx, ty);
				strcat(shpname, ".");
				strncat(shpname, missionData->theater, 3);

				// search the map for the image
				imgpos = imagelist.find(shpname);

				// set up the overlay matrix and load some shps
				if( imgpos != imagelist.end() )
				{
					// this tile already has a number
					overlaymatrix[linenum] |= HAS_TERRAIN;
					tmpterrain.shpnum = imgpos->second << 16;
					terrains[linenum] = tmpterrain;
				}
				else
				{
					// a new tile
					imagelist[shpname] = pc::imagepool->size();
					overlaymatrix[linenum] |= HAS_TERRAIN;
					tmpterrain.shpnum = pc::imagepool->size()<<16;
					terrains[linenum] = tmpterrain;
					try
					{
						image = new SHPImage(shpname, -1);
					}
					catch (ImageNotFound&)
					{
						logger->error("Could not load \"%s\"\n", shpname);
						throw LoadMapError("Could not load " + string(shpname));
					}
					pc::imagepool->push_back(image);
				}
			}
		}
	}
	catch(...)
	{}
	// Log it (end of TERRAIN decode
	logger->note("Terrain loaded...\n");


	// decode OverlayPack section
	if (maptype == GAME_RA)
	{
		unOverlayPack(inifile);
	}
	else
	{
		loadOverlay(inifile);
	}
	// Log it (end of waypoint decode
	logger->note("OverlayPack loaded...\n");


	// Try to set SMUDGE
	try
	{
		for( keynum = 0;;keynum++ )
		{
			key = inifile->readKeyValue("SMUDGE", keynum);
			/* , is the char which separate terraintype from action. */
			if( sscanf(key->first.c_str(), "%d", &linenum) == 1 &&
					sscanf(key->second.c_str(), "SC%d", &smudgenum) == 1 )
			{
				translateCoord(linenum, &tx, &ty);
				if( tx < x || ty < y || tx> x+width || ty> height+y )
				{
					continue;
				}
				linenum = (ty-y)*width + tx - x;
				overlaymatrix[linenum] |= (smudgenum<<4);
			}
			else if( sscanf(key->first.c_str(), "%d", &linenum) == 1 &&
					sscanf(key->second.c_str(), "CR%d", &smudgenum) == 1 )
			{
				//} else if( sscanf(line, "%d=CR%d", &linenum, &smudgenum) == 2 ) {
				translateCoord(linenum, &tx, &ty);
				if( tx < x || ty < y || tx> x+width || ty> height+y )
				{
					continue;
				}

				linenum = (ty-y)*width + tx - x;
				overlaymatrix[linenum] |= ((smudgenum+6)<<4);
			}
		}
	}
	catch(...)
	{}

	// Try to read techs levels < 30
	try {
		p::uspool->preloadUnitAndStructures(30);
	} catch (...) {
		logger->debug("Error during preloadUnitAndStructures() in [advancedSections()]");
	}
	// Log it
	logger->note("UnitAndStructurePool::preloadUnitAndStructures() ok\n");

	// Try to read techs levels
	try {
		p::uspool->generateProductionGroups();
	} catch (...) {
		logger->debug("Error during generateProductionGroups() in [advancedSections()]");
	}
	// Log it
	logger->note("UnitAndStructurePool::generateProductionGroups() ok\n");

	//
	// STRUCTURES
	//
	// If their are a section called "STRUCTURES"
	if (inifile->isSection(string("STRUCTURES")) == true)
	{
		try
		{
			for( keynum = 0;;keynum++ )
			{
				if (maptype == GAME_RA)
				{
					key = inifile->readKeyValue("STRUCTURES", keynum);
					// ',' is the char which separate terraintype from action.
					if( sscanf(key->first.c_str(), "%d", &tmpval) == 1 &&
							sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%[^,]", owner, type,
									&health, &linenum, &facing, trigger ) == 6 )
					{
						translateCoord(linenum, &tx, &ty);
						facing = min(31,facing>>3);
						if( tx < x || ty < y || tx> x+width || ty> height+y )
						{
							continue;
						}
						linenum = (ty-y)*width + tx - x;

						//printf("CnCMap::loadIni(%s)\n", owner);

						p::uspool->createStructure(type, linenum, p::ppool->getPlayerNum(owner), health, facing, false, trigger);
						//                    printf ("%s line %i: createStructure STRUCTURE %s, trigger = %s\n", __FILE__, __LINE__, type, trigger);
					}
				}
				else if (maptype == GAME_TD)
				{
					key = inifile->readKeyValue("STRUCTURES", keynum);
					// ',' is the char which separate terraintype from action.
					memset (trigger, 0, sizeof (trigger));
					if( sscanf(key->first.c_str(), "%d", &tmpval) == 1 &&
							sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%s", owner, type,
									&health, &linenum, &facing, trigger ) == 6 )
					{
						translateCoord(linenum, &tx, &ty);
						facing = min(31,facing>>3);
						if( tx < x || ty < y || tx> x+width || ty> height+y )
						{
							continue;
						}
						linenum = (ty-y)*width + tx - x;

						//printf("CnCMap::advancedSections(%s)\n", owner);
						p::uspool->createStructure(type, linenum, p::ppool->getPlayerNum(owner),
								health, facing, false, trigger);
					}
				}
			}
		}
		catch(...)
		{
			// Log it
			logger->error("error in CncMap::advanced...STRUCTURES ok\n");
		}
		// Log it
		logger->note("CncMap::advanced...STRUCTURES ok\n");
	}


	//
	// Decode Units section
	//
	// If their are a section called "UNITS"
	if (inifile->isSection(string("UNITS")) == true)
	{
		try
		{
			for( keynum = 0;;keynum++ )
			{
				// Read the key number "keynum"
				key = inifile->readKeyValue("UNITS", keynum);

				// ',' is the char which separate terraintype from action.
				if( sscanf(key->first.c_str(), "%d", &tmpval) == 1 &&
						sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%[^,],%s", owner, type,
								&health, &linenum, &facing, action, trigger ) == 7 )
				{
					// convert hash cursor in x and y
					translateCoord(linenum, &tx, &ty);
					facing = min(31,facing>>3);
					if( tx < x || ty < y || tx> x+width || ty> height+y )
					{
						continue;
					}
					linenum = (ty-y)*width + tx - x;
					// Create the unit
					p::uspool->createUnit(type, linenum, 5, p::ppool->getPlayerNum(owner), health, facing, UnitActionToNr(action), trigger);

					//printf ("%s line %i: createUnit UNIT %s, trigger = %s\n", __FILE__, __LINE__, key->first.c_str(), trigger);
				} else {
					logger->error("ERROR DURING DECODE Line read in UNIT = %s\n", key->second.c_str());
				}
			}
		}
		catch(...)
		{}
	}
	// Log it
	logger->note("CncMap::advanced...UNIT ok\n");

	//infantry
	try
	{
		for( keynum = 0;;keynum++ )
		{
			key = inifile->readKeyValue("INFANTRY", keynum);
			/* , is the char which separate terraintype from action. */
			if( sscanf(key->first.c_str(), "%d", &tmpval ) == 1 &&
					sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%[^,],%d,%s", owner, type,
							&health, &linenum, &subpos, action, &facing, trigger ) == 8 )
			{
				translateCoord(linenum, &tx, &ty);
				facing = min(31,facing>>3);
				if( tx < x || ty < y || tx> x+width || ty> height+y )
				{
					continue;
				}
				linenum = (ty-y)*width + tx - x;

				p::uspool->createUnit(type, linenum, subpos, p::ppool->getPlayerNum(owner), health, facing, UnitActionToNr(action), trigger);

				//printf ("%s line %i: createUnit INFANTRY, unit = %c%c%c, trigger = %s\n", __FILE__, __LINE__, type[0], type[1], type[2], trigger);
			}
		}
	}
	catch(...)
	{}

	// Decode and create CellTriggers
	if (maptype == GAME_RA)
	{
		try
		{
			for( keynum = 0;;keynum++ )
			{

				// Read the "CellTriggers" section
				key = inifile->readKeyValue("CellTriggers", keynum);

				// First read the coordinates and triggername of the celltrigger
				if( sscanf(key->first.c_str(), "%d", &linenum) == 1 &&
						sscanf(key->second.c_str(), "%[^,],", trigger) == 1 )
				{
					CellTrigger* cellTrigger;

					cellTrigger = new CellTrigger();
					cellTrigger->name = trigger;
					cellTrigger->cellpos = linenum;

					translateCoord(linenum, &cellTrigger->x, &cellTrigger->y);

					if( cellTrigger->x < x || cellTrigger->y < y || cellTrigger->x > x+width || cellTrigger->y > height+y )
					{
						continue;
					}

					// Add in the Trigger list of the map
					CellTriggers.push_back(*cellTrigger);

					// Remember there is a celltrigger in this cell
					bool res = p::uspool->createCellTrigger(cellTrigger->cellpos);

					// Test if trigger creation is good
					if (res==false){
						// @todo CHECK THAT BECAUSE ERROR IN CREATION
						//logger->error("Error in create TRIGGER in the map\n");
					}
				}
			}
		}
		catch (...)
		{}
	}

	// triggers
	RA_Tiggers triggers;
	INIFile* messageTable = new INIFile("tutorial.ini");
	try
	{
		for( keynum = 0;;keynum++ )
		{
			if (maptype == GAME_RA)
			{
				key = inifile->readKeyValue("TRIGS", keynum);
				// is the char which separate terraintype from action.
				triggers.name = key->first;
				transform(triggers.name.begin(),triggers.name.end(), triggers.name.begin(), toupper);
				// Split the line
				vector<char*> triggsData = splitList(cppstrdup(key->second.c_str()), ',');
				// check that the line had 18 param
				if (triggsData.size()!=18) {
					logger->warning("error in reading trigger [%s]\n", key->first.c_str());
				}
				else
				{
					sscanf(triggsData[0], "%d", &triggers.repeatable);
					sscanf(triggsData[1], "%d", &triggers.country);
					sscanf(triggsData[2], "%d", &triggers.activate);
					sscanf(triggsData[3], "%d", &triggers.actions);
					sscanf(triggsData[4], "%d", &triggers.trigger1.event);
					sscanf(triggsData[5], "%d", &triggers.trigger1.param1);
					sscanf(triggsData[6], "%d", &triggers.trigger1.param2);
					sscanf(triggsData[7], "%d", &triggers.trigger2.event);
					sscanf(triggsData[8], "%d", &triggers.trigger2.param1);
					sscanf(triggsData[9], "%d", &triggers.trigger2.param2);
					// Build Action 1
					int actionType = 0;
					sscanf(triggsData[10], "%d", &actionType); // get the type
					int param1 = 0;
					sscanf(triggsData[11], "%d", &param1);
					int param2 = 0;
					sscanf(triggsData[12], "%d", &param2);
					int param3 = 0;
					sscanf(triggsData[13], "%d", &param3);
					switch (actionType)
					{
					case TRIGGER_ACTION_NO_ACTION:
						triggers.action1 = new NoActionTriggerAction();
						break;
					case TRIGGER_ACTION_TEXT:
					{
						// Get string with the num in data
						string messageToDraw = string(messageTable->readString("Tutorial", triggsData[13]));
						printf("Txt = %s\n", messageToDraw.c_str());
						// Build the TriggerAction
						triggers.action1 = new TextTriggerAction(messageToDraw, pc::msg);
					}
						break;
					case TRIGGER_ACTION_GLOBAL_SET:
						// Create an action (param 3 is the number of the global)
						triggers.action1 = new GlobalSetTriggerAction(param3);
						break;
					case TRIGGER_ACTION_GLOBAL_CLEAR:
						// Create an action (param 3 is the number of the global)
						triggers.action1 = new GlobalClearTriggerAction(param3);
						break;
					default:
						triggers.action1 = new RawTriggerAction(actionType, param1, param2, param3);
						break;
					}

					// Build Action 2
					int action2Type = 0;
					sscanf(triggsData[14], "%d", &action2Type); // get the type
					int param1b = 0;
					sscanf(triggsData[15], "%d", &param1b);
					int param2b = 0;
					sscanf(triggsData[16], "%d", &param2b);
					int param3b = 0;
					sscanf(triggsData[17], "%d", &param3b);
					switch (action2Type)
					{
					case TRIGGER_ACTION_NO_ACTION:
						triggers.action2 = new NoActionTriggerAction();
						break;
					case TRIGGER_ACTION_TEXT:
					{
						// Get string with the num in data
						string messageToDraw = string(messageTable->readString("Tutorial", triggsData[17]));
						printf("Txt = %s\n", messageToDraw.c_str());
						// Build the TriggerAction
						triggers.action2 = new TextTriggerAction(messageToDraw, pc::msg);
						break;
					}
					case TRIGGER_ACTION_GLOBAL_SET:
						// Create an action (param 3 is the number of the global)
						triggers.action2 = new GlobalSetTriggerAction(param3b);
						break;
					case TRIGGER_ACTION_GLOBAL_CLEAR:
						// Create an action (param 3 is the number of the global)
						triggers.action2 = new GlobalClearTriggerAction(param3);
						break;
					default:
						triggers.action2 = new RawTriggerAction(action2Type, param1b, param2b, param3b);
						break;
					}

					printf ("%s line %i: Read trigger:\n", __FILE__, __LINE__);
					PrintTrigger(triggers);
					printf ("\n\n\n");

					// Set to zero (=never executed)
					triggers.hasexecuted = false;
					RaTriggers.push_back(triggers);
				}
			}
			else if (maptype == GAME_TD)
			{
				key = inifile->readKeyValue("TRIGGERS", keynum);
				/* is the char which separate terraintype from action. */
				//logger->warning ("%s line %i: Trigger1 text: %s\n", __FILE__, __LINE__, key->first.c_str());
				//              logger->warning ("%s line %i: Trigger2 text: %s\n", __FILE__, __LINE__, key->second.c_str());
			}
		}
	}
	catch(...)
	{}


	// Loads TeamTypes
	loadTeamTypes(inifile);


	// Load  [BASE] section
	/*try
	{
		for( keynum = 0;;keynum++ )
		{
			if (maptype == GAME_RA)
			{
				key = inifile->readKeyValue("Base", keynum);
				// is the char which separate terraintype from action.
				//                printf ("%s line %i: Base text 1: %s\n", __FILE__, __LINE__, key->first.c_str());
				//                printf ("%s line %i: Base text 2: %s\n", __FILE__, __LINE__, key->second.c_str());
				//printf ("%s line %i: Base text: %s\n", __FILE__, __LINE__, key->third.c_str());
				//                if (sscanf(key->second.c_str(), "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i", &triggers.repeatable, &triggers.country, &triggers.activate, &triggers.actions, &triggers.trigger1.event,  &triggers.trigger1.param1, &triggers.trigger1.param2, &triggers.trigger2.event,  &triggers.trigger2.param1, &triggers.trigger2.param2,      &triggers.action1.Action, &triggers.action1.param1, &triggers.action1.param2, &triggers.action1.param3,      &triggers.action2.Action, &triggers.action2.param1, &triggers.action2.param2, &triggers.action2.param3) == 18  ) {
			}
			else if (maptype == GAME_TD)
			{
				//key = inifile->readUnsortedKeyValue("TRIGERS", keynum);
				// is the char which separate terraintype from action.
				//logger->warning ("%s line %i: Trigger1 text: %s\n", __FILE__, __LINE__, key->first.c_str());
				//              logger->warning ("%s line %i: Trigger2 text: %s\n", __FILE__, __LINE__, key->second.c_str());
			}
		}
	}
	catch(...)
	{}*/

	//
	// Digest
	//
	// If their are a section called "Digest"
	/*if (inifile->isSection("DIGEST") == true)
	{
		try
		{
			// @todo implemente digest
			//char mapdata[1024];
			Uint8 mapdata[16384]; // 16k
			Uint8 temp[16384];
			// read packed data into array
			mapdata[0] = 0;
			try
			{
				INIKey key = inifile->readIndexedKeyValue("Digest", 0);
				strcat(((char*)mapdata), key->second.c_str());
			}
			catch(...)
			{}

			// Decode data read with Compression class
			Compression::dec_base64(mapdata, temp, strlen(((char*)mapdata)));

			logger->debug("temp read = %s\n", temp);

				// decode the format80 coded data (2 chunks)
				curpos = 0;
				for (int tmpval = 0; tmpval < 2; tmpval++)
				{
					if (Compression::decode80((Uint8 *)temp+4+curpos, mapdata+8192*tmpval)
							!= 8192)
					{
						logger->warning("A format80 chunk in the \"OverlayPack\" was of wrong size\n");
					}
					curpos = curpos + 4 + temp[curpos] + (temp[curpos+1]<<8)
							+ (temp[curpos+2]<<16);
				}


				for (ytile = y; ytile <= y+height; ++ytile)
				{
					for (xtile = x; xtile <= x+width; ++xtile)
					{
						curpos = xtile+ytile*128;
						tilepos = xtile-x+(ytile-y)*width;
						if (mapdata[curpos] == 0xff) // No overlay
							continue;
						if (mapdata[curpos] > 0x17) // Unknown overlay type
							continue;
						parseOverlay(tilepos, RAOverlayNames[mapdata[curpos]]);
					}
				}

				logger->debug("digest() ok\n");
		}
		catch (...)
		{}
	}*/
}

struct tiledata
{
	Uint32 image;
	Uint8 type;
};

/**
 * Bin loading routines
 */
/*
void CnCMap::loadBin()
{
	Uint32 index = 0;
	//    Uint8 templ, tile;
	int xtile, ytile;
	VFile * binfile;
	char * binname;

	TileList *mapdata;
	mapdata = new TileList[width*height];

	// Calculate name of bin file ( mapname.bin ).
	binname = new char[strlen(missionData->mapname) + 4 +1]; // +4 for -> ".BIN"
	strcpy(binname, missionData->mapname);
	strcat(binname, ".BIN");

	// get the offset and size of the binfile along with a pointer to it
	//binfile = mixes->getOffsetAndSize(binname, &offset, &size);
	binfile = VFSUtils::VFS_Open(binname);
	if (binname != NULL)
	{
		delete[] binname;
	}
	binname = NULL;

	if (binfile == NULL)
	{
		logger->error("Unable to locate BIN file!\n");
		throw LoadMapError("Unable to locate BIN file!\n");
	}

	// Seek the beginning of the map.
	// It's at begining of bin + maxwidth * empty y cells + empty x cells
	// times 2 sinse each entry is 2 bytes
	binfile->seekSet( (64*y + x) * 2);

	for (ytile = 0; ytile < height; ytile++)
	{
		for (xtile = 0; xtile < width; xtile++)
		{
			Uint16 tmpread = 0;
			// Read template and tile
			mapdata[index].templateNum = 0;
			binfile->readByte((Uint8 *)&(tmpread), 1);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			mapdata[index].templateNum = SDL_Swap16(tmpread);
#else
			mapdata[index].templateNum = tmpread;
#endif
			binfile->readByte(&(mapdata[index].tileNum), 1);

			index++;
		}
		//Skip til the end of the line and the onwards to the
		//beginning of usefull data on the next line
		//
		binfile->seekCur( 2*(64-width));
	}
	VFSUtils::VFS_Close(binfile);
	parseBin(mapdata);
	if (mapdata != NULL)
		delete[] mapdata;
	mapdata = NULL;
}*/

/**
 * @param inifile Inifile to decode [MapPack] section
 */
void CnCMap::unMapPack(INIFile *inifile)
{
	int tmpval;
	Uint32 curpos;
	int xtile, ytile;
	TileList *bindata;
	Uint32 keynum;
	INIKey key;
	Uint8 *mapdata1 = new Uint8[49152]; // 48k
	Uint8 *mapdata2 = new Uint8[49152];

	// read packed data into array
	mapdata1[0] = 0;
	try
	{
		for (keynum = 1;;++keynum)
		{
			key = inifile->readIndexedKeyValue("MAPPACK", keynum);
			strcat(((char*)mapdata1), key->second.c_str());
		}
	}
	catch(...)
	{}

	unsigned int lengthToDecode = strlen(((char*)mapdata1));

	// Decompress base 64 data
	Compression::dec_base64(mapdata1, mapdata2, lengthToDecode);

	// decode the format80 coded data (6 chunks)
	curpos = 0;
	for (tmpval = 0; tmpval < 6; tmpval++)
	{
		//printf("first vals in data is %x %x %x %x\n", mapdata2[curpos],
		//mapdata2[curpos+1], mapdata2[curpos+2], mapdata2[curpos+3]);
		if (Compression::decode80((Uint8 *)mapdata2+4+curpos, mapdata1+8192
				*tmpval) != 8192)
		{
			logger->warning("A format80 chunk in the \"MapPack\" was of wrong size\n");
		}
		curpos = curpos + 4 + mapdata2[curpos] + (mapdata2[curpos+1]<<8)
				+ (mapdata2[curpos+2]<<16);
	}

	// Free mapdata2
	if (mapdata2 != 0)
		delete[] mapdata2;
	mapdata2 = 0;

	// 128*128 16-bit template number
	// followed by
	// 128*128 8-bit tile numbers
	bindata = new TileList[width*height];
	tmpval = y*128+x;
	curpos = 0;
	for (ytile = 0; ytile < height; ytile++)
	{
		for (xtile = 0; xtile < width; xtile++)
		{
			// Read template and tile
			bindata[curpos].templateNum = ((Uint16 *)mapdata1)[tmpval];
			bindata[curpos].tileNum = mapdata1[tmpval+128*128*2];
			curpos++;
			tmpval++;
			// printf("tile %d, %d\n", bindata[curpos-1].templateNum, bindata[curpos-1].tileNum);
		}
		// Skip until the end of the line and the onwards to the
		// beginning of usefull data on the next line
		tmpval += (128-width);
	}
	if (mapdata1 != NULL)
		delete[] mapdata1;
	mapdata1 = NULL;

	// Parse Binary Data
	parseBin(bindata);

	// Free bindata
	if (bindata != 0)
		delete[] bindata;
	bindata = 0;
}

void CnCMap::parseBin(TileList* bindata)
{
	Uint32 index;

	Uint16 templ;
	Uint8 tile;
	Uint32 tileidx;
	int xtile, ytile;

	SDL_Surface *tileimg;
	SDL_Color palette[5*256];

	std::map<Uint32, struct tiledata> tilelist;
	std::map<Uint32, struct tiledata>::iterator imgpos;
	//     char tempc[sizeof(Uint8)];

	struct tiledata tiledata;
	Uint32 tiletype;
	tilematrix.resize(width*height);
	terrainoverlay.resize(width*height);

	for (unsigned int i = 0; i < terrainoverlay.size(); i++)
		terrainoverlay[i] = 0;

	// Pallet name is missionData.theater
	loadPal(missionData->theater, palette);
	SHPBase::setPalette(palette);
	SHPBase::calculatePalettes();

	// Load the templates.ini
	//templini = GetConfig("templates.ini");
        INIFile* templini = new INIFile("templates.ini");

	index = 0;
	for (ytile = 0; ytile < height; ytile++)
	{
		for (xtile = 0; xtile < width; xtile++)
		{
			// Read template and tile
			templ = bindata[index].templateNum;
			tile = bindata[index].tileNum;
			index++;
			// Template 0xff is an empty tile
			if (templ == ((maptype == GAME_RA) ? 0xffff : 0xff))
			{
				templ = 0;
				tile = 0;
			}

			// Code sugested by Olaf van der Spek to cause all tiles
			// in template 0 and 2 to be used
			if (templ == 0)
				tile = xtile&3 | (ytile&3 << 2);
			else if (templ == 2)
				tile = xtile&1 | (ytile&1 << 1);

			imgpos = tilelist.find(templ<<8 | tile);

			// set up the tile matrix and load some tiles
			if (imgpos != tilelist.end() )
			{
				// this tile already has a number
				tileidx = imgpos->second.image;
				tiletype = imgpos->second.type;
			}
			else
			{
				// a new tile
				tileimg = loadTile(templini, templ, tile, &tiletype);
				if (tileimg == 0)
				{
					logger->error("Error loading tiles\n");
					throw LoadMapError("Error loading tiles\n");
				}

				SDL_Surface * tmp;
				tmp = SDL_DisplayFormat(tileimg);
				SDL_FreeSurface(tileimg);
				tileimg = tmp;

				tileidx = tileimages.size();
				tiledata.image = tileidx;
				tiledata.type = tiletype;
				tilelist[templ<<8 | tile] = tiledata;
				tileimages.push_back(tileimg);
			}

			// Set the tile in the tilematrix
			tilematrix[width*ytile+xtile] = tileidx;
			if (terraintypes[width*ytile+xtile] == 0){
				terraintypes[width*ytile+xtile] = tiletype;
			}
		}
	}

	// @todo DEBUG
	// @todo REMOVE THAT
	Uint32 index2 = 0;
	for (ytile = 0; ytile < height; ytile++)
	{
		for (xtile = 0; xtile < width; xtile++)
		{
			// Read template and tile
			Uint16 templ2 = bindata[index2].templateNum;
			//Uint8 tile2 = bindata[index2].tileNum;
			index2++;

			if (templ2==0xff){
				printf("   .");
			} else if (templ2<10){
				printf("  %d|", templ2);
			} else if (templ2<100){
				printf(" %d|", templ2);
			} else if (templ2<1000){
				printf("%d|", templ2);
			} else {
				printf("   .");
			}
		}
		printf("\n");
	}
}

/**
 * Overlay loading routines
 */
void CnCMap::loadOverlay(INIFile *inifile)
{
	INIKey key;
	Uint32 linenum;
	Uint16 tx, ty;
	try
	{
		for (Uint32 keynum = 0;;keynum++)
		{
			key = inifile->readKeyValue("OVERLAY", keynum);
			if (sscanf(key->first.c_str(), "%u", &linenum) == 1)
			{
				translateCoord(linenum, &tx, &ty);
				if (!validCoord(tx, ty))
				continue;
				linenum = normaliseCoord(tx, ty);

				this->parseOverlay(linenum, key->second);
			}
		}
	}
	catch(...)
	{}
}

const char	* RAOverlayNames[] =
{
	"SBAG", "CYCL", "BRIK", "FENC", "WOOD",
	"GOLD01", "GOLD02", "GOLD03", "GOLD04", "GEM01",
	"GEM02", "GEM03", "GEM04", "V12", "V13",
	"V14", "V15", "V16", "V17", "V18",
	"FPLS", "WCRATE", "SCRATE",	"FENC", "SBAG"
};

void CnCMap::unOverlayPack(INIFile *inifile)
{
	// Check that the section is ok
	//if (inifile->isSection("OverlayPack")==false) {
	if (inifile->isSection("OVERLAYPACK") == false)
	{
		// Log it
		logger->warning("No \"OverlayPack\" section found for this map.\n");
		// zap the overlay loading
		return;
	}

	unsigned char mapdata[16384]; // 16k
	// Fill first with null
	mapdata[0] = 0;

	// Get number of keys in OverlayPack Section
	int numKeys = inifile->getNumberOfKeysInSection("OVERLAYPACK");
	// read packed data into array
	for (int keynum = 1; keynum < numKeys+1; keynum++)
	{
		stringstream tempStrS;
		tempStrS << keynum;
		if (inifile->isKeyInSection("OVERLAYPACK", tempStrS.str()) == true)
		{
			string stringReaded = inifile->readString("OVERLAYPACK", tempStrS.str().c_str());
			// Copy data in mapdata
			strcat((char*)mapdata, stringReaded.c_str());
		}
	}


	// buffer for decoded data
	unsigned char temp[16384]; // 16k
	// Decode data read with Compression class
	Compression::dec_base64(mapdata, temp, strlen((char*)mapdata));


	// decode the format80 coded data (2 chunks)
	int curpos = 0;
	for (int tmpval = 0; tmpval < 2; tmpval++)
	{
		if (Compression::decode80((Uint8 *)temp+4+curpos, mapdata+8192*tmpval)
				!= 8192)
		{
			logger->warning("A format80 chunk in the \"OverlayPack\" was of wrong size\n");
		}
		curpos = curpos + 4 + temp[curpos] + (temp[curpos+1]<<8)
				+ (temp[curpos+2]<<16);
	}


	for (Uint16 ytile = y; ytile <= y+height; ++ytile)
	{
		for (Uint16 xtile = x; xtile <= x+width; ++xtile)
		{
			Uint32 curpos = xtile+ytile*128;
			Uint32 tilepos = xtile-x+(ytile-y)*width;

			if (mapdata[curpos] == 0xff) // No overlay
				continue;

			if (mapdata[curpos] > 0x17) // Unknown overlay type
				continue;

			// Parse the overlay detected
			parseOverlay(tilepos, RAOverlayNames[mapdata[curpos]]);
		}
	}
}

/**
 */
void CnCMap::parseOverlay(const Uint32& linenum, const string& name)
{
	Uint8 type, frame;
	Uint16 res;

	// Hack !!
	if (name == "BRIK" || name == "SBAG" || name == "FENC" ||
			name == "WOOD" || name == "CYCL" || name == "BARB")
	{
		// Get the num of the player
		Uint8 numPlayer = p::ppool->getPlayerNum("Neutral");

		// Walls are structures.
		p::uspool->createStructure(name.c_str(), linenum, numPlayer, 256, 0, false, "None");
		return;
	}

	string shpname;
	shpname = name + '.' + string(missionData->theater, 3);

	try
	{
		// Remember: imagecache's indexing format is different
		// (imagepool index << 16) | frame
		frame = pc::imgcache->loadImage(shpname.c_str()) >> 16;
	}
	catch(ImageNotFound&)
	{
		shpname = name + ".shp";
		try
		{
			frame = pc::imgcache->loadImage(shpname.c_str()) >> 16;
		}
		catch (ImageNotFound&)
		{
			logger->error("Unable to load overlay \"%s\" (or \"%s.SHP\")\n",
					shpname.c_str(), name.c_str());
			throw LoadMapError("Unable to load overlay " + shpname + " (or " + name + ".SHP)");
		}
	}

	/// @todo Generic resources?
	if (name.substr(0, 2) == "TI" || name.substr(0, 4) == "GOLD"
		|| name.substr(0, 3) == "GEM")
	{
		Uint32 i = 0;
		// @todo CHANGE THAT
		// This is a hack to seed the map with semi-reasonable amounts of
		// resource growth.  This will hopefully become less ugly after the code
		// to manage resource growth has been written.
		if (sscanf(name.c_str(), "TI%u", &i) == 0)
		{
			i = atoi(name.c_str() + (name.length() - 2));
			/* An even worse hack: number of frames in gems is less than the
			 * number of different types of gem. */
			if ('E' == name[1])
				i = 3;
		}
		if (0 == i)
		{
			logger->error("Resource hack for \"%s\" failed.", name.c_str());
			throw LoadMapError("Resource hack for " + name + " failed.");
		}
		map<string, Uint8>::iterator t = resourcenames.find(name);
		if (resourcenames.end() == t)
		{
			type = resourcebases.size();
			/* Encode the type and amount data into the resource matrix's new
			 * cell. */
			res = type | ((i-1) << 8);
			resourcenames[name] = type;
			resourcebases.push_back(frame);
		}
		else
		{
			res = t->second | ((i-1) << 8);
		}
		resourcematrix[linenum] = res;
	}
	else
	{
		overlaymatrix[linenum] |= HAS_OVERLAY;
		overlays[linenum] = frame;

		if (toupper(name[0]) == 'T')
			terraintypes[linenum] = t_tree;
		else if (toupper(name[0]) == 'R')
			terraintypes[linenum] = t_rock;
		else
			terraintypes[linenum] = t_other_nonpass;
	}
}

/**
 * Load a palette
 *
 * @param palette array of SDL_Colour into which palette is loaded.
 */
void CnCMap::loadPal(const string& paln, SDL_Color *palette)
{
	VFile *palfile;
	int i;

	//    SDL_Color UnitOrStructurePalette[17];
	//    string palname = missionData.theater;
	string palname = paln;

	if (palname.length() > 8)
	{
		palname.insert(8, ".PAL");
	}
	else
	{
		palname += ".PAL";
	}
	// Seek the palette file in the mix
	palfile = VFSUtils::VFS_Open(palname.c_str());
	if (palfile == NULL)
	{
		logger->error("Unable to locate palette (\"%s\").\n", palname.c_str());
		throw LoadMapError("Unable to locate palette (" + palname + ")");
	}

	// Load the palette
	//for (int j = 0; j < 2; j++){
	for (i = 0; i < 256; i++)
	{
		palfile->readByte(&palette[i].r, 1);
		palfile->readByte(&palette[i].g, 1);
		palfile->readByte(&palette[i].b, 1);
		palette[i].r <<= 2;
		palette[i].g <<= 2;
		palette[i].b <<= 2;
	}
	//}

#if 0
	SHPBase::setPalette(palette);
	for (i = 0; i < 16; i++)
	{
		palfile->readByte(&UnitOrStructurePalette[i].r, 1);
		palfile->readByte(&UnitOrStructurePalette[i].g, 1);
		palfile->readByte(&UnitOrStructurePalette[i].b, 1);
		UnitOrStructurePalette[i].r <<= 2;
		UnitOrStructurePalette[i].g <<= 2;
		UnitOrStructurePalette[i].b <<= 2;
	}

	SHPBase::setUnitOrStructurePalette(UnitOrStructurePalette);
#endif

	// Close the VFile
	VFSUtils::VFS_Close(palfile);
}

/**
 * Load a tile from the mixfile.
 *
 * @param the mixfiles.
 * @param the template inifile.
 * @param the template number.
 * @param the tilenumber.
 * @returns a SDL_Surface containing the tile.
 */
SDL_Surface* CnCMap::loadTile(INIFile* templini, Uint16 templ,
		Uint8 tile, Uint32* tiletype)
{
	TemplateCache::iterator ti;
	TemplateImage *theaterfile;

	SDL_Surface *retimage;

	char tilefilename[13];
	char tilenum[11];
	char *temname;

	// The name of the file containing the template is something from
	// templates.ini . the three first
	// chars in the name of the theater eg. .DES .TEM .WIN

	sprintf(tilefilename, "TEM%d", templ);
	sprintf(tilenum, "tiletype%d", tile);
	*tiletype = templini->readInt(tilefilename, tilenum, 0);

	temname = templini->readString(tilefilename, "NAME");
	//	printf ("TemName = %s\n", temname);

	if (temname == NULL)
	{
		logger->warning("Error in templates.ini! (can't find \"%s\")\n",
				tilefilename);
		strcpy(tilefilename, "CLEAR1");
	}
	else
	{
		strcpy(tilefilename, temname);
		if (temname != NULL)
			delete[] temname;
		temname = NULL;
	}

	strcat(tilefilename, ".");
	strncat(tilefilename, missionData->theater, 3);

	// Check the templateCache
	ti = templateCache.find(tilefilename);
	// If we haven't preloaded this, lets do so now
	if (ti == templateCache.end())
	{
		try
		{
			if( maptype == GAME_RA )
			{
				theaterfile = new TemplateImage(tilefilename, -1, 1);
			}
			else
			{
				theaterfile = new TemplateImage(tilefilename, -1);
			}
		}
		catch(ImageNotFound&)
		{
			logger->warning("Unable to locate template %d, %d (\"%s\") in mix! using tile 0, 0 instead\n", templ, tile, tilefilename);
			if (templ == 0 && tile == 0)
			{
				logger->error("Unable to load tile 0,0.  Can't proceed\n");
				return NULL;
			}
			return loadTile( templini, 0, 0, tiletype );
		}

		// Store this TemplateImage for later use
		templateCache[tilefilename] = theaterfile;
	}
	else
	{
		theaterfile = ti->second;
	}

	// Now return this SDL_Surface
	retimage = theaterfile->getImage(tile);
	if (retimage == NULL)
	{
		logger->warning(
				"Illegal template %d, %d (\"%s\")! using tile 0, 0 instead\n",
				templ, tile, tilefilename);
		if (templ == 0 && tile == 0)
			return NULL;
		return loadTile(templini, 0, 0, tiletype);
	}

	// Save a cache of this TemplateImage & Tile, so we can reload the SDL_Surface later
	TemplateTilePair* pair = new TemplateTilePair;
	pair->theater = theaterfile;
	pair->tile = tile;

	templateTileCache.push_back(pair);

	return retimage;

}

/**
 * Reloads all the tile's SDL_Image
 */
void CnCMap::reloadTiles()
{
	SDL_Surface *image;

	// Free the old surfaces
	for (vector<SDL_Surface*>::size_type i = 0; i < tileimages.size(); i++)
	{
		if (tileimages[i] != NULL)
		{
			SDL_FreeSurface(tileimages[i]);
		}
		tileimages[i] = NULL;
	}

	tileimages.clear();

	for (TemplateTileCache::iterator i = templateTileCache.begin(); i
			!= templateTileCache.end(); ++i)
	{
		TemplateTilePair* pair = *i;
		image = pair->theater->getImage(pair->tile);
		tileimages.push_back(image);
	}
}

/**
 * Return true if it's the last mission of the game
 */
bool CnCMap::isEndOfGame()
{
	return missionData->endOfGame;
}

/**
 * Loading of TeamTypes
 */
void CnCMap::loadTeamTypes(INIFile* fileIni)
{
    // Checks that fileIni exist
    if (fileIni == 0)
    {
        logger->error("[CnCMap::loadTeamTypes] fileIni == NULL !!!\n");
        return;
    }

    // Check if the "[TeamTypes]" section exist
    if (fileIni->isSection("TeamTypes") == false)
    {
        logger->error("[CnCMap::loadTeamTypes] section [TeamTypes] was not found in ini file.\n");
        return;
    }

    // get number of line in '[TeamTypes]' section of the ini file
    int numberOfKey = fileIni->getNumberOfKeysInSection("TeamTypes");

    int keynum = 0; // Use to parse the key
    INIKey key; // Key to get the key values
    for (keynum = 0; keynum < numberOfKey; keynum++)
    {
        if (maptype == GAME_RA)
        {
            RA_Teamtype team;
            int pos;
            char teamname[255]; // Use to keep the team name

            team.Units.clear();
            key = fileIni->readKeyValue("TeamTypes", keynum);
            team.tname = key->first;
            sscanf(key->second.c_str(), "%i, %i, %i, %i, %i, %i, %i, %i, %[^,]", &team.country, &team.props, &team.unknown1, &team.unknown2, &team.maxteams, &team.waypoint, &team.trigger, &team.numb_teamtypes, teamname);
            pos = key->second.find(teamname, 0);
            string temp = key->second.substr(pos, key->second.size());
            for (int j = 0; j < team.numb_teamtypes; j++)
            {
                RA_TeamUnits unit;
                sscanf(temp.c_str(), "%[^:]:%i ,", teamname, &unit.numb);
                unit.tname = teamname;
                printf("%s line %i: Team = %s, push back unit: %s, %i\n", __FILE__, __LINE__, team.tname.c_str(), unit.tname.c_str(), unit.numb);
                team.Units.push_back(unit);
                pos = temp.find(",", 0);
                temp = temp.substr(pos + 1, temp.size());
                //printf("%s line %i: New string: %s\n", __FILE__, __LINE__, temp.c_str());
            }

            //We should start reading the commands from std::string temp here :)
            //printf("temp = %s\n", temp.c_str());
            string temp3 = string(splitList(cppstrdup(temp.c_str()), ',')[0]);
            //printf("temp3 = %s\n", temp3.c_str());
            pos = temp.find(",", 0);
            string temp2 = temp.substr(pos + 1, temp.size());
            //printf("temp2 = %s\n", temp2.c_str());

            int numcommand;
            sscanf(temp3.c_str(), "%i", &numcommand);
            //team.aiCommandList = new vector<AiCommand*>();
            //team.aiCommandList->resize(numcommand);
            for (int j = 0; j < numcommand; j++)
            {
                int id;
                int wayp;
                sscanf(splitList(cppstrdup(temp2.c_str()), ':')[0], "%i", &id);
                sscanf(splitList(cppstrdup(temp2.c_str()), ':')[1], "%i", &wayp);
                AiCommand* aiCom = new AiCommand();
                aiCom->setId(id);
                aiCom->setWaypoint(wayp);

                pos = temp2.find(",", 0);
                temp2 = temp2.substr(pos + 1, temp2.size());
                //printf("temp2 = %s\n", temp2.c_str());
                printf("id=%d, wayp=%d\n", aiCom->getId(), aiCom->getWaypoint());

                team.aiCommandList.push_back(aiCom);
            }

            RaTeamtypes.push_back(team);
        }
    }
}
