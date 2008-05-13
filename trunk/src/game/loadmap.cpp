#include "include/ccmap.h"

#include <cctype>
#include <stdexcept>

#include "Game.h"
#include "misc/Compression.h"
#include "include/config.h"
#include "include/fcnc_endian.h"
#include "misc/INIFile.h"
#include "video/ImageCache.h"
#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "include/UnitAndStructurePool.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "GameMode.h"
#include "include/triggers.h"
#include "MissionData.h"
#include "video/ImageNotFound.h"
#include "video/SHPImage.h"
#include "video/TemplateImage.h"
#include "CellTrigger.h"
#include "misc/KeyNotFound.h"
#include "misc/INIFile.h"
#include "LoadMapError.h"

using std::map;
using std::runtime_error;
using std::string;
using INI::KeyNotFound;

namespace pc
{
extern ConfigType Config;
}
extern Logger * logger;

/** 
 * Loads the maps ini file containing info on dimensions, units, trees
 * and so on.
 */
void CnCMap::loadIni()
{
	std::ostringstream TempString;
	INIFile *inifile;
	string tmpName;
	
	// Get the ini file of the map
	tmpName = missionData->mapname;
	tmpName += ".INI";
	
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
	if (inifile->readInt("basic", "newiniformat", 0) != 0)
	{
		logger->error("Red Alert maps not fully supported yet\n");
	}

	// Read simple ini section
	simpleSections(inifile);

	// Try to create UnitAndStructurePool
	try
	{
		p::uspool = new UnitAndStructurePool();
		logger->note("UnitAndStructurePool created\n");
	}
	catch (...)
	{
		// Log it
		logger->error("Error in creating UnitAndStructurePool (p::uspool)");
		// Throw error
		throw LoadMapError("Error in creating UnitAndStructurePool (p::uspool)");
	}

	if (missionData->player != NULL)
	{
		printf ("%s line %i: Player = %s\n", __FILE__, __LINE__, missionData->player);
	}

	if (gamemode == GAME_MODE_SINGLE_PLAYER)
	{
		p::ppool->setLPlayer(missionData->player);
		p::ppool->getLPlayer()->setMultiColour(pc::Config.side_colour.c_str());
		//			printf ("%s line %i: Set local player\n", __FILE__, __LINE__);
	}

	terraintypes.resize(width*height, 0);
	resourcematrix.resize(width*height, 0);

	advancedSections(inifile);

	if (maptype == GAME_RA)
	{
		unMapPack(inifile);
	}

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

	try
	{
		pips = new SHPImage("hpips.shp",mapscaleq);
	}
	catch(ImageNotFound&)
	{
		try
		{
			pips = new SHPImage("pips.shp", mapscaleq);
		}
		catch(ImageNotFound&)
		{
			// Logg it
			logger->error("Unable to load the pips graphics!\n");
			// Throw error
			throw LoadMapError("Unable to load the pips graphics!");
		}
	}
	pipsnum = pc::imagepool->size()<<16;
	pc::imagepool->push_back(pips);
	if (maptype == GAME_RA)
	{
		try
		{
			char moveflsh[13] = "moveflsh.";
			strncat( moveflsh, missionData->theater, 3 );
			moveflash = new SHPImage(moveflsh,mapscaleq);
		}
		catch (ImageNotFound&)
		{
			logger->error("Unable to load the movement acknowledgement pulse graphic\n");
			throw LoadMapError("Unable to load the movement acknowledgement pulse graphic");
		}
	}
	else
	{
		try
		{
			moveflash = new SHPImage("moveflsh.shp",mapscaleq);
		}
		catch (ImageNotFound&)
		{
			logger->error("Unable to load the movement acknowledgement pulse graphic\n");
			throw LoadMapError("Unable to load the movement acknowledgement pulse graphic");
		}
	}
	flashnum = pc::imagepool->size()<<16;
	pc::imagepool->push_back(moveflash);

}

