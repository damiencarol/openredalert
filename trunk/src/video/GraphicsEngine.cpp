// GraphicsEngine.cpp
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

#include "GraphicsEngine.h"

#include <string>
#include <stdexcept>
#include <vector>
#include <cmath>

#include "SDL/SDL_timer.h"
#include "SDL/SDL_video.h"

#include "misc/common.h"
#include "misc/config.h"
#include "include/Logger.h"
#include "game/CnCMap.h"
#include "game/PlayerPool.h"
#include "game/Player.h"
#include "game/Unit.hpp"
#include "game/UnitAndStructurePool.h"
#include "game/StructureType.h"
#include "audio/SoundEngine.h"
#include "include/sdllayer.h"
#include "misc/StringTableFile.h"
#include "ui/Cursor.h"
#include "ui/RA_Label.h"
#include "ui/Input.h"
#include "ui/Sidebar.h"
#include "VideoError.h"
#include "MessagePool.h"
#include "ImageCache.h"
#include "ImageCacheEntry.h"
#include "Dune2Image.h"

#ifndef VERSION
#define VERSION "6xx"
#endif

using std::string;
using std::runtime_error;

namespace pc {
	extern ImageCache* imgcache;
    extern ConfigType Config;
    extern Cursor* cursor;
    //extern vector<SHPImage*>* img
    extern MessagePool* msg;
    extern Sidebar* sidebar;
}
extern Logger * logger;


/**
 * Constructor, inits the sdl graphics and prepares to render maps and
 * draw vqaframes.
 *
 * @param commandline arguments regarding video.
 */
GraphicsEngine::GraphicsEngine()
{
    ConfigType config;
    config = getConfig();
    width = config.width;
    height = config.height;
	drawFogOfWar = true;//false;
//	char VideoDriverName[20];
	StringTableFile* stringFile = new StringTableFile("conquer.eng");

	FrameRateSurface = 0;
	MoneySurface = 0;
	OptionsSurface = 0;


	/// Set the caption of the game window
	string caption = "OpenRedAlert - ";
    caption += VERSION;
	SDL_WM_SetCaption(caption.c_str(), 0);


	SDL_WM_GrabInput(config.grabmode);

	// Init the icon of the main window
	//icon = SDL_LoadBMP_RW(SDL_RWFromFile("data/gfx/icon.bmp", "rb"), 1);
	//if (icon != 0){
	//	SDL_WM_SetIcon(icon, 0);
	//}

	//config.videoflags = SDL_SWSURFACE|SDL_ANYFORMAT|SDL_GLSDL|SDL_DOUBLEBUF;

	screen = SDL_SetVideoMode(width, height, config.bpp, config.videoflags);

	if (screen == 0) {
		logger->error("Unable to set %dx%d video: %s\n", width, height, SDL_GetError());
		//@todo throw VideoError("Unable to set " + width + "x" + height + " video: " );//+ SDL_GetError());
	}

	if ( (screen->flags & 0x00004000	/* Surface is RLE encoded */) == 0x00004000	/* Surface is RLE encoded */ ) {
		printf("Sprite blit uses RLE acceleration\n");
	}

	// Indicates where is the screen mem
	/*
	if ( (screen->flags & SDL_HWSURFACE) == SDL_HWSURFACE ) {
		logger->note("Screen is in video memory\n");
	} else {
		logger->note("Screen is in system memory\n");
	}
	*/

	// Indicate if we use OpenGL
	/*
	if ( (screen->flags & SDL_OPENGL) == SDL_OPENGL ) {
		logger->note("Using opengl\n");
	}
	*/

	whitepix	= SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);
	greenpix	= SDL_MapRGB(screen->format, 0, 0xff, 0);
	yellowpix	= SDL_MapRGB(screen->format, 0xff, 0xff, 0);
	redpix		= SDL_MapRGB(screen->format, 0xff, 0, 0);
	blackpix	= SDL_MapRGB(screen->format, 0, 0, 0);

	pc::imgcache = new ImageCache();

	repairing_icon = 0;

	try {
		pc::msg = new MessagePool();
		pc::msg->setWidth(width);
	} catch(...) {
		logger->error("Unable to create message pool (missing font?)\n");
		throw VideoError("Unable to create message pool (missing font?)");
	}
	logger->renderGameMsg(true);


	firstframe = SDL_GetTicks();
	frames = 0;
	clearBack = false;

	FrameRateLabel = 0;
	OptionsLabel = 0;
	MoneyLabel = 0;

	//oldmouse.x = oldmouse.y = oldmouse.w = oldmouse.h = 0;
	memset(&oldmouse, 0, sizeof (SDL_Rect));


	// Create the label for victory msg
	victoryLabel = new RA_Label();
	victoryLabel->setColor(0xAA, 0x00, 0x00);
	victoryLabel->SetFont("vcr.fnt");
	victoryLabel->UseAntiAliasing(false);
	victoryLabel->setText(stringFile->getString(15));

	// Create the label for defeat msg
	defeatLabel = new RA_Label();
	defeatLabel->setColor(0xAA, 0x00, 0x00);
	defeatLabel->SetFont("vcr.fnt");
	defeatLabel->UseAntiAliasing(false);
	defeatLabel->setText(stringFile->getString(16));

	// Free the string file
	delete stringFile;
}

/**
 * Destructor, free the memory used by the graphicsengine.
 */
GraphicsEngine::~GraphicsEngine()
{
	if (pc::imgcache != 0){
		delete pc::imgcache;
	}
	pc::imgcache = 0;

    logger->renderGameMsg(false);

	if (pc::msg != 0)
		delete pc::msg;
    pc::msg = 0;
	//if (icon != 0)
	//	SDL_FreeSurface(icon);
	//icon = 0;

	if (FrameRateSurface != 0)
		SDL_FreeSurface (FrameRateSurface);

	if (MoneySurface != 0)
		SDL_FreeSurface (MoneySurface);

	if (OptionsSurface == 0)
		SDL_FreeSurface (OptionsSurface);

	if (FrameRateLabel != 0){
		delete FrameRateLabel;
	}
	if (OptionsLabel != 0){
		delete OptionsLabel;
	}
	if (MoneyLabel != 0){
		delete MoneyLabel;
	}

    // Delete Label for victory message
    if (victoryLabel != 0){
    	delete victoryLabel;
    }
    // Delete Label for defeat message
    if (defeatLabel != 0){
    	delete defeatLabel;
    }
}

/**
 * Setup the vars for the current mission.
 *
 * @param the current map.
 * @param the current sidebar.
 * @param the current cursor.
 */
void GraphicsEngine::setupCurrentGame()
{
    // assume the width of tile 0 is correct
    tilewidth	= p::ccmap->getMapTile(0)->w;
	tileheight	= p::ccmap->getMapTile(0)->h;

    // assume sidebar is visible for initial maparea (will be changed at
    // frame 1 if it isn't anyway)
    maparea.h = height-pc::sidebar->getTabLocation()->h;
    maparea.y = pc::sidebar->getTabLocation()->h;
    maparea.w = width-pc::sidebar->getTabLocation()->w;
    maparea.x = 0;
    if (maparea.w > p::ccmap->getWidth()*tilewidth) {
        maparea.w = p::ccmap->getWidth()*tilewidth;
    }
    if (maparea.h > p::ccmap->getHeight()*tilewidth) {
        maparea.h = p::ccmap->getHeight()*tilewidth;
    }
    p::ccmap->setMaxScroll(maparea.w/tilewidth, maparea.h/tilewidth,
                      maparea.w%tilewidth, maparea.h%tilewidth, tilewidth);
    if (getConfig().bpp == 8) {
        // This doesn't work, but the cursors look nicer so leaving it in to
        // remind me to look at cursor rendering again.
        SDL_SetColors(screen, SHPBase::getPalette(0), 0, 256);
    }
    // this is a new game so clear the screen
    clearScreen();
    p::ccmap->prepMiniClip(pc::sidebar->getTabLocation()->w,pc::sidebar->getTabLocation()->h);
    minizoom.normal = (tilewidth*pc::sidebar->getTabLocation()->w)/max(maparea.w, maparea.h);
    minizoom.max    = max(1,tilewidth*pc::sidebar->getTabLocation()->w / max(
                p::ccmap->getWidth()*tilewidth, p::ccmap->getHeight()*tilewidth))+1;
    mz = &minizoom.max;
//	mz = &minizoom.normal;
//printf ("Minizoom = %i\n", *mz);
    playercolours.resize(SHPBase::numPalettes());
    for (Uint8 i = 0; i < playercolours.size() ; ++i) {
        // Magic Number 179: See comment at start of
        // shpimage.cpp
    	if (pc::Config.gamenum == GAME_TD) {
        	playercolours[i] = SHPBase::getColour(screen->format, i, 179);
        }else if (pc::Config.gamenum == GAME_RA) {
        	playercolours[i] = SHPBase::getColour(screen->format, i, 85);
        }
    }
}

