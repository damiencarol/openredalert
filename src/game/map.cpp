#include "include/ccmap.h"

#include <cmath>
#include <iostream>

#include "include/config.h"
#include "include/imageproc.h"
#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "game/Unit.h"
#include "include/UnitAndStructurePool.h"
#include "include/ccmap.h"
#include "include/triggers.h"
#include "MissionData.h"
#include "video/LoadingScreen.h"
#include "video/SHPImage.h"
#include "video/TemplateImage.h"
#include "CellTrigger.h"
#include "Unit.h"

using std::vector;
using std::string;
using std::map;

namespace pc
{
extern ConfigType Config;
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
		waypoints[i] = 0;
	//    this->InitCnCMap ();
	fmax = (double)maxscroll/100.0;

	// Build blank MissionData
	this->missionData = new MissionData();
}

void CnCMap::InitCnCMap(void)
{
	loaded = false;
	this->maptype = pc::Config.gamenum;
	this->gamemode = pc::Config.gamemode;
	scrollstep = pc::Config.scrollstep;
	scrolltime = pc::Config.scrolltime;
	maxscroll = pc::Config.maxscroll;
	/* start at top right corner of map. */
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
	minimap = NULL;
	oldmmap = NULL;
	loading = false;
	
	// If game == RA   =>     translate_64 = false 
	if (pc::Config.gamenum == GAME_TD){
		translate_64 = true;
	} else {
		translate_64 = false;
	}

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
 * @TODO Map loading goes here.
 */
void CnCMap::loadMap(const char* mapname, LoadingScreen* lscreen)
{
	///@ TODO DEBUG 
	if (missionData == NULL)
	{
		logger->error("missionData == NULL !!!\n");
	}
	
	// Copy the map name
	missionData->mapname = new char[strlen(mapname)+1];
	strcpy(missionData->mapname, mapname);
		
	string message = "Reading ";
	message += mapname;
	message += ".INI";

	logger->debug("%s\n", message.c_str());

	loading = true;
	// Load the ini part of the map
	if (lscreen != NULL)
	{
		lscreen->setCurrentTask(message);
	}
	loadIni();

	// Load the bin part of the map (the tiles)
	message = "Loading ";
	message += mapname;
	message += ".BIN";

	if (lscreen != NULL)
	{
		lscreen->setCurrentTask(message);
	}
	if (maptype == GAME_TD)
	{
		loadBin();
	}

	//Path::setMapSize(width, height);
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
	/* scrolling continues at current rate for scrolltime
	 * passes then decays quickly */
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
 * @param the maximum x scroll.
 * @param the maximum y scroll.
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

	/** @TODO: Tile cost should worked out as follows
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
	/** @TODO If unit prefers to be near tiberium (harvester) or should avoid
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
	if (pc::Config.gamenum == GAME_RA)
		miniclip.y = abs(miniclip.sidew - 20 - miniclip.h)/2;
	else
		miniclip.y = abs(miniclip.sidew - miniclip.h)/2;

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
void CnCMap::setTriggerByName(std::string TriggerName, RA_Tiggers *Trig)
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
	return (!(tx < x || ty < y || tx>
					x+width || ty> height+y));
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
RA_Tiggers *CnCMap::getTriggerByNumb(unsigned int TriggerNumb)
{
	if (TriggerNumb >= RaTriggers.size())
		return NULL;
	return &RaTriggers[TriggerNumb];
}
RA_Tiggers *CnCMap::getTriggerByName(std::string TriggerName)
{
	unsigned int j;
	for (unsigned int i = 0; i < RaTriggers.size(); ++i)
	{
		//printf ("%s line %i: String1 = %s, String2 = %s, j = %i, size1 = %i, size2 = %i\n",  __FILE__, __LINE__, TriggerName.c_str(), RaTriggers[i].name.c_str(), j, TriggerName.size(), RaTriggers[i].name.size());

		if (TriggerName.size() != RaTriggers[i].name.size())
			continue;

		for (j = 0; j < TriggerName.size(); ++j)
		{
			//printf ("%s line %i: TriggerName.size() = %i, RaTriggers[i].name.size() = %i, j = %i\n", __FILE__, __LINE__, TriggerName.size(), RaTriggers[i].name.size(), j);
			if (toupper(TriggerName[j]) != toupper(RaTriggers[i].name[j]))
				break;
		}
		//printf ("%s line %i: String1 = %s, String2 = %s, j = %i, size1 = %i, size2 = %i\n",  __FILE__, __LINE__, TriggerName.c_str(), RaTriggers[i].name.c_str(), j, TriggerName.size(), RaTriggers[i].name.size());
		if (j == TriggerName.size())
		{
			//trigger = &RaTriggers[i];
			return &RaTriggers[i];
		}
	}
	return NULL;
}

RA_Teamtype *CnCMap::getTeamtypeByNumb(unsigned int TeamNumb)
{
	if (TeamNumb >= RaTeamtypes.size())
		return NULL;
	return &RaTeamtypes[TeamNumb];
}

RA_Teamtype *CnCMap::getTeamtypeByName(std::string TeamName)
{
	for (unsigned int i = 0; i < RaTeamtypes.size(); i++)
	{
		if (TeamName.size() != RaTeamtypes[i].tname.size())
			continue;
		unsigned int j;
		for (j = 0; j < TeamName.size(); j++)
		{
			if (toupper(TeamName[j]) != toupper(RaTeamtypes[i].tname[j]))
				break;
		}
		if (j == TeamName.size())
		{
			return &RaTeamtypes[i];
		}
	}
	return NULL;
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
SDL_Surface *CnCMap::getMapTile(Uint32 pos)
{
	return tileimages[tilematrix[pos]];
}
Uint32 CnCMap::getTerrainOverlay(Uint32 pos)
{
	if (pos < terrainoverlay.size())
		return terrainoverlay[pos];
	return 0;
}
void CnCMap::setTerrainOverlay(Uint32 pos, Uint32 ImgNum, Uint16 Frame)
{
	if (pos < terrainoverlay.size())
		terrainoverlay[pos] = ImgNum | (Frame &0x7FF);
}
Uint16 CnCMap::getWidth() const {return width;}
Uint16 CnCMap::getHeight() const {return height;}
Uint32 CnCMap::getSize() const {return width*height;}
MissionData* CnCMap::getMissionData() {
        return missionData;
    }
Uint8 CnCMap::getGameMode() const {
        return gamemode;
    }
Uint16 CnCMap::getYScroll() const {
        return scrollpos.cury;
    }
Uint16 CnCMap::getXScroll() const {
        return scrollpos.curx;
    }
Uint32 CnCMap::getScrollPos() const {
        return scrollpos.cury*width+scrollpos.curx;
    }
Uint32 CnCMap::getSmudge(Uint32 pos) const {
        return ((overlaymatrix[pos] & 0xF0) << 12);
    }
Uint32 CnCMap::getTiberium(Uint32 pos) const {
        return (overlaymatrix[pos] & 0xF);
    }
Uint32 CnCMap::getResourceFrame(Uint32 pos) const {
        return ((resourcebases[resourcematrix[pos] & 0xFF]<<16) +
                (resourcematrix[pos]>>8));
    }
bool CnCMap::getResource(Uint32 pos, Uint8* type, Uint8* amount) const {
        if (0 == type || 0 == amount) {
            return (0 != resourcematrix[pos]);
        }
        *type = resourcematrix[pos] & 0xFF;
        *amount = resourcematrix[pos] >> 8;
        return (0 != resourcematrix[pos]);
    }
Uint16 CnCMap::getYTileScroll() const {
        return (Uint16)scrollpos.curytileoffs;
    }
Uint16 CnCMap::getXTileScroll() const {
        return (Uint16)scrollpos.curxtileoffs;
    }
const MiniMapClipping& CnCMap::getMiniMapClipping() const {return miniclip;}
SHPImage* CnCMap::getPips() {
        return pips;
    }
Uint32 CnCMap::getPipsNum() const {
        return pipsnum;
    }
SHPImage* CnCMap::getMoveFlash() {
        return moveflash;
    }
Uint32 CnCMap::getMoveFlashNum() const {
        return flashnum;
    }
Uint16 CnCMap::getX() const {
        return x;
    }
Uint16 CnCMap::getY() const {
        return y;
    }