/** 
 * Function to load all vars in the simple sections of the inifile
 * 
 * TODO add key name in the log (to throw LoadMapError
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

		// Try to read the build level (def = 25)
		missionData->buildlevel = inifile->readInt("Basic", "BUILDLEVEL", 25);
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
Uint8 UnitActionToNr(const std::string action)
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
	char teamname[255];
	int facing, health, subpos;
	int linenum, smudgenum, tmpval;
	Uint16 tx = 0;
	Uint16 ty = 0;
	Uint16 xsize = 0;
	Uint16 ysize = 0;
	Uint16 tmp2 = 0;
	std::map<std::string, Uint32> imagelist;
	std::map<std::string, Uint32>::iterator imgpos;
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

	// set player start locations
	for (int k = 0; k < 8; k++)
	{
		p::ppool->setPlayerStarts(k, getWaypoint(k));
	}
	INIFile *arts= NULL;
	arts = GetConfig("art.ini");

	// load the shadowimages
	try
	{
		image = new SHPImage("SHADOW.SHP", mapscaleq);
		numShadowImg = image->getNumImg();
		shadowimages.resize(numShadowImg);
		for( i = 0; i < 48; i++ )
		{
			image->getImageAsAlpha(i, &shadowimages[i]);
		}
		if (image != NULL)
		delete image;
		image = NULL;

	}
	catch(ImageNotFound&)
	{
		logger->warning("Unable to load \"shadow.shp\"\n");
		numShadowImg = 0;
	}

	// load the smudge marks and the tiberium to the imagepool
	if (strncasecmp(missionData->theater, "INT", 3) != 0)
	{
		string sname;
		if (maptype == GAME_TD)
		{
			sname = "TI1";
		}
		else if (maptype == GAME_RA)
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
			image = new SHPImage(sname.c_str(), mapscaleq);
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
				image = new SHPImage(shpname, mapscaleq);
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
				image = new SHPImage(shpname, mapscaleq);
			}
			catch (ImageNotFound&)
			{	continue;}
			pc::imagepool->push_back(image);
		}
	}

	overlaymatrix.resize(width*height, 0);

	try
	{
		for( keynum = 0;;keynum++ )
		{
			bool bad = false;
			key = inifile->readKeyValue("TERRAIN", keynum);
			// , is the char which separate terraintype from action.

			if( sscanf(key->first.c_str(), "%d", &linenum) == 1 &&
					sscanf(key->second.c_str(), "%[^,],", shpname) == 1 )
			{
				/* Set the next entry in the terrain vector to the correct values.
				 * the map-array and shp files vill be set later */
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

				/* search the map for the image */
				imgpos = imagelist.find(shpname);

				/* set up the overlay matrix and load some shps */
				if( imgpos != imagelist.end() )
				{
					/* this tile already has a number */
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
						image = new SHPImage(shpname, mapscaleq);
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

	if (maptype == GAME_RA)
	{
		unOverlayPack(inifile);
	}
	else
	{
		loadOverlay(inifile);
	}

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

	// Try to read techs levels
	try {
		p::uspool->preloadUnitAndStructures(missionData->buildlevel);
	} catch (...) {
		logger->debug("Error during preloadUnitAndStructures() in [advancedSections()]");
	}
	
	// Try to read techs levels
	try {
		p::uspool->generateProductionGroups();
	} catch (...) {
		logger->debug("Error during generateProductionGroups() in [advancedSections()]");
	}

	//
	// structures
	//
	// If their are a section called "STRUCTURE"
	if (inifile->isSection(string("STRUCTURE")) == true)
	{
		try
		{
			for( keynum = 0;;keynum++ )
			{
				if (maptype == GAME_RA)
				{
					key = inifile->readKeyValue("STRUCTURES", keynum);
					/* , is the char which separate terraintype from action. */
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
						p::uspool->createStructure(type, linenum, p::ppool->getPlayerNum(owner),
								health, facing, false, trigger);
						//                    printf ("%s line %i: createStructure STRUCTURE %s, trigger = %s\n", __FILE__, __LINE__, type, trigger);
					}
				}
				else if (maptype == GAME_TD)
				{
					key = inifile->readKeyValue("STRUCTURES", keynum);
					/* , is the char which separate terraintype from action. */
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
						p::uspool->createStructure(type, linenum, p::ppool->getPlayerNum(owner),
								health, facing, false, trigger);
					}
				}
			}
		}
		catch(...)
		{}
	}

	
	
	//
	// units
	//
	// If their are a section called "UNITS"
	if (inifile->isSection(string("UNITS")) == true)
	{
		try
		{
			for( keynum = 0;;keynum++ )
			{
				key = inifile->readKeyValue("UNITS", keynum);
				/* , is the char which separate terraintype from action. */
				if( sscanf(key->first.c_str(), "%d", &tmpval) == 1 &&
						sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%[^,],%s", owner, type,
								&health, &linenum, &facing, action, trigger ) == 7 )
				{
					translateCoord(linenum, &tx, &ty);
					facing = min(31,facing>>3);
					if( tx < x || ty < y || tx> x+width || ty> height+y )
					{
						continue;
					}
					linenum = (ty-y)*width + tx - x;
					p::uspool->createUnit(type, linenum, 5, p::ppool->getPlayerNum(owner), health, facing, UnitActionToNr(action), trigger);

					//printf ("%s line %i: createUnit UNIT %s, trigger = %s\n", __FILE__, __LINE__, key->first.c_str(), trigger);
				}
			}
		}
		catch(...)
		{}
	}

	/*infantry*/
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
						logger->error("Error in create TRIGGER in the map\n");
					}
				}
			}
		} 
		catch (...)
		{}
	}

	/*triggers*/
	RA_Tiggers triggers;
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
				if (sscanf(key->second.c_str(), "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i", &triggers.repeatable, &triggers.country, &triggers.activate, &triggers.actions, &triggers.trigger1.event, &triggers.trigger1.param1, &triggers.trigger1.param2, &triggers.trigger2.event, &triggers.trigger2.param1, &triggers.trigger2.param2, &triggers.action1.Action, &triggers.action1.param1, &triggers.action1.param2, &triggers.action1.param3, &triggers.action2.Action, &triggers.action2.param1, &triggers.action2.param2, &triggers.action2.param3) == 18 )
				{
#if 0
					printf ("%s line %i: Read trigger:\n", __FILE__, __LINE__);
					PrintTrigger ( triggers );
					printf ("\n\n\n");
#endif
					triggers.hasexecuted = false;
					RaTriggers.push_back(triggers);

					//PrintTrigger ( triggers );
				} else {
					logger->warning("error in reading a trigger");
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

	/*Teamtypes*/
	RA_Teamtype team;
	RA_TeamUnits unit;
	int pos;
	try
	{
		for( keynum = 0;;keynum++ )
		{
			if (maptype == GAME_RA)
			{
				team.Units.clear();
				key = inifile->readKeyValue("Teamtypes", keynum);
				team.tname = key->first;
				sscanf(key->second.c_str(), "%i, %i, %i, %i, %i, %i, %i, %i, %[^,]", &team.country, &team.props, &team.unknown1, &team.unknown2, &team.maxteams, &team.waypoint, &team.trigger, &team.numb_teamtypes, teamname );
				pos = key->second.find (teamname,0);
				std::string temp = key->second.substr (pos, key->second.size());
				for (int j = 0; j < team.numb_teamtypes; j++)
				{
					sscanf(temp.c_str(), "%[^:]:%i ,", teamname, &unit.numb);
					unit.tname = teamname;
					//                    printf ("%s line %i: Team = %s, push back unit: %s, %i\n", __FILE__, __LINE__, team.tname.c_str(), unit.tname.c_str(), unit.numb);
					team.Units.push_back (unit);
					pos = temp.find (",", 0);
					temp = temp.substr(pos+1, temp.size());
					//printf ("%s line %i: New string: %s\n", __FILE__, __LINE__, temp.c_str());
				}
				//We should start reading the commands from std::string temp here :)

				RaTeamtypes.push_back (team);
			}
		}
	}
	catch(...)
	{}

	/*BASE*/
	try
	{
		for( keynum = 0;;keynum++ )
		{
			if (maptype == GAME_RA)
			{
				key = inifile->readKeyValue("Base", keynum);
				/* is the char which separate terraintype from action. */
				//                printf ("%s line %i: Base text 1: %s\n", __FILE__, __LINE__, key->first.c_str());
				//                printf ("%s line %i: Base text 2: %s\n", __FILE__, __LINE__, key->second.c_str());
				//printf ("%s line %i: Base text: %s\n", __FILE__, __LINE__, key->third.c_str());
				//                if (sscanf(key->second.c_str(), "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i", &triggers.repeatable, &triggers.country, &triggers.activate, &triggers.actions, &triggers.trigger1.event,  &triggers.trigger1.param1, &triggers.trigger1.param2, &triggers.trigger2.event,  &triggers.trigger2.param1, &triggers.trigger2.param2,      &triggers.action1.Action, &triggers.action1.param1, &triggers.action1.param2, &triggers.action1.param3,      &triggers.action2.Action, &triggers.action2.param1, &triggers.action2.param2, &triggers.action2.param3) == 18  ) {
			}
			else if (maptype == GAME_TD)
			{
				//key = inifile->readUnsortedKeyValue("TRIGERS", keynum);
				/* is the char which separate terraintype from action. */
				//logger->warning ("%s line %i: Trigger1 text: %s\n", __FILE__, __LINE__, key->first.c_str());
				//              logger->warning ("%s line %i: Trigger2 text: %s\n", __FILE__, __LINE__, key->second.c_str());
			}
		}
	}
	catch(...)
	{}

}