/**
 * Render a scene complete with map, sidebar and cursor.
 *
 * @param flipscreen true if flip screen function will be call
 */
void GraphicsEngine::renderScene(bool flipscreen)
{
	
	SDL_Surface *curimg = 0;
	SDL_Rect dest = {0, 0, 0, 0};
	SDL_Rect src = {0, 0, 0, 0};
	SDL_Rect udest = {0, 0, 0, 0};

#ifdef _MSC_VER
	//VS will cry if you try to use these variables that were not initialized
	dest.h = dest.w = src.h = src.w = udest.h = udest.w = 0;
	dest.x = dest.y = src.x = src.y = udest.x = udest.y = 0;
#endif
        
	// remove the old mousecursor
//	SDL_FillRect(screen, &oldmouse, blackpix);

	// draw the side bar, between 0.001 and 0.000 sec
	drawSidebar();

	// Draw the mini map, 0.000 sec
	DrawMinimap();

	// render the map, between 0.004 and  0.005 sec
	DrawMap(dest, src, udest);

	// draw the level two overlays, 0.000 sec
	DrawL2Overlays();

	// Draw the fog of war, between 0.001 and 0.002 sec
	DrawFogOfWar(dest, src, udest);

	// draw the selectionbox, 0.000 sec
	DrawSelectionBox();

	// Draw the message "Victory" or "defeat"
	drawMissionLabel();

	// Draw messages
	curimg = pc::msg->getMessages();
	if (curimg != 0) {
		dest.x = maparea.x;
		dest.y = maparea.y;
		dest.w = curimg->w;
		dest.h = curimg->h;

		SDL_UpperBlit(curimg, 0, screen, &dest);
	}

	//	printf ("%s line %i: Elapsed = %02f sec\n", __FILE__, __LINE__, (float)((double)SDL_GetTicks() - (double)starttick)/(double)1000);
	//printf("%d structure in the pool\n", p::uspool->getNumbStructures());

	if (flipscreen){
		// Draw the mouse cursor to the screen, between 0.000 and 0.001 sec
		DrawMouse();
		DrawTooltip();

		SDL_Flip(screen);
	}

#ifdef _WIN32
//	SDL_FillRect(screen, &oldmouse, blackpix);
#endif
}

/**
 * Draw a VQA frame to the screen.
 *
 * @param frame the vqa frame.
 */
void GraphicsEngine::drawVQAFrame(SDL_Surface* frame)
{
    SDL_Rect dest;
    dest.w = frame->w;
    dest.h = frame->h;
    dest.x = (screen->w-frame->w)>>1;
    dest.y = (screen->h-frame->h)>>1;
    SDL_UpperBlit(frame, 0, screen, &dest);
    SDL_Flip(screen);
}

/**
 * Clear the frontBuffer, i.e. paint it black.
 */
void GraphicsEngine::clearBuffer()
{
    SDL_Rect dest;

    dest.x = 0;
    dest.y = 0;
    dest.w = width;
    dest.h = height;

    SDL_FillRect(screen, &dest, blackpix);

}

/**
 * Clear the screen, i.e. paint it black.
 */
void GraphicsEngine::clearScreen()
{
    SDL_Rect dest;

    dest.x = 0;
    dest.y = 0;
    dest.w = width;
    dest.h = height;

    SDL_FillRect(screen, &dest, blackpix);

    /* directx (and possibly other platforms needs to clear both
    front and back buffer */

    SDL_Flip(screen);
    SDL_FillRect(screen, &dest, blackpix);

}

/**
 * Render a loading screen
 *
 * @param buff Message to show
 * @param logo background screen of the loading screen
 */
void GraphicsEngine::renderLoading(const string& buff, SDL_Surface* logo)
{
    SDL_Rect dest;
    SDL_Surface *curimg;
    int i;
    static int numdots = 0;

    dest.x = 0;
    dest.y = 0;
    dest.w = width;
    dest.h = height;

    SDL_FillRect(screen, &dest, blackpix);

    if (logo != 0) {
        dest.x = (width-logo->w)/2;
        dest.y = (height-logo->h)/2;
        SDL_UpperBlit(logo,0,screen,&dest);
    }
    dest.x = dest.y = 0;

    std::string msg(buff);
    for (i = 0; i < numdots; ++i) {
        msg += ".";
    }
    numdots = (numdots+1)%4;

    pc::msg->postMessage(msg);

    curimg = pc::msg->getMessages();
    if (curimg != 0) {
        // This used to center the text, but since moving to a fixed width
        // message pool, this isn't quite centerred
        dest.x = (width-curimg->w)/2;
        dest.y = (height-curimg->h)/2;
        dest.w = curimg->w;
        dest.h = curimg->h;
        SDL_UpperBlit(curimg, 0, screen, &dest);
    }
    pc::msg->clear();

    SDL_Flip(screen);

}

/**
 * Get a screen position from a map position
 *
 * @param MapPos position in map coordinates
 * @param ScreenX returning screen x position
 * @param ScreenY returning screen y position
 * @return true if the position is on the screen, false if the position is outside the screen
 */
bool GraphicsEngine::MapPosToScreenXY (Uint32 MapPos, Sint16 *ScreenX, Sint16 *ScreenY, CnCMap* map)
{
	Uint32 scrollpos = map->getScrollPos();

	if (scrollpos > MapPos){
		return false;
	}

	map->translateFromPos((MapPos-scrollpos), (Uint16*) ScreenX, (Uint16*)ScreenY);

	*ScreenX *= tilewidth;
	*ScreenY *= tileheight;

	if (*ScreenX >= screen->w || *ScreenY >= screen->h){
		return false;
	}

	*ScreenX -= map->getXTileScroll();
	*ScreenY -= map->getYTileScroll();

	return true;
}

/**
 * Render the top left selection box.
 */
void GraphicsEngine::DrawSelectionBox ()
{
	SDL_Rect	dest;

	if (Input::isDrawing()) {
		dest.x = min(Input::getMarkRect().x, (Sint16)Input::getMarkRect().w);
		dest.y = min(Input::getMarkRect().y, (Sint16)Input::getMarkRect().h);
		dest.w = abs(Input::getMarkRect().x - Input::getMarkRect().w);
		dest.h = 1;
		SDL_FillRect(screen, &dest, whitepix);
		dest.y += abs(Input::getMarkRect().y - Input::getMarkRect().h);
		SDL_FillRect(screen, &dest, whitepix);
		dest.y -= abs(Input::getMarkRect().y - Input::getMarkRect().h);
		dest.h = abs(Input::getMarkRect().y - Input::getMarkRect().h);
		dest.w = 1;
		SDL_FillRect(screen, &dest, whitepix);
		dest.x += abs(Input::getMarkRect().x - Input::getMarkRect().w);
		SDL_FillRect(screen, &dest, whitepix);
	}

	dest.x = dest.y = 0;
	dest.w = width;
	dest.h = height;
	SDL_SetClipRect( screen, &dest);
}

/**
 * Render the cursor.
 */
void GraphicsEngine::DrawMouse()
{
	SDL_Surface	*curimg = 0;
	SDL_Rect	dest;
	//oldmouse = {0, 0, 0, 0}; // Old var

	// Draw the mouse
	dest.x = pc::cursor->getX();
	dest.y = pc::cursor->getY();
	if (dest.x < 0){
		dest.x = 0;
	}
	if (dest.y < 0){
		dest.y = 0;
	}
	// Get the image
	curimg = pc::cursor->getCursor();
	// Set Height and Width
	dest.w = curimg->w;
	dest.h = curimg->h;

	// Blit to the screen
	SDL_UpperBlit(curimg, 0, screen, &dest);

	// Copy in local variable (oldmouse = dest;)
	memcpy(&oldmouse, &dest, sizeof (SDL_Rect));
}

/**
 * Render the tooltip.
 */
void GraphicsEngine::DrawTooltip()
{
	SDL_Surface*	curimg = 0;
	SDL_Rect		dest;

	dest.x = pc::cursor->getX();
	dest.y = pc::cursor->getY();

	curimg = pc::cursor->getTooltip ();

	if (curimg != 0){

		dest.x -= curimg->w;
		dest.x -= 2;
		if (dest.x < 0)
			dest.x = 0;

		dest.w = curimg->w;
		dest.h = curimg->h;

		SDL_UpperBlit(curimg, 0, screen, &dest);
	}
}

/**
 * Render the radar mini map
 */
