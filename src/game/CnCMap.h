// CnCMap.h
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

#ifndef CNCMAP_H
#define CNCMAP_H

#include <map>
#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "triggers.h"
#include "video/SHPImage.h"
#include "game/MiniMapClipping.h"
#include "game/Unit.h"
#include "misc/INIFile.h"
#include "misc/gametypes.h"

class CellTrigger;
class LoadingScreen;
class CnCMap;
class MissionData;
class TemplateImage;

using std::string;

/**
 * Information about terrain placement
 */
struct TerrainEntry
{
    Uint32 shpnum;
    Sint16 xoffset;
    Sint16 yoffset;
};

struct TileList
{
    Uint16 templateNum;
    Uint8 tileNum;
};


class RA_TeamUnits
{
public:
	/** type name of the unit */
    string	tname;
    /** Number of units of this type */
    int     numb;
};

class RA_Teamtype
{
public:
    string                 	tname;
    int            			country;
    int                     props;
    int                     unknown1;
    int                     unknown2;
    int                     maxteams;
    int                     waypoint;
    /** Number of the trigger to set at all unit of the group */
    int                     trigger;
    /** Number of unit in the team */
    int                     numb_teamtypes;
    vector<RA_TeamUnits>	Units;

    /** List of command during Ai management */
    vector<AiCommand*> aiCommandList;
};


struct ScrollVector {
    Sint8 x;
    Sint8 y;
    Uint8 t;
};

struct ScrollBookmark {
    Uint16 x;
    Uint16 y;
    Uint16 xtile;
    Uint16 ytile;
};

struct ScrollData {
    Uint16 maxx, maxy;
    Uint16 curx, cury;
    Uint16 maxxtileoffs;
    Uint16 maxytileoffs;
    Uint16 curxtileoffs;
    Uint16 curytileoffs;
    Uint16 tilewidth;
};


struct TemplateTilePair {
	/** Template for Theater */
    TemplateImage *theater;
    /** Tile number in this Theater */
    Uint8 tile;
};

typedef std::map<std::string, TemplateImage*> TemplateCache;
typedef std::vector<TemplateTilePair* > TemplateTileCache;

/**
 * Map in Red Alert
 */
class CnCMap {
public:
    CnCMap();
    ~CnCMap();

    void Init(gametypes gameNumber, Uint8 gameMode);
    // old proto :: void InitCnCMap();

    // Comments with "C/S:" at the start are to do with the client/server split.
    // C/S: Members used in both client and server
    void loadMap(const char* mapname, LoadingScreen* lscreen);

    MissionData* getMissionData() ;

    bool isLoading() const ;

	bool canSpawnAt(Uint16 pos) const;
	bool isBuildableAt( Uint32 PlayerNumb, Uint16 pos, bool WaterBound ) const;
    bool isBuildableAt(Uint16 pos, Unit* excpUn = NULL) const;
    bool isBuildableAt(Uint32 PlayerNumb, Uint16 pos, Unit* excpUn = NULL) const;
    Uint16 getCost(Uint16 pos, Unit* excpUn = 0) const;
    Uint16 getWidth() const ;
    Uint16 getHeight() const ;
    Uint32 getSize() const ;
    Uint32 translateToPos(Uint16 x, Uint16 y) const;
    void translateFromPos(Uint32 pos, Uint16 *x, Uint16 *y) const;

    enum TerrainType {
        t_land=0,
        t_water=1,
        t_road=2,
        t_rock=3,
        t_tree=4,
        t_water_blocked=5,
        t_other_nonpass=7
    };

    enum ScrollDirection {s_none = 0, s_up = 1, s_right = 2, s_down = 4, s_left = 8,
        s_upright = 3, s_downright = 6, s_downleft = 12, s_upleft = 9, s_all = 15};

    // C/S: Not sure about this one
    Uint8 getGameMode() const ;

	/** C/S: These functions are client only*/
	void setTerrainOverlay( Uint32 pos, Uint32 ImgNum, Uint16 Frame ) ;

	/** C/S: These functions are client only*/
	Uint32 getTerrainOverlay( Uint32 pos ) ;