struct tiledata
{
	Uint32 image;
	Uint8 type;
};

/** 
 * Bin loading routines
 */
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
			/* Read template and tile */
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
		/* Skip til the end of the line and the onwards to the
		 * beginning of usefull data on the next line
		 */
		binfile->seekCur( 2*(64-width));
	}
	VFSUtils::VFS_Close(binfile);
	parseBin(mapdata);
	if (mapdata != NULL)
		delete[] mapdata;
	mapdata = NULL;
}

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

	Compression::dec_base64(mapdata1, mapdata2, strlen(((char*)mapdata1)));

	/* decode the format80 coded data (6 chunks) */
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
	if (mapdata2 != NULL)
		delete[] mapdata2;
	mapdata2 = NULL;

	/* 128*128 16-bit template number followed by 128*128 8-bit tile numbers */
	bindata = new TileList[width*height];
	tmpval = y*128+x;
	curpos = 0;
	for (ytile = 0; ytile < height; ytile++)
	{
		for (xtile = 0; xtile < width; xtile++)
		{
			/* Read template and tile */
			bindata[curpos].templateNum = ((Uint16 *)mapdata1)[tmpval];
			bindata[curpos].tileNum = mapdata1[tmpval+128*128*2];
			curpos++;
			tmpval++;
			/*  printf("tile %d, %d\n", bindata[curpos-1].templateNum,
			 bindata[curpos-1].tileNum); */
		}
		/* Skip until the end of the line and the onwards to the
		 * beginning of usefull data on the next line
		 */
		tmpval += (128-width);
	}
	if (mapdata1 != NULL)
		delete[] mapdata1;
	mapdata1 = NULL;
	parseBin(bindata);

	if (bindata != NULL)
		delete[] bindata;
	bindata = NULL;
}