void GraphicsEngine::DrawMinimap()
{
	Sint16		xpos;
	Sint16		ypos;
	SDL_Rect	dest;
	SDL_Rect	src;
	Uint32		curpos;
	Player* 	lplayer;

	// Get the Local player
	lplayer = p::ccmap->getPlayerPool()->getLPlayer();

	// Get the visibility of the local player
	vector<bool>* mapvis = lplayer->getMapVis();

    // draw minimap
    if (lplayer->getNumberRadars()>0 && !pc::sidebar->isRadaranimating())
    {
        const Uint8 minizoom = *mz;
        const Uint32 mapwidth = p::ccmap->getWidth();
        const Uint32 mapheight = p::ccmap->getHeight();
        const MiniMapClipping& clip = (MiniMapClipping) p::ccmap->getMiniMapClipping();
        Uint8 minx;
        Uint32 cury;
        bool blocked;

        // Need the exact dimensions in tiles
        // @todo Positioning needs tweaking
        SDL_Surface *minimap = p::ccmap->getMiniMap(minizoom);
        // Draw black under minimap if haven't previously drawn (or was drawn,
        // then disabled, then reenabled).

        if (minimap == 0){
        	return;
        }

        dest.w = clip.w;
        dest.h = clip.h;
        dest.x = maparea.x+maparea.w+clip.x;
        dest.y = maparea.y+clip.y;

        src.x = p::ccmap->getXScroll()*minizoom;
        src.y = p::ccmap->getYScroll()*minizoom;
        src.w = dest.w;
        src.h = dest.h;
        if (src.x + src.w >= minimap->w) {
            src.x = minimap->w - src.w;
        }
        if (src.y + src.h >= minimap->h) {
            src.y = minimap->h - src.h;
        }
        SDL_UpperBlit(minimap, &src, screen, &dest);
        // Draw black over not visible parts
        curpos = p::ccmap->getScrollPos();
        minx = min(curpos%mapwidth, mapwidth - clip.tilew);
        cury = min(curpos/mapwidth, mapheight - clip.tileh);
        curpos = minx+cury*mapwidth;
        
        for (ypos = 0; ypos < clip.tileh ; ++ypos) 
        {
            for (xpos = 0 ; xpos < clip.tilew ; ++xpos) 
            {
                if (mapvis->at(curpos)) {
 			float width, height;
			Uint8 igroup, owner, pcol;
			Uint32 cellpos;

			// This is needed to prevent buildings/units be drawn outside the radar area
			if (xpos < clip.tilew && ypos < clip.tileh){
				// Rather than make the graphics engine depend on the
				// UnitOrStructureType, just pull what we need from the
				// USPool.
				if (p::uspool->getUnitOrStructureLimAt(curpos, &width,
					&height, &cellpos, &igroup, &owner, &pcol, &blocked)) {
					/// @todo drawing infanty groups as smaller pixels
					if (blocked) {
						dest.x = maparea.x+maparea.w+clip.x+xpos*minizoom;
						dest.y = maparea.y+clip.y+ypos*minizoom;
						dest.w = (Uint16)ceil(width*minizoom);
						dest.h = (Uint16)ceil(height*minizoom);
						SDL_FillRect(screen, &dest, playercolours[pcol]);
					}
				}
			}
                } else {

                    // Draw a black square here
                    dest.x = maparea.x+maparea.w+clip.x+xpos*minizoom;
                    dest.y = maparea.y+clip.y+ypos*minizoom;
                    dest.w = minizoom+2;
                    dest.h = dest.w+1;
                    SDL_FillRect(screen, &dest, blackpix);
                }
                ++curpos;
            }
            ++cury;
            curpos = minx+cury*mapwidth;
        }
    }

   SDL_SetClipRect(screen, &maparea);
}

/**
 * Handle drawing the structures
 */
void GraphicsEngine::DrawStructures()
{
	//static Uint32	last_repair_tick;
	//static bool		DrawSmallRepair;

	Structure*		str = 0;
	Sint16			xpos;
	Sint16			ypos;
	//SDL_Surface*	RepairImg = 0;
	//SDL_Rect		udest;
	//int				scaleq = -1; // For image (always -1 for RA)

	//printf("p::uspool->getNumbStructures()=%d\n", p::uspool->getNumbStructures());

	// For all structures
	for (unsigned int i = 0; i < p::uspool->getNumbStructures(); i++)
	{
		// Get a structure
		str = p::uspool->getStructure(i);
		// Check that we actually found a structure
		if (str == 0){
			//printf("Structur = NULL\n");
			continue;
		}
		//printf("Structure type = %s\n", str->getType()->getTName());

		// Check if the structure is being build
		if (str->IsBuilding()){
			continue;
		}

		// Load the repairing icon image (displayed while repairing a structure
		//if (repairing_icon == 0)
		//	repairing_icon = pc::imgcache->loadImage("select.shp", scaleq);

		// Update the animation icon (small or large)
		//if ( (SDL_GetTicks() - last_repair_tick) > 300){
		//	if (DrawSmallRepair){
		//		DrawSmallRepair = false;
		//	}else{
		//		DrawSmallRepair = true;
		//	}
		//	last_repair_tick = SDL_GetTicks();
		//}

		// Get the animated icon (small or large)
		//if (DrawSmallRepair){
		//	RepairImg = pc::imgcache->getImage(repairing_icon,3).image;
		//}else{
		//	RepairImg = pc::imgcache->getImage(repairing_icon,2).image;
		//}

		//if (RepairImg == 0){
		//	continue;
		//}

		// Get x and y coords with a hash cursor and
		// Check that structure is in the screen
		if (!MapPosToScreenXY(str->getPos(), &xpos, &ypos, p::ccmap)){
			continue;
		}

		// Get the image
		/*SDL_Surface* img = pc::imgcache->getImage(str->getImageNum(0)).image;
		// Draw Structure
		udest.x = xpos + (str->getType()->getXsize() * tilewidth);//- (RepairImg->w/2); //+ (str->getType()->getXsize() * tilewidth)
		udest.y = ypos + (str->getType()->getYsize() * tileheight);// - (RepairImg->h/2)// + (str->getType()->getYsize() * tileheight);
		udest.w = img->w;
		udest.h = img->h;
		SDL_UpperBlit(img, 0, screen, &udest);*/
	}
}

/**
 * Handle drawing the repair icon for structures that are being repaired
 */
void GraphicsEngine::DrawRepairing()
{
	static Uint32	last_repair_tick;
	static bool		DrawSmallRepair;

	Structure		*str = 0;

	Sint16			xpos, ypos;
	SDL_Surface		*RepairImg = 0;
	SDL_Rect		udest;
	int				scaleq = -1;

	// For all structures
	for (unsigned int i = 0; i < p::uspool->getNumbStructures(); i++){

		// For now we only do this for red alert
		if (pc::Config.gamenum != GAME_RA){
			continue;
		}

		// Get a structure
		str = p::uspool->getStructure(i);

		// Check that we actually found a structure
		if (str == 0){
			continue;
		}

		// Check if the structure is being repaired
		if (!str->isRepairing ()){
			continue;
		}

		// Load the repairing icon image (displayed while repairing a structure
		if (repairing_icon == 0)
			repairing_icon = pc::imgcache->loadImage("select.shp", scaleq);

		// Update the animation icon (small or large)
		if ( (SDL_GetTicks() - last_repair_tick) > 300){
			if (DrawSmallRepair){
				DrawSmallRepair = false;
			}else{
				DrawSmallRepair = true;
			}
			last_repair_tick = SDL_GetTicks();
		}

		// Get the animated icon (small or large)
		if (DrawSmallRepair){
			RepairImg = pc::imgcache->getImage(repairing_icon,3).image;
		}else{
			RepairImg = pc::imgcache->getImage(repairing_icon,2).image;
		}

		if (RepairImg == 0){
			continue;
		}

		if (!MapPosToScreenXY (str->getPos(), &xpos, &ypos, p::ccmap)){
			continue;
		}

		// Draw the repair icon (animation)
                StructureType* theType = dynamic_cast<StructureType*>(str->getType());
		udest.x = xpos + (theType->getXsize() * tilewidth/2)- (RepairImg->w/2); //+ (str->getType()->getXsize() * tilewidth)
		udest.y = ypos + (theType->getYsize() * tileheight/2) - (RepairImg->h/2); // + (str->getType()->getYsize() * tileheight)
		udest.w = RepairImg->w;
		udest.h = RepairImg->h;
		SDL_UpperBlit(RepairImg, 0, screen, &udest);
	}
}

/**
 * Handle drawing the BOMB (C4) icon for structures that are being bombing
 */