    bool SnowTheme();

    SDL_Surface *getMapTile( Uint32 pos );
    SDL_Surface *getShadowTile(Uint8 shadownum);

    RA_Teamtype* getTeamtypeByName(string TeamName);

    RA_Teamtype* getTeamtypeByNumb(unsigned int TeamNumb);

	RA_Tiggers* getTriggerByName(string TriggerName);

	void setTriggerByName(string TriggerName, RA_Tiggers *Trig);

    RA_Tiggers* getTriggerByNumb(int TriggerNumb);

    /**
     * In red alert when type is bigger the 4 it is normal ore,
     * when type is smaller or equal to 4 the resource is christal :)
     */
    bool getResource(Uint32 pos, Uint8* type, Uint8* amount) const ;

    void decreaseResource(Uint32 pos, Uint8 amount);

    /**
     * @return the resource data in a form best understood
     * by the imagecache/renderer
     */
    Uint32 getResourceFrame(Uint32 pos) const ;

    Uint32 getTiberium(Uint32 pos) const ;

    Uint32 getSmudge(Uint32 pos) const ;
    Uint32 setSmudge(Uint32 pos, Uint8 value) ;
    Uint32 setTiberium(Uint32 pos, Uint8 value) ;
    Uint32 getOverlay(Uint32 pos);
    Uint32 getTerrain(Uint32 pos, Sint16* xoff, Sint16* yoff);
    Uint8 getTerrainType(Uint32 pos) const ;

    /** Reloads all the tiles SDL_Images */
    void reloadTiles();

    Uint8 accScroll(Uint8 direction);
    Uint8 absScroll(Sint16 dx, Sint16 dy, Uint8 border);
    void doscroll();
    void setMaxScroll(Uint32 x, Uint32 y, Uint32 xtile, Uint32 ytile, Uint32 tilew);
    void setValidScroll();

    void setScrollPos(Uint32 x, Uint32 y) ;

    Uint32 getScrollPos() const ;
    Uint16 getXScroll() const ;
    Uint16 getYScroll() const ;
    Uint16 getXTileScroll() const ;
    Uint16 getYTileScroll() const ;

    SDL_Surface* getMiniMap(Uint8 pixside);
    void prepMiniClip(Uint16 sidew, Uint16 sideh) ;
    const MiniMapClipping& getMiniMapClipping() const ;

    bool toScroll() ;
    void storeLocation(Uint8 loc);
    void restoreLocation(Uint8 loc);

    Uint32 getWaypoint(Uint8 pointnr);

    void setWaypoint (Uint8 pointnr, Uint32 mappos);
    SHPImage* getPips() ;
    Uint32 getPipsNum() const ;
    SHPImage* getMoveFlash() ;
    Uint32 getMoveFlashNum() const ;
    /** X offset of viewing map
     * C/S: Client only? */
    Uint16 getX() const ;
    /** Y offset of viewing map
     * C/S: Client only? */
    Uint16 getY() const ;

    /** Checks the WW coord is valid */
    bool validCoord(Uint16 tx, Uint16 ty) const;
    /** Converts a WW coord into a more flexible coord */
    Uint32 normaliseCoord(Uint32 linenum) const;
    /** Converts a WW coord into a more flexible coord */
    Uint32 normaliseCoord(Uint16 tx, Uint16 ty) const;
    void translateCoord(Uint32 linenum, Uint16* tx, Uint16* ty) const;

    /** Return the number with string of a COMMAND */
    Uint8 UnitActionToNr(const string action);

    /** Return true if it's the last mission of the game */
    bool isEndOfGame();
private:
    enum {
    	HAS_OVERLAY=0x100,
    	HAS_TERRAIN=0x200
    };
#if _MSC_VER && _MSC_VER < 1300
#define NUMMARKS ((Uint8)5)
#else
    static const Uint8 NUMMARKS=5;
#endif
    MissionData* missionData;

    /** Load the ini part of the map */
    void loadIni();

    /** The map section of the ini */
    void simpleSections(INIFile *inifile);

    /** The advanced section of the ini*/
    void advancedSections(INIFile *inifile);