void CnCMap::parseBin(TileList* bindata)
{
	Uint32 index;

	Uint16 templ;
	Uint8 tile;
	Uint32 tileidx;
	int xtile, ytile;
	INIFile *templini;

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

	/* Load the templates.ini */
	templini = GetConfig("templates.ini");

	index = 0;
	for (ytile = 0; ytile < height; ytile++)
	{
		for (xtile = 0; xtile < width; xtile++)
		{
			/* Read template and tile */
			templ = bindata[index].templateNum;
			tile = bindata[index].tileNum;
			index++;
			/* Template 0xff is an empty tile */
			if (templ == ((maptype == GAME_RA) ? 0xffff : 0xff))
			{
				templ = 0;
				tile = 0;
			}

			/* Code sugested by Olaf van der Spek to cause all tiles in template
			 * 0 and 2 to be used */
			if (templ == 0)
				tile = xtile&3 | (ytile&3 << 2);
			else if (templ == 2)
				tile = xtile&1 | (ytile&1 << 1);

			imgpos = tilelist.find(templ<<8 | tile);

			/* set up the tile matrix and load some tiles */
			if (imgpos != tilelist.end() )
			{
				/* this tile already has a number */
				tileidx = imgpos->second.image;
				tiletype = imgpos->second.type;
			}
			else
			{
				// a new tile
				tileimg = loadTile(templini, templ, tile, &tiletype);

				if (tileimg == NULL)
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
			if (terraintypes[width*ytile+xtile] == 0)
				terraintypes[width*ytile+xtile] = tiletype;

		}
	}
}

/////// Overlay loading routines

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

				parseOverlay(linenum, key->second);
			}
		}
	}
	catch(...)
	{}
}