void GraphicsEngine::DrawBombing()
{
	static Uint32	last_repair_tick;
	static Uint8	numImage;

	Structure		*str = 0;

	Sint16			xpos;
	Sint16			ypos;
	SDL_Surface*	bombImage = 0;
	SDL_Rect		udest;
	int				scaleq = -1;

	// For all structures
	for (unsigned int i = 0; i < p::uspool->getNumbStructures(); i++)
	{
		// For now we only do this for red alert
		if (pc::Config.gamenum != GAME_RA){
			continue;
		}

		// Get a structure
		str = p::uspool->getStructure(i);

		// Check that we actually found a structure
		if (str == 0){
			continue;
		}

		// Check if the structure is being bombing
		if (str->isBombing()==false){
			continue;
		}
		//printf("str bb = %s\n",
		//		p::uspool->getStructure(i)->getType()->getTName());

		// Load the bombing icon image (displayed while bombing a structure)
		if (bombing_icon == 0){
			bombing_icon = new Dune2Image("mouse.shp", scaleq);
			//bombing_icon = pc::imgcache->loadImage("mouse.shp", scaleq);
		}

		// Update the animation icon (from the first (116) to the last (118))
		if ( (SDL_GetTicks() - last_repair_tick) > 1000){//300
			if (numImage>117 || numImage<116){
				numImage = 116;
			}else{
				numImage++;
			}
			last_repair_tick = SDL_GetTicks();
		}

		// Get the icon (116, 117 or 118)
		//bombImage = pc::imgcache->getImage(bombing_icon, 116).image;
		bombImage = bombing_icon->getImage(numImage);

		if (bombImage == 0){
			logger->error("bombimage = 0 \n");
			continue;
		}

		if (!MapPosToScreenXY(str->getPos(), &xpos, &ypos, p::ccmap)){
			continue;
		}

		// Draw the repair icon (animation)
                StructureType* theType = dynamic_cast<StructureType*>(str->getType());
		udest.x = xpos + (theType->getXsize() * tilewidth - bombImage->w)/2; //+ (str->getType()->getXsize() * tilewidth)
		udest.y = ypos + (theType->getYsize() * tileheight - bombImage->h)/2; /* + (str->getType()->getYsize() * tileheight) */
		udest.w = bombImage->w;
		udest.h = bombImage->h;
		// Draw the icon
		SDL_BlitSurface(bombImage, 0, screen, &udest);
	}
}

/**
 * Handle drawing the some on a damaged vehicule
 */
void GraphicsEngine::DrawVehicleSmoke(void)
{
	Unit		*un = 0;
	Uint32		NumbSmokeImg;

	NumbSmokeImg = pc::imgcache->getNumbImages("smoke_m.shp");

//	printf ("%s line %i: Numb smoke img = %i\n", __FILE__, __LINE__, NumbSmokeImg);

	for (unsigned int i = 0; i < p::uspool->getNumbUnits (); i++){

		// For now we only do this for red alert
		if (pc::Config.gamenum != GAME_RA)
			continue;

		// Get a Unit
		un = p::uspool->getUnit(i);

		// Check that we actually found a unit
		if (un == 0)
			continue;

		Uint16 MaxHealth = un->getType()->getMaxHealth();
		Uint16 Health = un->getHealth();

		// Check if we should draw smoke
		if (Health < MaxHealth){

			// @todo Implement the smoking anim on vehicule
		}

#if 0
		// Load the repairing icon image (displayed while repairing a structure
		if (repairing_icon == 0)
			repairing_icon = pc::imgcache->loadImage("select.shp", scaleq);

		// Update the animation icon (small or large)
		if ( (SDL_GetTicks() - last_repair_tick) > 300){
			if (DrawSmallRepair)
				DrawSmallRepair = false;
			else
				DrawSmallRepair = true;
			last_repair_tick = SDL_GetTicks();
		}

		// Get the animated icon (small or large)
		if (DrawSmallRepair)
			RepairImg = pc::imgcache->getImage(repairing_icon,3).image;
		else
			RepairImg = pc::imgcache->getImage(repairing_icon,2).image;

		if (RepairImg == NULL)
			continue;

		if (!MapPosToScreenXY (str->getPos(), &xpos, &ypos ))
			continue;

		// Draw the repair icon (animation)
		udest.x = xpos + (str->getType()->getXsize() * tilewidth/2)- (RepairImg->w/2); //+ (str->getType()->getXsize() * tilewidth)
		udest.y = ypos + (str->getType()->getYsize() * tileheight/2) - (RepairImg->h/2)/* + (str->getType()->getYsize() * tileheight) */;
		udest.w = RepairImg->w;
		udest.h = RepairImg->h;
		SDL_BlitSurface(RepairImg, NULL, screen, &udest);
#endif
	}
}

/**
 * Render the health bars for selected structures
 */
void GraphicsEngine::DrawStructureHealthBars(SDL_Rect dest, SDL_Rect udest, Uint32 curdpos)
{
	double		ratio;
	Structure*	str = 0;
	Uint16		selection;

	// Check for a position outside the map
	if (curdpos >= p::ccmap->getSize())
		return;

	selection = p::uspool->getSelected(curdpos);

	if ((selection&0xff) != 0)
    {
        // Get the structure at the position
		str = p::uspool->getStructureAt(curdpos);

		if (str == 0)
			return;

		udest.h = 5;

		if (str->isSelected() && udest.w >= 2 )
		{
			ratio = str->getRatio();
			SDL_FillRect(screen, &udest, blackpix);
			udest.h -= 2;
			++udest.x;
			++udest.y;
			udest.w = (Uint16)((double)(udest.w-2) * ratio);
			SDL_FillRect(screen, &udest, ((ratio<=0.5)?(ratio<=0.25?redpix:yellowpix):greenpix));
		}
	}
}

/**
 * Render the health bars for selected units
 */
void GraphicsEngine::DrawGroundUnitHealthBars(SDL_Rect dest, SDL_Rect udest, Uint32 curdpos)
{
	Uint16		selection;
	double		ratio;
	Unit		*un = 0;

	// Check for a position outside the map
	if (curdpos >= p::ccmap->getSize()){
		return;
	}

	selection = p::uspool->getSelected(curdpos);

	if ((selection&0xff) != 0){
		udest.h = 5;

		// Check for infantry unit
		if (selection>>8 == 0xff) {

			// For each infantry position
			for (int i = 0; i < 5; i++){
				if (selection &(1<<i)) {
					un  = p::uspool->getGroundUnitAt(curdpos,i);
					if (un == 0)
						continue;
					if (un->isSelected()){
						ratio   = un->getRatio();
						udest.x = dest.x + un->getXoffset();
						udest.y = dest.y + un->getYoffset();
						// Draw the health bar for each infantry unit in a diffirent position
						if (i == 0){
							udest.x += 6;
							udest.y += 5;
						}else if (i == 2){
							udest.x += 12;
						}else if (i == 3){
							udest.y += 10;
						}else if (i == 4){
							udest.x += 12;
							udest.y += 10;
						}
						clipToMaparea(&udest);
						udest.w = 12;
						udest.h = 5;
						SDL_FillRect(screen, &udest, blackpix);
						udest.w = (Uint16)(10.0 * ratio);
						udest.h -= 2;
						++udest.x;
						++udest.y;
						SDL_FillRect(screen, &udest, ((ratio<=0.5)?(ratio<=0.25?redpix:yellowpix):greenpix));
					}
				}
			}
		} else {
			// Not a infantry unit
			un  = p::uspool->getGroundUnitAt(curdpos);

			if (un == 0)
				return;

			// Draw the health bar
			if (un->isSelected() && udest.w >= 2 ) {
				ratio = un->getRatio();
				SDL_FillRect(screen, &udest, blackpix);
				udest.h -= 2;
				++udest.x;
				++udest.y;
				udest.w = (Uint16)((double)(udest.w-2) * ratio);
				SDL_FillRect(screen, &udest, ((ratio<=0.5)?(ratio<=0.25?redpix:yellowpix):greenpix));
			}

			// Draw the harvester contents (if needed)
			if ((un->isSelected()) && (un->getType()->getName() == "HARV"))
			{
				if (un->getOwner() == p::ccmap->getPlayerPool()->getLPlayerNum()){
					Uint8 ResourceType;
					Uint8 NumbResources = un->GetNumResources ();
					for (int k = 0; k < 5; k++){
						udest.x = dest.x + 7 + un->getXoffset() + k * 7;
						udest.y = dest.y + 43 + un->getYoffset();
						clipToMaparea(&udest);
						udest.w = 5;
						udest.h = 5;
						SDL_FillRect(screen, &udest, blackpix);

						if (un->GetResourceType (k, &ResourceType))
							udest.w = (Uint16)3.0;
						else
							udest.w = (Uint16)0;
						udest.h -= 2;
						++udest.x;
						++udest.y;

						// In ra resource types higher than 4 are normal ore,
						// 4 and smaller are christals
						if (NumbResources >= k)
							SDL_FillRect(screen, &udest,  ResourceType > 4?greenpix:redpix);
					}
				}
			}
		}
	}
}