    /** Load the bin part of the map (TD)*/
    void loadBin();

    /** Load the overlay section of the map (TD)*/
    void loadOverlay(INIFile *inifile);

    /** Extract RA map data*/
    void unMapPack(INIFile *inifile);

    /** Extract RA overlay data*/
    void unOverlayPack(INIFile *inifile);

    /** Load RA TeamTypes */
    void loadTeamTypes(INIFile* fileIni);

    /**
     * load the palette
     *
     * The only thing map specific about this function is the
     *  theatre (whose palette is then loaded into SHPBase).
     */
    void loadPal(const string& paln, SDL_Color *palette);

    /** Parse the BIN part of the map (RA or TD)*/
    void parseBin(TileList *bindata);

    /** Parse the overlay part of the map */
    void parseOverlay(const Uint32& linenum, const string& name);

    /** load a specified tile*/
    SDL_Surface *loadTile(INIFile *templini, Uint16 templ, Uint8 tile,
            Uint32* tiletype);

    /** width of map in tiles */
    Uint16 width;
    /** height of map in tiles */
    Uint16 height;
    /** X coordinate for the first tile (inner map) */
    Uint16 x;
    /** Y coordinate for the first tile (inner map) */
    Uint16 y;

    /** Are we loading the map?*/
    bool loading;

    /** Have we loaded the map?*/
    bool loaded;

    ScrollData scrollpos;
    /* A array of tiles and a vector containing the images for the tiles
     * The matrix used to store terrain information.*/
    vector<Uint16> tilematrix;
    /* A array of tiles and a vector containing the images for the tiles
     * The matrix used to store terrain information.*/
    vector<Uint32> terrainoverlay;

    // Client only
    TemplateCache templateCache; //Holds cache of TemplateImage*s

    vector<SDL_Surface*> tileimages; //Holds the SDL_Surfaces of the TemplateImage
    vector<SDL_Surface*> tileimages_backup; //Holds the SDL_Surfaces of the TemplateImage
    TemplateTileCache templateTileCache; //Stores the TemplateImage* and Tile# of each SDL_Surface in tileimages

    Uint16 numShadowImg;
    vector<SDL_Surface*> shadowimages;


    /** These come from the WAYPOINTS section of the inifile, and contain start
     * locations for multiplayer maps.*/
    Uint32 waypoints[100];

    vector<Uint32> overlaymatrix;

    vector<RA_Tiggers> RaTriggers;

    vector<RA_Teamtype> RaTeamtypes;

    vector<CellTrigger> CellTriggers;

    vector<Uint16> resourcematrix;
    vector<Uint32> resourcebases;
    map<string, Uint8> resourcenames;

    vector<Uint8> terraintypes;

    map<Uint32, TerrainEntry> terrains;
    map<Uint32, Uint16> overlays;

    /// @todo We get this from the game loader part, investigate if there's a better approach.
    Uint8 maptype;
    Uint8 gamemode; // 0 - single player, 1 - skirmish, 2 - multiplayer

    /** @todo These need a better (client side only) home, (ui related)*/
    SDL_Surface *minimap, *oldmmap;
    MiniMapClipping miniclip;

    /// @todo These need a better (client side only) home (ui related)
    ScrollVector scrollvec;
    bool toscroll;
    /** stores which directions can be scrolled */
    Uint8 valscroll;
    /** stores certain map locations*/
    ScrollBookmark scrollbookmarks[NUMMARKS];
    Uint8 scrollstep, maxscroll, scrolltime;

    /// @todo These need a better (client side only) home (ui/gfx related)
    Uint32 pipsnum;
    SHPImage* pips;
    Uint32 flashnum;
    SHPImage* moveflash;

    /** RA ->  translate_64 = false
     * TD -> translate_64 = true
     * When converting WW style linenum values, do we use 64 or 128
     * as our modulus/divisor? */
    bool translate_64;

	/** only used in: Uint8 CnCMap::absScroll(Sint16 dx, Sint16 dy, Uint8 border)*/
    double fmax;
};

#endif //CNCMAP_H