const char
		* RAOverlayNames[] =
		{ "SBAG", "CYCL", "BRIK", "FENC", "WOOD", "GOLD01", "GOLD02", "GOLD03",
				"GOLD04", "GEM01", "GEM02", "GEM03", "GEM04", "V12", "V13",
				"V14", "V15", "V16", "V17", "V18", "FPLS", "WCRATE", "SCRATE",
				"FENC", "SBAG" };

void CnCMap::unOverlayPack(INIFile *inifile)
{
	Uint32 curpos, tilepos;
	Uint8 xtile, ytile;
	Uint32 keynum;
	INIKey key;
	Uint8 mapdata[16384]; // 16k
	Uint8 temp[16384];

	// read packed data into array
	mapdata[0] = 0;
	try
	{
		for (keynum = 1;;++keynum)
		{
			key = inifile->readIndexedKeyValue("OVERLAYPACK", keynum);
			strcat(((char*)mapdata), key->second.c_str());
		}
	}
	catch(...)
	{}

	Compression::dec_base64(mapdata, temp, strlen(((char*)mapdata)));

	/* decode the format80 coded data (2 chunks) */
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
}

void CnCMap::parseOverlay(const Uint32& linenum, const string& name)
{
	Uint8 type, frame;
	Uint16 res;

	if (name == "BRIK" || name == "SBAG" || name == "FENC" || name == "WOOD"
			|| name == "CYCL" || name == "BARB")
	{
		// Walls are structures.
		p::uspool->createStructure(name.c_str(), linenum,
				p::ppool->getPlayerNum("NEUTRAL"), 256, 0, false);
		return;
	}

	string shpname;
	shpname = name + '.' + string(missionData->theater, 3);
	try
	{
		/* Remember: imagecache's indexing format is different
		 * (imagepool index << 16) | frame */
		frame = pc::imgcache->loadImage(shpname.c_str()) >> 16;
	}
	catch(ImageNotFound&)
	{
		shpname = name + ".SHP";
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

	/// @TODO Generic resources?
	if (strncasecmp(name.c_str(), "TI", 2) == 0 || strncasecmp(name.c_str(),
			"GOLD", 4) == 0 || strncasecmp(name.c_str(), "GEM", 3) == 0)
	{
		Uint32 i = 0;
		/* This is a hack to seed the map with semi-reasonable amounts of
		 * resource growth.  This will hopefully become less ugly after the code
		 * to manage resource growth has been written. */
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

void CnCMap::loadPal(const std::string& paln, SDL_Color *palette)
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
SDL_Surface *CnCMap::loadTile(INIFile *templini, Uint16 templ, Uint8 tile,
		Uint32* tiletype)
{
	TemplateCache::iterator ti;
	TemplateImage *theaterfile;

	SDL_Surface *retimage;

	char tilefilename[13];
	char tilenum[11];
	char *temname;

	/* The name of the file containing the template is something from
	 * templates.ini . the three first
	 * chars in the name of the theater eg. .DES .TEM .WIN */

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
				theaterfile = new TemplateImage(tilefilename, mapscaleq, 1);
			}
			else
			{
				theaterfile = new TemplateImage(tilefilename, mapscaleq);
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

	//Now return this SDL_Surface
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
	for (std::vector<SDL_Surface*>::size_type i = 0; i < tileimages.size(); i++)
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