/**
 * Render the health bars for selected flying units
 */
void GraphicsEngine::DrawFlyingUnitHealthBars(SDL_Rect dest, SDL_Rect	udest, Uint32 curdpos)
{
	double			ratio;
	Unit			*un = 0;
	Uint16			selection;

	// Check for a position outside the map
	if (curdpos >= p::ccmap->getSize()){
		return;
	}

	selection = p::uspool->getSelected(curdpos);

	if ((selection&0xff) != 0){

		un  = p::uspool->getFlyingAt(curdpos);

		if (un == 0)
			return;

		udest.h = 5;

		/// HACK we don't support infantry for flying units (see DrawGroundUnitHealthBars(..))
		if (un->isSelected() && udest.w >= 2 ) {
			ratio = un->getRatio();
			SDL_FillRect(screen, &udest, blackpix);
			udest.h -= 2;
			++udest.x;
			++udest.y;
			udest.w = (Uint16)((double)(udest.w-2) * ratio);
			SDL_FillRect(screen, &udest, ((ratio<=0.5)?(ratio<=0.25?redpix:yellowpix):greenpix));
		}
	}
}

/**
 * Render the flying objects (l2overlays).
 */
void GraphicsEngine::DrawL2Overlays()
{
	Uint32		i;
	SDL_Rect	udest;
	SDL_Rect	dest;
	Uint32		curpos, curdpos;
	Uint8		numshps;
	Uint32		*unitorstructshps = 0;
	Sint8		*uxoffsets = 0;
	Sint8		*uyoffsets = 0;



	for( i = 0; i < l2overlays.size(); i++)
	{
		curpos = l2overlays[i];

		dest.x = maparea.x-p::ccmap->getXTileScroll()+(curpos%p::ccmap->getWidth()-p::ccmap->getXScroll())*tilewidth;
		dest.y = maparea.y-p::ccmap->getYTileScroll()+(curpos/p::ccmap->getWidth()-p::ccmap->getYScroll())*tilewidth;

		numshps = p::uspool->getL2overlays(curpos, &unitorstructshps, &uxoffsets, &uyoffsets);


		for ( curdpos = 0; curdpos < numshps; curdpos++)
		{

#ifdef DEBUG_SHP_IMAGES
			Uint16 imagenumb	= unitorstructshps[curdpos]&0x7FF;
			Uint16 NumbImages	= imgcache->getNumbImages(unitorstructshps[curdpos]);
			std::string Name	= imgcache->getImageName(unitorstructshps[curdpos]);

			if (imagenumb > NumbImages)
				printf ("%s line %i: ERROR: Imagenumb > numbimages for image %s, numb images = %i\n", __FILE__, __LINE__, Name.c_str(), NumbImages);
#endif

			ImageCacheEntry& images = pc::imgcache->getImage(unitorstructshps[curdpos]);
			if (images.image != 0){
				udest.x = dest.x + uxoffsets[curdpos];
				udest.y = dest.y + uyoffsets[curdpos];
				udest.w = images.image->w;
				udest.h = images.image->h;
				SDL_UpperBlit(images.image, 0, screen, &udest);
			}
        }

		if (unitorstructshps != 0){
			delete[] unitorstructshps;
		}
		unitorstructshps = 0;

		if (uxoffsets != 0){
			delete[] uxoffsets;
		}
		uxoffsets = 0;

		if (uyoffsets != 0){
			delete[] uyoffsets;
		}
		uyoffsets = 0;
    }

}

/**
 * Render the fog of war
 */
void GraphicsEngine::DrawFogOfWar(SDL_Rect dest, SDL_Rect src, SDL_Rect udest)
{
	Uint32				curpos;
	Player				*lplayer = p::ccmap->getPlayerPool()->getLPlayer();
	int					i;
	Uint16				mapWidth;
	Uint16				mapHeight;

    vector<bool>* mapvis = lplayer->getMapVis();

    mapWidth = (maparea.w+p::ccmap->getXTileScroll()+tilewidth-1)/tilewidth;
    mapWidth = min(mapWidth, p::ccmap->getWidth());
    mapHeight = (maparea.h+p::ccmap->getYTileScroll()+tilewidth-1)/tilewidth;
    mapHeight = min(mapHeight, p::ccmap->getHeight());

	//	printf ("Start draw fog of war\n");

	// draw black on all non-visible squares (fog of war)
	dest.w = tilewidth;
	dest.h = tilewidth;
	curpos = p::ccmap->getScrollPos();
	dest.y = maparea.y-p::ccmap->getYTileScroll();
	// @todo This uses hardcoded values which it shouldn't do.
	// It should also cache the imagenums. If the imagenum is out
	// of reach NULL will be returned, we should check for this
	int shadowoffs;
	for( Sint16 ypos = 0; ypos < mapHeight; ypos++) {
		dest.x = maparea.x-p::ccmap->getXTileScroll();
		for( Sint16 xpos = 0; xpos < mapWidth; xpos++) {
			udest.x = dest.x;
			udest.y = dest.y;
			udest.w = tilewidth;
			udest.h = tilewidth;
			if (mapvis->at(curpos)) 
                        {
				src.x = 0;
				src.y = 0;
				src.w = tilewidth;
				src.h = tilewidth;
				clipToMaparea(&src, &udest);
				i = 0;

				// tile above this one is not visible
				if (curpos >= p::ccmap->getWidth() && !mapvis->at(curpos - p::ccmap->getWidth())) {
					i |= 1;
				}
				// tile next to this one (right) is not visible
				if (curpos%p::ccmap->getWidth() < (Uint16)(p::ccmap->getWidth()-1) && !mapvis->at(curpos + 1)) {
					i |= 2;
				}
				// tile below this one is not visible
				if (curpos < (Uint32)p::ccmap->getWidth()*(p::ccmap->getHeight()-1) && !mapvis->at(curpos + p::ccmap->getWidth())) {
					i |= 4;
				}
				// tile to the left is not visible
				if (curpos%p::ccmap->getWidth() > 0 && !mapvis->at(curpos-1)) {
					i |= 8;
				}

				// calc shadowoffs
				shadowoffs = 12*((curpos + curpos/p::ccmap->getWidth())%4);

				if (i != 0) {
					if (pc::Config.gamenum == GAME_TD) {
						if (i&1) {
							SDL_UpperBlit(p::ccmap->getShadowTile(0+shadowoffs), &src, screen, &udest);
						}
						if (i&2) {
							SDL_UpperBlit(p::ccmap->getShadowTile(1+shadowoffs), &src, screen, &udest);
						}
						if (i&4) {
							SDL_UpperBlit(p::ccmap->getShadowTile(3+shadowoffs), &src, screen, &udest);
						}
						if (i&8) {
							SDL_UpperBlit(p::ccmap->getShadowTile(5+shadowoffs), &src, screen, &udest);
						}
						if ((i&3) == 3) {
							SDL_UpperBlit(p::ccmap->getShadowTile(8+shadowoffs), &src, screen, &udest);
						}
						if ((i&6) == 6) {
							SDL_UpperBlit(p::ccmap->getShadowTile(9+shadowoffs), &src, screen, &udest);
						}
						if ((i&12) == 12) {
							SDL_UpperBlit(p::ccmap->getShadowTile(10+shadowoffs), &src, screen, &udest);
						}
						if ((i&9) == 9) {
							SDL_UpperBlit(p::ccmap->getShadowTile(11+shadowoffs), &src, screen, &udest);
						}
					}else if (pc::Config.gamenum == GAME_RA) {

						if (i == 12) { // Bottom Left
							SDL_UpperBlit(p::ccmap->getShadowTile(12/*+shadowoffs*/), &src, screen, &udest);
						}else if (i == 9) { // Top Left
							SDL_UpperBlit(p::ccmap->getShadowTile(9/*+shadowoffs*/), &src, screen, &udest);
						}else if (i == 6) { // Bottom Right
							SDL_UpperBlit(p::ccmap->getShadowTile(6/*+shadowoffs*/), &src, screen, &udest);
						}else if (i == 3) { // Top Right
							SDL_UpperBlit(p::ccmap->getShadowTile(3/*+shadowoffs*/), &src, screen, &udest);
						}else if (i == 8) { // Left (could also use 23
							SDL_UpperBlit(p::ccmap->getShadowTile(8), &src, screen, &udest);
						}else if (i == 4) { // Bottom
							SDL_UpperBlit(p::ccmap->getShadowTile(4), &src, screen, &udest);
						}else if (i == 2) { // Right
							SDL_UpperBlit(p::ccmap->getShadowTile(2), &src, screen, &udest);//7
						}else if (i == 1) { // Top
							SDL_UpperBlit(p::ccmap->getShadowTile(1), &src, screen, &udest);
						}
					}
				} else {

					if (curpos >= p::ccmap->getWidth() && curpos%p::ccmap->getWidth() < (Uint16)(p::ccmap->getWidth()-1) && !mapvis->at(curpos-p::ccmap->getWidth()+1)) {
						i |= 1;
					}
					if (curpos < (Uint32)p::ccmap->getWidth()*(p::ccmap->getHeight()-1) && curpos%p::ccmap->getWidth() < (Uint16)(p::ccmap->getWidth()-1) && !mapvis->at(curpos+p::ccmap->getWidth()+1)) {
						i |= 2;
					}
					if (curpos < (Uint32)p::ccmap->getWidth()*(p::ccmap->getHeight()-1) && curpos%p::ccmap->getWidth() > 0 && !mapvis->at(curpos+p::ccmap->getWidth()-1)) {
						i |= 4;
					}
					if (curpos >= p::ccmap->getWidth() && curpos%p::ccmap->getWidth() > 0 && !mapvis->at(curpos-p::ccmap->getWidth()-1)) {
						i |= 8;
					}

					if (pc::Config.gamenum == GAME_TD) {
						switch(i) {
							case 1:
								SDL_UpperBlit(p::ccmap->getShadowTile(8+shadowoffs), &src, screen, &udest);
								break;
							case 2:
								SDL_UpperBlit(p::ccmap->getShadowTile(9+shadowoffs), &src, screen, &udest);
								break;
							case 4:
								SDL_UpperBlit(p::ccmap->getShadowTile(10+shadowoffs), &src, screen, &udest);
								break;
							case 8:
								SDL_UpperBlit(p::ccmap->getShadowTile(11+shadowoffs), &src, screen, &udest);
								break;
							default:
								break;
						}
					}else if (pc::Config.gamenum == GAME_RA) {
						switch(i) {
							case 1: // sub right top corner
								SDL_UpperBlit(p::ccmap->getShadowTile(33/*+shadowoffs*/), &src, screen, &udest);
								break;
							case 2: // sub right bottum corner
								SDL_UpperBlit(p::ccmap->getShadowTile(34/*+shadowoffs*/), &src, screen, &udest);
								break;
							case 4: // sub left bottum corner
								SDL_UpperBlit(p::ccmap->getShadowTile(35/*+shadowoffs*/), &src, screen, &udest);
								break;
							case 8: // sub left top corner
								SDL_UpperBlit(p::ccmap->getShadowTile(32/*+shadowoffs*/), &src, screen, &udest);
								break;
							default:
								break;
						}
					}
				}
			} else {
				// draw a black square here
				SDL_FillRect(screen, &udest, blackpix);
			}
			dest.x += tilewidth;
			curpos++;
		}
		curpos += p::ccmap->getWidth()-mapWidth;
		dest.y += tilewidth;
	}
}

/**
 * Render the map
 */
void GraphicsEngine::DrawMap(SDL_Rect dest, SDL_Rect src, SDL_Rect udest)
{
	SDL_Surface	*bgimage = 0;
	//SDL_Rect	oldudest;
	Sint8		xmax;
	Sint8 		ymax;
	Sint8*		uxoffsets;
	Sint8*		uyoffsets;
	Uint32		curpos;
	Uint32		curdpos;
	Uint32		overlay;
	Uint32		terrain;
	Uint32		tiberium;
	Uint32		smudge;
	Uint32		TerrainOverlay;

	Sint16		txoff;
	Sint16		tyoff;
	Uint8		numshps;
	Uint32		*unitorstructshps;

	//std::vector<Uint16>	l2overlays;
	Sint16		xpos, ypos;
	int			i;
	Uint16		mapWidth;
	Uint16		mapHeight;

    mapWidth = (maparea.w+p::ccmap->getXTileScroll()+tilewidth-1)/tilewidth;
    mapWidth = min(mapWidth, p::ccmap->getWidth());
    mapHeight = (maparea.h+p::ccmap->getYTileScroll()+tilewidth-1)/tilewidth;
    mapHeight = min(mapHeight, p::ccmap->getHeight());

	xmax = min(p::ccmap->getWidth()-(p::ccmap->getXScroll()+mapWidth), 4);
	ymax = min(p::ccmap->getHeight()-(p::ccmap->getYScroll()+mapHeight), 4);

	curpos = p::ccmap->getScrollPos();

    dest.y = maparea.y - p::ccmap->getYTileScroll();

//	printf ("%s line %i: width = %i, height = %i\n", __FILE__, __LINE__, (tilewidth * (mapWidth+xmax)), (tileheight * (mapHeight+ymax+2)));
//	printf ("%s line %i: startx = %i, starty = %i, xmax = %i, ymax = %i\n", __FILE__, __LINE__, p::ccmap->getXScroll(), p::ccmap->getYScroll(), (mapWidth+xmax), (mapHeight+ymax+2));

	for (ypos = 0; ypos < mapHeight+ymax+2; ypos++)
	{
		dest.x = maparea.x - p::ccmap->getXTileScroll();
		for( xpos = 0; xpos < mapWidth+xmax; xpos++)
		{
			dest.w = tilewidth;
			dest.h = tilewidth;
			if (xpos < mapWidth && ypos < mapHeight) {
				udest.x = dest.x;
				udest.y = dest.y;
				udest.w = tilewidth;
				udest.h = tilewidth;
				src.x = 0;
				src.y = 0;
				src.w = tilewidth;
				src.h = tilewidth;
				clipToMaparea(&src, &udest);

				// Draw the map tiles
				bgimage = p::ccmap->getMapTile(curpos);
				if (bgimage != 0) {
					SDL_UpperBlit(bgimage, &src, screen, &udest);
				}

				// Draw smudges
				smudge = p::ccmap->getSmudge(curpos);
				if (smudge != 0) {
					ImageCacheEntry& images = pc::imgcache->getImage(smudge);
					if (images.image != 0){
						SDL_UpperBlit(images.image, &src, screen, &udest);
						SDL_UpperBlit(images.shadow, &src, screen, &udest);
					}
				}

				// Draw the tiberium
				tiberium = p::ccmap->getResourceFrame(curpos);
				if (tiberium != 0) {
					ImageCacheEntry& images = pc::imgcache->getImage(tiberium);
					if (images.image != 0){
						SDL_UpperBlit(images.image, &src, screen, &udest);
					}
                }

				// Draw the overlay
				overlay = p::ccmap->getOverlay(curpos);
				if (overlay != 0) {
					ImageCacheEntry& images = pc::imgcache->getImage(overlay);
					if (images.image != 0){
						SDL_UpperBlit(images.image, &src, screen, &udest);
						SDL_UpperBlit(images.shadow, &src, screen, &udest);
					}
				}
				if (p::uspool->hasL2overlay(curpos)) {
					l2overlays.push_back(curpos);
				}

				// Draw the terrain overlay (worn down ground under buildings)
				TerrainOverlay = p::ccmap->getTerrainOverlay(curpos);
				if (TerrainOverlay != 0 ) {
					ImageCacheEntry& images = pc::imgcache->getImage(TerrainOverlay);
					if (images.image != 0){
						SDL_UpperBlit(images.image, &src, screen, &udest);
					}
                }
			}

			if (ypos > 1) {
				dest.y -= (tilewidth<<1);
				curdpos = curpos - (p::ccmap->getWidth()<<1);
				terrain = p::ccmap->getTerrain(curdpos, &txoff, &tyoff);
				if (terrain != 0) {
					ImageCacheEntry& images = pc::imgcache->getImage(terrain);
					if (images.image != 0){
						src.x = 0;
						src.y = 0;
						src.w = images.image->w;
						src.h = images.image->h;
						udest.x = dest.x+txoff;
						udest.y = dest.y+tyoff;
						udest.w = images.image->w;
						udest.h = images.image->h;
						clipToMaparea(&src, &udest);

						SDL_UpperBlit(images.image, &src, screen, &udest);
						SDL_UpperBlit(images.shadow, &src, screen, &udest);
					}
				}

// Change 04-02-2007
#if 1

				// Handle drawing the structures
				numshps = p::uspool->getStructureNum(curdpos, &unitorstructshps, &uxoffsets, &uyoffsets);
				if (numshps > 0)
				{
					//printf("shpnum structur is <> 0\n");
					for( i  = 0; i < numshps; i++) {
						ImageCacheEntry& images = pc::imgcache->getImage(unitorstructshps[i]);
						if (images.image != 0){
							src.x = 0;
							src.y = 0;
							src.w = images.image->w;
							src.h = images.image->h;
							udest.x = dest.x+uxoffsets[i];
							udest.y = dest.y+uyoffsets[i];
							udest.w = images.image->w;
							udest.h = images.image->h;
							clipToMaparea(&src, &udest);
							SDL_UpperBlit(images.image, &src, screen, &udest);
							SDL_UpperBlit(images.shadow, &src, screen, &udest);
						}
					}
					if (unitorstructshps != 0){
						delete[] unitorstructshps;
					}
					unitorstructshps = 0;

					if (uxoffsets != 0){
						delete[] uxoffsets;
					}
					uxoffsets = 0;

					if (uyoffsets != 0){
						delete[] uyoffsets;
					}
					uyoffsets = 0;
				}

				//DrawHealthBars(dest, udest, curdpos, true);
				DrawStructureHealthBars(dest, udest, curdpos);



				dest.y += (tilewidth<<1);
			}
			dest.x += tilewidth;
			curpos++;
		}
		curpos += p::ccmap->getWidth()-mapWidth-xmax;
		dest.y += tilewidth;
	}

	curpos = p::ccmap->getScrollPos();

    dest.y = maparea.y-p::ccmap->getYTileScroll();

	// Draw units and strucutres in a new loop so we don't draw map tiles over structures :)
	for( ypos = 0; ypos < mapHeight+ymax+2; ypos++) {
		dest.x = maparea.x - p::ccmap->getXTileScroll();
		for( xpos = 0; xpos < mapWidth+xmax; xpos++) {
			dest.w = tilewidth;
			dest.h = tilewidth;
			if (xpos < mapWidth && ypos < mapHeight) {
				udest.x = dest.x;
				udest.y = dest.y;

			}



			if (ypos > 1) {
				dest.y -= (tilewidth<<1);
				curdpos = curpos-(p::ccmap->getWidth()<<1);

#else

				// Handle drawing the structures
				numshps = p::uspool->getStructureNum(curdpos, &unitorstructshps, &uxoffsets, &uyoffsets);
				if (numshps > 0) {
					for( i  = 0; i < numshps; i++) {
						ImageCacheEntry& images = imgcache->getImage(unitorstructshps[i]);
						if (images.image != NULL){
							src.x = 0;
							src.y = 0;
							src.w = images.image->w;
							src.h = images.image->h;
							udest.x = dest.x+uxoffsets[i];
							udest.y = dest.y+uyoffsets[i];
							udest.w = images.image->w;
							udest.h = images.image->h;
							clipToMaparea(&src, &udest);
							SDL_BlitSurface(images.image, &src, screen, &udest);
							SDL_BlitSurface(images.shadow, &src, screen, &udest);
						}
					}
					if (unitorstructshps != NULL)
						delete[] unitorstructshps;
					unitorstructshps = NULL;

					if (uxoffsets != NULL)
						delete[] uxoffsets;
					uxoffsets = NULL;

					if (uyoffsets != NULL)
						delete[] uyoffsets;
					uyoffsets = NULL;
				}
				else
				{
					printf("numshps is == 0 for structures\n");
				}
#endif

				// Handle drawing the units
				numshps = p::uspool->getUnitNum(curdpos, &unitorstructshps, &uxoffsets, &uyoffsets);
				if (numshps > 0) {
					for( i  = 0; i < numshps; i++) {
						ImageCacheEntry& images = pc::imgcache->getImage(unitorstructshps[i]);
						if (images.image != 0){
							src.x = 0;
							src.y = 0;
							src.w = images.image->w;
							src.h = images.image->h;
							udest.x = dest.x+uxoffsets[i];
							udest.y = dest.y+uyoffsets[i];
							udest.w = images.image->w;
							udest.h = images.image->h;
							clipToMaparea(&src, &udest);
							SDL_UpperBlit(images.image, &src, screen, &udest);
							SDL_UpperBlit(images.shadow, &src, screen, &udest);
						}
					}
					if (unitorstructshps != 0)
						delete[] unitorstructshps;
					unitorstructshps = 0;

					if (uxoffsets != 0)
						delete[] uxoffsets;
					uxoffsets = 0;

					if (uyoffsets != 0)
						delete[] uyoffsets;
					uyoffsets = 0;
				}

				DrawGroundUnitHealthBars(dest, udest, curdpos);

				// Handle drawing the air units
				numshps = p::uspool->getFlyingUnitNum(curdpos, &unitorstructshps, &uxoffsets, &uyoffsets);
				if (numshps > 0) {
					//printf ("%s line %i: Found air based units to draw\n", __FILE__, __LINE__);
					for( i  = 0; i < numshps; i++) {
						ImageCacheEntry& images = pc::imgcache->getImage(unitorstructshps[i]);
						if (images.image != 0){
							src.x = 0;
							src.y = 0;
							src.w = images.image->w;
							src.h = images.image->h;
							udest.x = dest.x+uxoffsets[i];
							udest.y = dest.y+uyoffsets[i];
							udest.w = images.image->w;
							udest.h = images.image->h;
							clipToMaparea(&src, &udest);
							SDL_UpperBlit(images.image, &src, screen, &udest);
							SDL_UpperBlit(images.shadow, &src, screen, &udest);
						}
					}
					if (unitorstructshps != 0)
						delete[] unitorstructshps;
					unitorstructshps = 0;

					if (uxoffsets != 0)
						delete[] uxoffsets;
					uxoffsets = 0;

					if (uyoffsets != 0)
						delete[] uyoffsets;
					uyoffsets = 0;
				}


				DrawFlyingUnitHealthBars(dest, udest, curdpos);

				dest.y += (tilewidth<<1);
			}
			dest.x += tilewidth;
			curpos++;
		}
		curpos += p::ccmap->getWidth() - mapWidth - xmax;
		dest.y += tilewidth;
    }

	// Draw repaire icon
	DrawRepairing();

	// Draw smoke on vehicules
	DrawVehicleSmoke();

	// Test
	DrawStructures();

	// Handle drawing the BOMB (C4) icon
	DrawBombing();
}

/**
 */
void GraphicsEngine::clipToMaparea(SDL_Rect *dest)
{
    if (dest->x < maparea.x) {
        dest->w -= maparea.x-dest->x;
        dest->x = maparea.x;
    }
    if (dest->y < maparea.y) {
        dest->h = maparea.y - dest->y;
        dest->y = maparea.y;
    }
}

/**
 */
void GraphicsEngine::clipToMaparea(SDL_Rect *src, SDL_Rect *dest)
{
    if (dest->x < maparea.x) {
        src->x = maparea.x - dest->x;
        src->w -= src->x;
        dest->x = maparea.x;
        dest->w = src->w;
    }
    if (dest->y < maparea.y) {
        src->y = maparea.y - dest->y;
        src->h -= src->y;
        dest->y = maparea.y;
        dest->h = src->h;
    }
}

/**
 * Draw the victory or the defeat label.
 */
void GraphicsEngine::drawMissionLabel()
{
    // Get the local player
    Player* lPlayer = p::ccmap->getPlayerPool()->getLPlayer();

    //printf("x=%d y=%d w=%d h=%d\n", maparea.x, maparea.y, maparea.w, maparea.h);
    //printf("resX=%d W=%d resY=%d  H=%d\n",
    //maparea.x + (maparea.w + defeatLabel->getWidth())/2, defeatLabel->getWidth(),
    //maparea.y + (maparea.h + defeatLabel->getHeight())/2, defeatLabel->getHeight());

    // If the local player is winning
    if (lPlayer->isVictorious() == true)
    {
        Sint16 resX = maparea.x + (maparea.w - defeatLabel->getWidth())/2;

        // If the sidebar is visible
        if (pc::sidebar->getVisible())
        {
            // @todo get the real width of the sidebar
            // resX -= pc::sidebar->getSidebarImage()->w;
            resX -=30;
        }
        
		victoryLabel->Draw(screen, // Draw at screen
							resX,
							maparea.y + (maparea.h + victoryLabel->getHeight())/2
							);
    }
    
    // If the local player is lossing
    if (lPlayer->isDefeated() == true) 
    {
        Sint16 resX = maparea.x + (maparea.w - defeatLabel->getWidth())/2;

		// If the sidebar is visible
		if (pc::sidebar->getVisible())
		{
			// @todo get the real width of the sidebar
			// resX -= pc::sidebar->getSidebarImage()->w;
			resX -=30;
		}

		defeatLabel->Draw(screen, // Draw at screen
							resX,
							maparea.y + (maparea.h - defeatLabel->getHeight())/2
							);
	}
}

/**
 * Draw the sidebar and tabs.
 */
void GraphicsEngine::drawSidebar()
{
    SDL_Rect dest;
    SDL_Rect *tabpos;


    //    static Uint32 blackpix = SDL_MapRGB(screen->format, 0, 0, 0);
    Uint32 tick;
    Uint16 framerate;
    char mtext[128];

    Player* lplayer = p::ccmap->getPlayerPool()->getLPlayer();

    tabpos = pc::sidebar->getTabLocation();

    if (clearBack) {
        clearBuffer();
        clearBack = false;
    }

    if (pc::sidebar->getVisible()) {
        if (pc::sidebar->getVisChanged()) {
            clearBuffer();
            clearBack = true;

            maparea.h = height-tabpos->h;
            maparea.y = tabpos->h;
            maparea.w = width-tabpos->w;
            maparea.x = 0;
            if (maparea.w > p::ccmap->getWidth()*tilewidth) {
                maparea.w = p::ccmap->getWidth()*tilewidth;
                maparea.x = (width-tabpos->w-maparea.w)>>1;
            }
            if (maparea.h > p::ccmap->getHeight()*tilewidth) {
                maparea.h = p::ccmap->getHeight()*tilewidth;
                maparea.y = tabpos->h+((height-tabpos->h-maparea.h)>>1);
            }
            p::ccmap->setMaxScroll(maparea.w/tilewidth, maparea.h/tilewidth,
                              maparea.w%tilewidth, maparea.h%tilewidth, tilewidth);
        }
        dest.x = width - tabpos->w;
        dest.w = tabpos->w;
        dest.y = tabpos->h;
        dest.h = height - tabpos->h;

        //      SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0xa0, 0xa0, 0xa0));
        SDL_UpperBlit(pc::sidebar->getSidebarImage(dest), 0, screen, &dest);
    } else if (pc::sidebar->getVisChanged()) {
        clearBuffer();
        clearBack = true;

        maparea.h = height-tabpos->h;
        maparea.y = tabpos->h;
        maparea.w = width;
        maparea.x = 0;
        if (maparea.w > p::ccmap->getWidth()*tilewidth) {
            maparea.w = p::ccmap->getWidth()*tilewidth;
            maparea.x = (width-maparea.w)>>1;
        }
        if (maparea.h > p::ccmap->getHeight()*tilewidth) {
            maparea.h = p::ccmap->getHeight()*tilewidth;
            maparea.y = tabpos->h+((height-tabpos->h-maparea.h)>>1);
        }
        p::ccmap->setMaxScroll(maparea.w/tilewidth, maparea.h/tilewidth,
                          maparea.w%tilewidth, maparea.h%tilewidth, tilewidth);
    }

    tabpos->y = 0;//maparea.y-tabpos->h;
    tabpos->x = 0;

	if (OptionsLabel == 0){
		OptionsLabel = new RA_Label();
		OptionsLabel->setColor(0xdf, 0xdf, 0xdf);
		OptionsLabel->SetFont("grad6fnt.fnt");
		OptionsLabel->UseAntiAliasing(false);
		OptionsLabel->setText("Options");
	}

	// Draw the left tab
	SDL_Surface *TabImage = pc::sidebar->getTabImage();

    dest.y = tabpos->y + tabpos->h - OptionsLabel->getHeight();
    dest.x = ( (tabpos->w - OptionsLabel->getWidth()) >>1);//+tabpos->x;


	// Create the Options surface
	if (OptionsSurface == 0){
		OptionsSurface = SDL_CreateRGBSurface(0x00000000	/* Surface is in system memory */|0x00001000	/* Blit uses a source color key */, TabImage->w, TabImage->h, 16, 0, 0, 0, 0);

		dest.x = 0;
		dest.y = 0;
		dest.w = OptionsSurface->w;
		dest.h = OptionsSurface->h;

		SDL_UpperBlit( TabImage, 0, OptionsSurface, &dest);

	    dest.x = ((TabImage->w - OptionsLabel->getWidth()) >>1);
	    dest.y = tabpos->y + tabpos->h - OptionsLabel->getHeight();

		OptionsLabel->Draw(OptionsSurface, dest.x, dest.y+1);

	} else {
		SDL_UpperBlit(OptionsSurface, 0, screen, tabpos);
	}


    tabpos->x = width - tabpos->w;

	if (MoneyLabel == 0){
		MoneyLabel = new RA_Label();
		MoneyLabel->setColor(0xff, 0xff, 0xff);
		MoneyLabel->SetFont("grad6fnt.fnt");
		MoneyLabel->UseAntiAliasing(false);
	}

	// Draw the right tab
    sprintf(mtext, "%d", lplayer->getMoney());
    dest.x = 0;//((tabpos->w-pc::sidebar->getFont()->calcTextWidth(mtext))>>1)+tabpos->x;

	// Create the MoneySurface surface
	if (MoneySurface == 0){
		MoneySurface = SDL_CreateRGBSurface(0x00000000	/* Surface is in system memory */|0x00001000	/* Blit uses a source color key */, TabImage->w, TabImage->h, 16, 0, 0, 0, 0);
	}else{
		dest.x = 0;
		dest.y = 0;
		dest.w = MoneySurface->w;
		dest.h = MoneySurface->h;

		MoneyLabel->setText(mtext);

		SDL_UpperBlit( TabImage, 0, MoneySurface, &dest);

		dest.x = ((tabpos->w - MoneyLabel->getWidth())>>1);
		dest.y = tabpos->y + tabpos->h - MoneyLabel->getHeight();

		MoneyLabel->Draw(MoneySurface, dest.x, dest.y+1);

		SDL_UpperBlit(MoneySurface, 0, screen, tabpos);
	}


    tick = SDL_GetTicks();
    frames++;
    framerate = (frames*1000)/(tick - firstframe+1);
    sprintf(mtext, "%d fps", framerate);
    SDL_Rect clearCoor;
    clearCoor.w = width-2*tabpos->w;/*100;*/
    clearCoor.h = tabpos->h+4;/*20;*/
    clearCoor.x = 0; //tabpos->w;/*width >>1;*/
    clearCoor.y = 0; //dest.y;

	// Create the FrameRate surface
	if (FrameRateSurface == 0){
		FrameRateSurface = SDL_CreateRGBSurface(0x00000000	/* Surface is in system memory */|0x00001000	/* Blit uses a source color key */, clearCoor.w, clearCoor.h, 16, 0, 0, 0, 0);
	}else{

		if (FrameRateLabel == 0){
			FrameRateLabel = new RA_Label();
			//FrameRateLabel->setcolor (0xbb, 0xbb, 0xbb);
			FrameRateLabel->setColor(0xff, 0xff, 0xff);
			FrameRateLabel->SetFont("scorefnt.fnt");
			//FrameRateLabel->SetFont("12metfnt.fnt");
			FrameRateLabel->UseAntiAliasing(false);
		}

		FrameRateLabel->setText(mtext);

		SDL_FillRect(FrameRateSurface, &clearCoor, SDL_MapRGB(screen->format, 0, 0, 0));
		FrameRateLabel->Draw(FrameRateSurface, 0, 0);

		clearCoor.x = tabpos->w;
		clearCoor.y = tabpos->y + tabpos->h - FrameRateLabel->getHeight();


		SDL_UpperBlit(FrameRateSurface, 0, screen, &clearCoor);
	}

    if (frames == 1000) {
        frames = 1;
        firstframe = tick;
    }
}

/**
 * Draw a line on the screen
 */
void GraphicsEngine::drawLine(Sint16 startx, Sint16 starty,
                              Sint16 stopx, Sint16 stopy, Uint16 width, Uint32 colour)
{
    float xmod, ymod, length, xpos, ypos;
    int i, len;
    SDL_Rect dest;
    xmod = static_cast<float>(stopx-startx);
    ymod = static_cast<float>(stopy-starty);

    length = sqrt(xmod*xmod+ymod*ymod);
    xmod /= length;
    ymod /= length;

    len = static_cast<int>(length+0.5f);
    xpos = static_cast<float>(startx-(width>>1));
    ypos = static_cast<float>(starty-(width>>1));
    for(i = 0; i < len; i++) {
        dest.x = (Sint16)xpos;
        dest.y = (Sint16)ypos;
        dest.w = width;
        dest.h = width;
        SDL_FillRect(screen, &dest, colour);
        xpos += xmod;
        ypos += ymod;
    }
}

SDL_Surface *GraphicsEngine::get_SDL_ScreenSurface()
{
	return screen;
}

Uint16 GraphicsEngine::getWidth()
{
	return width;
}

SDL_Rect* GraphicsEngine::getMapArea()
{
	return &maparea;
}

Uint16 GraphicsEngine::getHeight()
{
	return height;
}

void GraphicsEngine::swapMiniMapZoomFactor()
{
	this->mz = ( this->mz == &minizoom.max ? &minizoom.normal : &minizoom.max );
}
