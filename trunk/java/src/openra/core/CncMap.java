package openra.core;

import java.awt.Point;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import org.ini4j.Ini;
import org.ini4j.IniFile;
import org.ini4j.InvalidIniFormatException;


public class CncMap {
	private ArrayList<UnitData> unitDataList;
	private ArrayList<TriggerData> triggerDataList;
	private ArrayList<CellTriggerData> cellTriggerDataList;
	private MissionData missionData;
	
	
	private UnitAndStructurePool uspool;
	private ArrayList<Trigger> triggerpool;
	//private Cell[] cells;
	
	/**
	 * These come from the WAYPOINTS section of the inifile, and contain start
	 * locations for multi-player maps.
	 */
	private int[] waypoints = new int[100];
	private ArrayList<TileData> mapPackDataList;
	private String digest;
	private ArrayList<TeamTypeData> teamtypeData;
	private double carryOverMoney;
	private boolean toCarryOver;
	
	public CncMap() {
		// Clear the waypoints
		for (int i = 0; i < 100; i++) {
			waypoints[i] = -1;
		}

		fmax = (double) maxscroll / 100.0;

		// Build blank MissionData
		this.missionData = new MissionData();

		// Build the Rich Player Pool
		this.playerPool = new PlayerPool();
		
		// Build the blank unit data list
		this.unitDataList = new ArrayList<UnitData>();
		// Build the blank trigger data list
		this.triggerDataList = new ArrayList<TriggerData>();
		// Build the blank cell trigger data list
		this.cellTriggerDataList = new ArrayList<CellTriggerData>();
		
		// Build the Trigger list
		this.triggerpool = new ArrayList<Trigger>();
		
		// Build the CellTrigger list
		//this.CellTriggers = new ArrayList<CellTrigger>();
	}

	/**
	 * @throws LoadMapError  
	 */
	public void Init(gametypes gameNumber, GameMode pGameMode) throws LoadMapError {
		// Init the PlayerPool
		this.playerPool.Init(pGameMode);
		
		this.uspool.Init(pGameMode);
		
		if (pGameMode == GameMode.GAME_MODE_SINGLE_PLAYER)
		{
			// Create Triggers
			for (int index=0; index<this.triggerDataList.size(); index++)
			{
				// Get the Data and related object
				TriggerData theData = this.triggerDataList.get(index);
				Player thePlayer = this.getPlayerPool().getPlayer(theData.getCountry());
				
				// Build the Trigger
				Trigger theTrigger = new Trigger(theData.getName(), thePlayer);
				
				this.triggerpool.add(theTrigger);
			}
			
			// Create CellTriggers
			for (int index = 0; index < this.cellTriggerDataList.size(); index++)
			{
				// Get the Data and related object
				CellTriggerData theData = this.cellTriggerDataList.get(index);
				int linenum = theData.getLineNum();
				Trigger theTrigger = this.getTrigger(theData.getTrigger());
				
				// Build the cell trigger
				CellTrigger theCellTrigger = new CellTrigger(linenum, theTrigger);
				// TODO : Check if a reference must be create in Trigger object
				
				// Add it to the list
				//this.CellTriggers.add(theCellTrigger);
				theTrigger.getCellTriggerReferences().add(theCellTrigger);
			}
			
			// Create Units
			for (int i=0; i<this.unitDataList.size(); i++)
			{
				// Get the Data and related objects
				UnitData theData = this.unitDataList.get(i);
				Player thePlayer = this.playerPool.getPlayer(theData.getOwnerPlayer());
				UnitType theType = this.uspool.getUnitType(theData.getUnitType());
				
				// Build the Unit
				Unit theUnit = new Unit(theType, thePlayer);

				// Add it to the pool
				this.uspool.getUnitPool().add(theUnit);

				// Set other characteristics
				theUnit.setFacing(theData.getFacing());
				// TODO : build the mission object
				// theUnit.setMission()
				// TODO : Check if a reference must be create in Trigger object
				String triggerName = theData.getTriggerName();
				if (triggerName.equals("None") == false) {
					Trigger theTrigger = getTrigger(triggerName);
					if (theTrigger == null) System.err.println("Trigger :" + theData.getTriggerName() + " is NULL !!!");
					//theUnit.setTrigger(theTrigger);
					theTrigger.getUnitOrStructureReferences().add(theUnit);
				}
				// Set the X and Y coordinates when create a Unit
				theUnit.setXPos(theData.getLineNum() % 128);
				theUnit.setYPos(theData.getLineNum() / 128);
				theUnit.setHealth(theData.getHealth());
				theUnit.setSubPos(theData.getSubPos());
			}
			
			this.RaTeamtypes = new Vector<RA_Teamtype>();
			// Create team types
			for (int i=0; i<this.teamtypeData.size() ; i++)
			{
				TeamTypeData theData = this.teamtypeData.get(i);
				
				this.RaTeamtypes.add(new RA_Teamtype(theData.getName()));
			}
		}
	}
	
	public Trigger getTrigger(String triggerName) {
		for (int a = 0; a < this.triggerpool.size(); a++)
		{
			if (this.triggerpool.get(a).getName().equals(triggerName)){
				return this.triggerpool.get(a);
			}
		}
		return null;
	}

	/**
	 * Comments with "C/S:" at the start are to do with the client/server split.
	 * C/S: Members used in both client and server
	 * 
	 * @param mapFile
	 *            Name of the map file
	 */
	public static CncMap loadMap(File mapFile) {
		
		// Test if mapFle is not null
		if (mapFile == null){
			return null;
		}
		
		// build a new map
		CncMap theMap = new CncMap();
		theMap.loading = true;
		theMap.loaded = false;
		
		// Try to load the file as IniFile
		IniFile iniFileOfMap;
		try {
			iniFileOfMap = new IniFile(mapFile);
		} catch (BackingStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}

		// Load the ini part of the map
		try {
			theMap.loadIni(iniFileOfMap);
		} catch (LoadMapError e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}

		// Free the IniFile
		// TODO : check what to do with IniFile for closing
		// iniFileOfMap.;

		// End of loading
		theMap.loading = false;
		theMap.loaded = true;
		return theMap;

	}

	MissionData getMissionData() {
		return this.missionData;
	}

	boolean isLoading() {
		return this.loading;
	}

	boolean canSpawnAt(int pos) {
		return false;
	}

	boolean isBuildableAt(int PlayerNumb, int pos, boolean WaterBound) {
		return false;
	}

	boolean isBuildableAt(int pos, Unit excpUn) {
		return false;
	}

	boolean isBuildableAt(int PlayerNumb, int pos, Unit excpUn) {
		return false;
	}

	int getCost(int pos, Unit excpUn) {
		return 0;
	}

	public int getWidth() {
		return this.width;
	}

	public int getHeight() {
		return this.height;
	}

	int getSize() {
		return 0;
	}

	int translateToPos(int x, int y) {
		return 0;
	}

	void translateFromPos(int pos, int x, int y) {
	}

	/*
	 * public enum TerrainType int { t_land=0, t_water=1, t_road=2, t_rock=3,
	 * t_tree=4, t_water_blocked=5, t_other_nonpass=7 };
	 * 
	 * enum ScrollDirection { s_none = 0, s_up = 1, s_right = 2, s_down = 4,
	 * s_left = 8, s_upright = 3, s_downright = 6, s_downleft = 12, s_upleft =
	 * 9, s_all = 15};
	 */

	// C/S: Not sure about this one
	//GameMode getGameMode() {
	//	return this.gamemode;
	//}

	/** C/S: These functions are client only */
	void setTerrainOverlay(int pos, int ImgNum, int Frame) {
	}

	/** C/S: These functions are client only */
	int getTerrainOverlay(int pos) {
		return 0;
	}

	/** Return true if this map is in snow theme */
	public boolean isSnowTheme() {
		return this.missionData.getTheater().substring(0, 3).equals("SNO");
	}

	SDL_Surface getMapTile(int pos) {
		return null;
	}

	SDL_Surface getShadowTile(int shadownum) {
		return null;
	}

	RA_Teamtype getTeamtypeByName(String TeamName) {
		return null;
	}

	RA_Teamtype getTeamtypeByNumb(int TeamNumb) {
		return null;
	}

	/**
	 * Get a trigger by this name
	 * @param TriggerName name of the trigger
	 * @return the <code>RA_Tiggers</code> if found else return <code>null</code>
	 */
	private RA_Tiggers getTriggerByName(String TriggerName) {
		Iterator<RA_Tiggers> it = this.RaTriggers.iterator()	;	
		while (it.hasNext())
		{
			RA_Tiggers theTigg = it.next();
			if (theTigg.name.equals(TriggerName))
			{
				return theTigg;
			}
		}
		// Nothing found
		return null;
	}

	void setTriggerByName(String TriggerName, RA_Tiggers Trig) {
	}

	RA_Tiggers getTriggerByNumb(int TriggerNumb) {
		return null;
	}

	/**
	 * In red alert when type is bigger the 4 it is normal ore, when type is
	 * smaller or equal to 4 the resource is crystal :)
	 */
	boolean getResource(int pos, int type, int amount) {
		return false;
	}

	void decreaseResource(int pos, int amount) {
	}

	/**
	 * @return the resource data in a form best understood by the image
	 *         cache/renderer
	 */
	int getResourceFrame(int pos) {
		return 0;
	}

	int getTiberium(int pos) {
		return 0;
	}

	int getSmudge(int pos) {
		return 0;
	}

	int setSmudge(int pos, int value) {
		return 0;
	}

	int setTiberium(int pos, int value) {
		return 0;
	}

	int getOverlay(int pos) {
		return 0;
	}

	int getTerrain(int pos, int xoff, int yoff) {
		return 0;
	}

	int getTerrainType(int pos) {
		return 0;
	}

	/** Reloads all the tiles SDL_Images */
	void reloadTiles() {
	}

	int accScroll(int direction) {
		return 0;
	}

	int absScroll(int dx, int dy, int border) {
		return 0;
	}

	void doscroll() {
	}

	void setMaxScroll(int x, int y, int xtile, int ytile, int tilew) {
	}

	void setValidScroll() {
	}

	void setScrollPos(int x, int y) {
	}

	int getScrollPos() {
		// TODO : refactor
		return 0;// this.scrollpos.;
	}

	int getXScroll() {
		return 0;
	}

	int getYScroll() {
		return 0;
	}

	int getXTileScroll() {
		return 0;
	}

	int getYTileScroll() {
		return 0;
	}

	SDL_Surface getMiniMap(int pixside) {
		return null;
	}

	void prepMiniClip(int sidew, int sideh) {
	}

	MiniMapClipping getMiniMapClipping() {
		return null;
	}

	boolean toScroll() {
		return false;
	}

	void storeLocation(int loc) {
	}

	void restoreLocation(int loc) {
	}

	int getWaypoint(int pointnr) {
		return 0;
	}

	void setWaypoint(int pointnr, int mappos) {
	}

	SHPImage getPips() {
		return null;
	}

	int getPipsNum() {
		return 0;
	}

	SHPImage getMoveFlash() {
		return null;
	}

	int getMoveFlashNum() {
		return 0;
	}

	/**
	 * X offset of viewing map
	 */
	public int getX() {
		return x;
	}

	/**
	 * Y offset of viewing map
	 */
	public int getY() {
		return y;
	}

	/** Checks the WW coordinate is valid */
	boolean validCoord(int tx, int ty) {
		return false;
	}

	/** Converts a WW coordinate into a more flexible coord */
	int normaliseCoord(int linenum) {
		return 0;
	}

	/** Converts a WW coord into a more flexible coord */
	int normaliseCoord(int tx, int ty) {
		return 0;
	}

	/** 
	 * Translate coordinate from value in ini file
	 */
	Point translateCoord(int linenum, int tx, int ty) {
		return new Point(linenum % 128, linenum / 128);
	}

	/** Return the number with string of a COMMAND */
private	int UnitActionToNr(String action) {
		return 0;
	}

	/** Return the PlayerPool of the map */
	public PlayerPool getPlayerPool() {
		return this.playerPool;
	}

	// -------------------------------------------
	// ---------------------------------------------

	private static int HAS_OVERLAY = 0x100;
	private static int HAS_TERRAIN = 0x200;

	private static int NUMMARKS = 5;

	
	/**
	 * Loads the maps ini file containing info on dimensions, units, trees and
	 * so on.
	 * 
	 * @param inifile
	 *            Ini file to load
	 * @throws LoadMapError 
	 */
	private void loadIni(IniFile inifile) throws LoadMapError {
		// Check that file is not NULL
		if (inifile == null)
		{
			// Log the error
			System.err.println("Map ini not found.  Check your installation.\n");
			// Throw an error
			throw new LoadMapError("Error in loading the ini file");
		}
		
	   int iniFormat = inifile.node("Basic").getInt("NewINIFormat", 0);
		// WARN if the ini format is not supported
		if (iniFormat != 3)
		{
			// Log the error
			System.err.println("Only Red Alert maps are fully supported\nThe format of this Map is not supported\n");
			// Trow an error
			throw new LoadMapError("Error in loading the ini file [" + inifile.name() + "], the version of the ini (NewINIFormat) is not equal to 3");
		}

		// Build the player list
		try {
			this.playerPool.LoadIni(inifile);
		} catch (Exception e1) {
			// Log it
			System.err.println("Error in loading playerPool INI\n");
			// Throw error
			throw new LoadMapError("Error in loading playerPool INI\n");
		}
		
		
		

		// Read simple ini section
		simpleSections(inifile);

		// Try to create UnitAndStructurePool
		try
		{
			// Create the UnitAndStructurePool
			this.uspool = new UnitAndStructurePool();//this.missionData.getTheater());
		}
		catch (Exception ex)
		{
			// Log it
			System.err.println("Error in creating UnitAndStructurePool (p::uspool)\n");
			// Throw error
			throw new LoadMapError("Error in creating UnitAndStructurePool (p::uspool)\n");
		}




		// Load advanced section of the ini
		try {
			advancedSections(inifile);
			
			loadUnitSection(inifile);
			
			loadTriggerIniSection(inifile);
			
			// Load the cell trigger section
			loadCellTriggerIniSection(inifile);
			
			// Load the waypoints
			loadWaypointsIniSection(inifile);
			
			// Load and decode the Map pack section
			loadMapPackSection(inifile);
			
			loadBreifing(inifile);
			
			// Load the digest
			loadDigest(inifile);
			
		} catch (BackingStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw new LoadMapError(e.getMessage());
		}

		// Unpack the section "MapPack"
		unMapPack(inifile);

		// spawn player starts for non single player games.
		/*if (gamemode != GameMode.GAME_MODE_SINGLE_PLAYER)
		{
			//int LplayerColorNr = playerPool->MultiColourStringToNumb(pc::Config.side_colour.c_str());
			//int offset = 0;
			for (int i=0; i<pc.Config.totalplayers; i++)
			{

				//			sprintf (TempString, "%i", i+1);
				/*stringstream TempString;
				TempString.str("");
				TempString << (unsigned int)(i+1);
				printf ("Spawning player %s\n", TempString.str().c_str());
				string tmpName = "multi";
				//tmpname += TempString;
				tmpName += TempString.str();
				p::ccmap->getPlayerPool()->getPlayerNum(tmpName.c_str());
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
					}*/

					//p::ccmap->getPlayerPool()->setPlayer(i+1, Nick.c_str(), i+offset, "nod" /* pc::Config.mside.c_str() */);
				//}
				//else
				//{
					//
					// This is the local player ;)
					//
					//p::ccmap->getPlayerPool()->setLPlayer(i+1, pc::Config.nick.c_str(),pc::Config.side_colour.c_str(), pc::Config.mside.c_str());
				//}
			/*	playerPool.getPlayer(i).setMoney(pc.Config.startMoney);
			}
			//printf ("player side is %i\n", p::ccmap->getPlayerPool()->getPlayer(p::ccmap->getPlayerPool()->getLPlayerNum())->getSide());
			//playerPool->placeMultiUnits();
			
			// Place a MCV for each players
			for (int a = 0; a <8; a++)
			{
				System.out.println("%s line %i: Place multi units (MCV)\n");
				// Create the unit in the pool
				this.uspool.createUnit("MCV", waypoints[a], 0, this.getPlayerPool().getPlayer(a), 256, 0, 0, "None");
			}
		}*/
		
		// Load the images of pips and save the number
		//pipsnum = pc.imgcache.loadImage("pips.shp");


	    // Load the animation of mouvement graphics
	    // (the circle when clicking for movement)
	    //String moveflsh = "moveflsh." + missionData.getTheater().substring(0, 3);
	    //flashnum = 0;
	    // Load the images and save the number
	    //flashnum = pc.imgcache.loadImage(moveflsh);

	    //printf("moveflsh = %s   nmu = %d\n", moveflsh, flashnum);
	    //printf("pips =    nmu = %d\n", pipsnum);
	}

	private void loadDigest(IniFile inifile) throws BackingStoreException {

		if (inifile.nodeExists("Digest") == true)
		{
			// Go to the units section
			Preferences section = inifile.node("Digest");
		
			this.digest = section.get("1", "");
		}			
	}
	
	private void loadBreifing(final IniFile inifile) throws BackingStoreException {

		if (inifile.nodeExists("Briefing") == true)
		{		
			// Load the section
			Preferences toto = inifile.node("Briefing");
			String[] waypointsLine = toto.keys();
			this.briefing = "";
			// For each key found
			for (int i = 0; i < waypointsLine.length; i++)
			{
				// Decode the waypoint
				String strToDecode = toto.get(waypointsLine[i], "");
				int waypointNumber = Integer.parseInt(waypointsLine[i]);
				//int lineNum = Integer.parseInt(strToDecode);

				//int number = 71 - strToDecode.length();
				
				if (i != 0) {
					strToDecode = " " + strToDecode;
				}
				
				// Store it
				this.briefing += strToDecode;
			}
		}			
	}

	/**
	 * Decode and store all waypoints informations
	 * 
	 * @param inifile
	 * @throws BackingStoreException
	 */
	private void loadWaypointsIniSection(IniFile inifile) throws BackingStoreException {
		// Check that the cell trigger section exists
		if (inifile.nodeExists("Waypoints") == false) {
			return;
		}
		
		// Load the section
		Preferences toto = inifile.node("Waypoints");
		String[] waypointsLine = toto.keys();
		
		// For each key found
		for (int i = 0; i < waypointsLine.length; i++)
		{
			// Decode the waypoint
			String strToDecode = toto.get(waypointsLine[i], "");
			int waypointNumber = Integer.parseInt(waypointsLine[i]);
			int lineNum = Integer.parseInt(strToDecode);

			// Store it
			this.waypoints[waypointNumber] = lineNum;
		}
	}

	/**
	 * Load data of the cell trigger from ini file of the map
	 * @param inifile
	 * ini file of the map
	 * @throws BackingStoreException
	 *  throw when the 
	 */
	private void loadCellTriggerIniSection(IniFile inifile) throws BackingStoreException {
		// Check that the cell trigger section exists
		if (inifile.nodeExists("CellTriggers") == false) {
			return;
		}
		
		// Load the section
		Preferences toto = inifile.node("CellTriggers");
		String[] CellriggersLine = toto.keys();
		
		// For each key found
		for (int i = 0; i < CellriggersLine.length; i++)
		{
			// Create a new entry
			CellTriggerData theData = new CellTriggerData();
			String strToDecode = toto.get(CellriggersLine[i], "");
			String[] values = strToDecode.split(",");
			
			theData.setTrigger(values[0]);
			theData.setLineNum(Integer.parseInt(CellriggersLine[i]));

			// Store it
			this.cellTriggerDataList.add(theData);
		}
	}

	/**
	 * Load data of the trigger from ini file
	 * @param inifile
	 * @throws BackingStoreException 
	 */
	private void loadTriggerIniSection(IniFile inifile) throws BackingStoreException {
	    
		// TODO : Relocate this one
	    // Get located text for TextActionTriggers
		//IniFile messageTable = new IniFile(new File("data/main/general/tutorial.ini"));
		
		// Check that the trigger section exists
		if (inifile.nodeExists("Trigs") == false) {
			return;
		}
		
		// Load the section
		Preferences toto = inifile.node("Trigs");
		String[] triggersNames = toto.keys();
		
		for (int i = 0; i < triggersNames.length; i++)
		{
			TriggerData theData = new TriggerData();
			String strToDecode = toto.get(triggersNames[i], "");
			String[] values = strToDecode.split(",");
			
			theData.setName(triggersNames[i]);
			theData.setRepeatable(Integer.parseInt(values[0]));
			theData.setCountry(Integer.parseInt(values[1]));
			theData.setActivate(Integer.parseInt(values[2]));
			theData.setActions(Integer.parseInt(values[3]));
			theData.setTrigger1(Integer.parseInt(values[4]));
			theData.setTrigger1Param1(Integer.parseInt(values[5]));
			theData.setTrigger1Param2(Integer.parseInt(values[6]));
			theData.setTrigger2(Integer.parseInt(values[7]));
			theData.setTrigger2Param1(Integer.parseInt(values[8]));
			theData.setTrigger2Param2(Integer.parseInt(values[9]));
			theData.setAction1(Integer.parseInt(values[10]));
			theData.setAction1Param1(Integer.parseInt(values[11]));
			theData.setAction1Param2(Integer.parseInt(values[12]));
			theData.setAction1Param3(Integer.parseInt(values[13]));
			theData.setAction2(Integer.parseInt(values[14]));
			theData.setAction2Param1(Integer.parseInt(values[15]));
			theData.setAction2Param2(Integer.parseInt(values[16]));
			theData.setAction2Param3(Integer.parseInt(values[17]));

			this.triggerDataList.add(theData);
		}
		/*try
		{
			for(int keynum = 0;;keynum++ )
			{
				if (maptype == GAME_RA)
				{
					INISection::const_iterator key = inifile->readKeyValue("TRIGS", keynum);
					// is the char which separate terraintype from action.
					triggers.name = key->first;
					transform(triggers.name.begin(),triggers.name.end(), triggers.name.begin(), toupper);
					// Split the line
					vector<char*> triggsData = splitList(key->second, ',');
					// check that the line had 18 param
					if (triggsData.size()!=18) {logger->warning("error in reading trigger [%s]\n", key->first.c_str());	}
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
					//key = inifile->readKeyValue("TRIGGERS", keynum);
					// is the char which separate terraintype from action.
					//logger->warning ("%s line %i: Trigger1 text: %s\n", __FILE__, __LINE__, key->first.c_str());
					//              logger->warning ("%s line %i: Trigger2 text: %s\n", __FILE__, __LINE__, key->second.c_str());
				}
			}
		}
		catch(...)
		{}*/
	}

	/**
	 * Decode [UNITS], [SHIP] and [INFANTRY] section of the map ini file
	 * @param inifile
	 * @throws BackingStoreException 
	 */
	private void loadUnitSection(IniFile inifile) throws BackingStoreException {
		//
		// LOAD UNIT
		//
		// Check that the section "[UNITS]" exist and load it
		if (inifile.nodeExists("UNITS") == true)
		{
			// Go to the units section
			Preferences section = inifile.node("UNITS");
		
			// Get Keys
			String[] keys = section.keys();
			// Parse all entry and load it
			for (int i = 0; i < keys.length; i++) {
				String strToDecode = section.get(keys[i], "");
				String[] values = strToDecode.split(",");

				UnitData theData = new UnitData();
				theData.setOwnerPlayer(values[0]);
				theData.setUnitType(values[1]);
				// 0 = 0% / 255 = 100%
				theData.setHealth(Integer.parseInt(values[2]));
				// use translateCoord(linenum, tx, ty); to transform
				theData.setLineNum(Integer.parseInt(values[3]));
				theData.setFacing(Integer.parseInt(values[4]));
				theData.setMission(values[5]);
				theData.setTriggerName(values[6]);

				this.unitDataList.add(theData);
			}
		}
		// If their are a section called "UNITS"
		/*if (inifile->isSection(string("UNITS")) == true)
		{
			try
			{
				for(int keynum = 0;;keynum++ )
				{
					// Read the key number "keynum"
					INISection::const_iterator key = inifile->readKeyValue("UNITS", keynum);

					// ',' is the char which separate terraintype from action.
					if( sscanf(key->first.c_str(), "%d", &tmpval) == 1 &&
							sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%[^,],%s", owner, type,
									&health, &linenum, &facing, action, trigger ) == 7 )
					{
	                                    unsigned int tx;
	                                    unsigned int ty;
						// convert hash cursor in x and y
						translateCoord(linenum, &tx, &ty);
						facing = min(31,facing>>3);
						if( tx < x || ty < y || tx> x+width || ty> height+y )
						{
							continue;
						}
						linenum = (ty-y)*width + tx - x;
						// Create the unit
						p::uspool->createUnit(type, linenum, 5, playerPool->getPlayerNum(owner), health, facing, UnitActionToNr(action), trigger);

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
*/
		
		//
		// LOAD INFANTRY
		//
		// Check that the section "[INFANTRY]" exist and load it
		if (inifile.nodeExists("INFANTRY") == true)
		{
			// Go to the units section
			Preferences section = inifile.node("INFANTRY");
		
			// Get Keys
			String[] keys = section.keys();
			// Parse all entry and load it
			for (int i = 0; i < keys.length; i++) {
				String strToDecode = section.get(keys[i], "");
				String[] values = strToDecode.split(",");

				UnitData theData = new UnitData();
				theData.setOwnerPlayer(values[0]);
				theData.setUnitType(values[1]);
				// 0 = 0% / 255 = 100%
				theData.setHealth(Integer.parseInt(values[2]));
				// use translateCoord(linenum, tx, ty); to transform
				theData.setLineNum(Integer.parseInt(values[3]));
				theData.setSubPos(Integer.parseInt(values[4]));
				theData.setMission(values[5]);
				theData.setFacing(Integer.parseInt(values[6]));
				theData.setTriggerName(values[7]);

				this.unitDataList.add(theData);
			}
		}
	
		//
		// Read the "Infantry" section of the ini file	
		//
		// get the number of keys
		/*int numberOfKey = inifile->getNumberOfKeysInSection("INFANTRY");
		
		char typeCustom[500];
		
		for (int keynum = 0; (keynum < numberOfKey) && (keynum<22); keynum++ )
		{
			INISection::const_iterator key = inifile->readKeyValue("INFANTRY", keynum);
			// , is the char which separate terraintype from action.
			if( sscanf(key->first.c_str(), "%d", &tmpval ) == 1 &&
				sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%[^,],%d,%s", owner, typeCustom,
					&health, &linenum, &subpos, action, &facing, trigger ) == 8 )
			{
	                            unsigned tx;
	                            unsigned ty;
					translateCoord(linenum, &tx, &ty);
					facing = min(31,facing>>3);
					if( tx < x || ty < y || tx> x+width || ty> height+y )
					{
						continue;
					}
					linenum = (ty-y)*width + tx - x;

					p::uspool->createUnit(typeCustom, linenum, subpos, playerPool->getPlayerNum(owner), health, facing, UnitActionToNr(action), trigger);

					//printf ("%s line %i: createUnit INFANTRY, unit = %c%c%c, trigger = %s\n", __FILE__, __LINE__, type[0], type[1], type[2], trigger);
			}
		}*/
		
		
		//
		// LOAD SHIP
		//
		// Check that the section "[SHIPS]" exist and load it
		if (inifile.nodeExists("SHIPS") == true)
		{
			// Go to the units section
			Preferences section = inifile.node("SHIPS");
		
			// Get Keys
			String[] keys = section.keys();
			// Parse all entry and load it
			for (int i = 0; i < keys.length; i++) {
				String strToDecode = section.get(keys[i], "");
				String[] values = strToDecode.split(",");

				UnitData theData = new UnitData();
				theData.setOwnerPlayer(values[0]);
				theData.setUnitType(values[1]);
				// 0 = 0% / 255 = 100%
				theData.setHealth(Integer.parseInt(values[2]));
				// use translateCoord(linenum, tx, ty); to transform
				theData.setLineNum(Integer.parseInt(values[3]));
				theData.setFacing(Integer.parseInt(values[4]));
				theData.setMission(values[5]);
				theData.setTriggerName(values[6]);

				this.unitDataList.add(theData);
			}
		}
	}
	
	/**
	 * Decode [MapPack] section of the map ini file
	 * <p>
	 * The section MapPack is decoded with base64 algorithm.
	 * Afer that informations are decoded with format80 algorithm.
	 * 
	 * @param inifile
	 * @throws BackingStoreException 
	 */
	private void loadMapPackSection(IniFile inifile) throws BackingStoreException {
		//
		// LOAD MAPPACK
		//
		// Check that the section "[MapPack]" exist and load it
		if (inifile.nodeExists("MapPack") == true)
		{
			// String to decode
			String data1 = "";
			
			// Go to the units section
			Preferences section = inifile.node("MapPack");
		
			// Get Keys
			String[] keys = section.keys();
			// Parse all entry and load it
			for (int i = 0; i < keys.length; i++) {
				String strToDecode = section.get(keys[i], "");
				data1 += strToDecode;
			}
			
			//byte[] data2 = Base64.decode(data1.toCharArray());
			
			byte[] dataSrc2 = Base64.decode(data1, Base64.DECODE & Base64.DONT_BREAK_LINES);


			
			// Debug
			/*ByteBuffer bbuf = ByteBuffer.wrap(dataSrc2);
			File file = new File("64.dat");
			try {
		        // Create a writable file channel
		        FileChannel wChannel = new FileOutputStream(file, false).getChannel();		    
		        // Write the ByteBuffer contents; the bytes between the ByteBuffer's
		        // position and the limit is written to the file
		        wChannel.write(bbuf);		    
		        // Close the file
		        wChannel.close();
		    } catch (IOException e) {
		    }*/

			
			
			
			
			byte[] dataDest3 = new byte[49152];
			
			
			byte[][] chunks = new byte[6][];
			int curposChunk = 0;
			int curposNextChunk = 0;
			for (int tmpval = 0; tmpval < 6; tmpval++)
			{
				ByteBuffer dataSrc = ByteBuffer.wrap(dataSrc2);
				
				int aa = dataSrc.get(curposChunk);
				if (aa < 0) aa =  (aa + 256);
				int bb = dataSrc.get(curposChunk+1);
				if (bb < 0) bb =  (bb + 256);
				int cc = dataSrc.get(curposChunk+2);
				if (cc < 0) cc =  (cc + 256);
				curposNextChunk = curposChunk + 4 + aa + bb*256 + cc*256*256;
							
				chunks[tmpval] = new byte[curposNextChunk - (curposChunk + 4)];
				for (int j = 0; j<chunks[tmpval].length; j++)
				{
					chunks[tmpval][j] = dataSrc.get(curposChunk +j + 4);
				}
				
				curposChunk = curposNextChunk;
			}
			
			// Decode 6 coded chunks
			byte[][] format80chunks = new byte[6][8192];			
			for (int tmpval = 0; tmpval < 6; tmpval++)
			{				
				ByteBuffer dataSrc = ByteBuffer.wrap(chunks[tmpval]);
				dataSrc.order(ByteOrder.LITTLE_ENDIAN);

				ByteBuffer dataDest = ByteBuffer.wrap(format80chunks[tmpval]);
				dataDest.order(ByteOrder.LITTLE_ENDIAN);			

				Compression.decodeFormat80(dataSrc, dataDest);
			}
			
			
			// Copy to the output table
			for (int tmpval = 0; tmpval < 6; tmpval++)
			{
				for (int j = 0; j < 8192; j++)
				{
					dataDest3[j + tmpval*8192] = format80chunks[tmpval][j];
				}
			}
			
			
			// Debug
			/*ByteBuffer bbuf2 = ByteBuffer.wrap(dataDest3);
			File file2 = new File("jdecoded.dat");
			try {
		        // Create a writable file channel
		        FileChannel wChannel = new FileOutputStream(file2, false).getChannel();		    
		        // Write the ByteBuffer contents; the bytes between the ByteBuffer's
		        // position and the limit is written to the file
		        wChannel.write(bbuf2);		    
		        // Close the file
		        wChannel.close();
		    } catch (IOException e) {
		    }*/
			
			
			/*
			for (int i = 0; i < 128; i++)
			{
				for (int j = 0; j < 128; j++)
				{
					if (i == 45 && j == 48){
						int a = 0;
					}
					int num1 = dataDest3[(i*128 + j)*2];
					//if (num1 <0 ) num1 = num1 +256;
					
					int num2 = dataDest3[(i*128 + j)*2 + 1];
					//if (num2 <0 ) num2 = num2 +256;
					
					//int num3 = dataDest3[(i*128 + j)*2 + 128*128*2];
					//if (i > y && i < y + height && j > x && j < x + width)
					int num;
					//int num = dataDest3[(i*128 + j)*3+2];
					//if (num1 == -1){ num = 0;
					//}
					//else
					//{
						if (num1 < 0)num1+=256;
						if (num2 < 0)num2+=256;
						num = num1 + num2*256;// + num3;
					//}
						if (num > 999) num =0;
						
						if (num == 0 || num == 255) System.out.print("   .");
						else if (num <10) System.out.print("  " + num + "|");
						else if (num <100) System.out.print(" " + num + "|");
						else if (num <1000) System.out.print(num + "|");
						else System.out.print("###" + "|");

						//tmpval2++;
				}
				System.out.println("");
				
				//tmpval2 += (128-width);
			}

			
			System.out.println("Template number");*/
			
			
			
			
			// All data are decoded now
			// 128*128 16-bit template number
			// followed by
			// 128*128 8-bit tile numbers
			this.mapPackDataList = new ArrayList<TileData>(128*128);
			
			for (int ytile = 0; ytile < 128; ytile++)
			{
				for (int xtile = 0; xtile < 128; xtile++)
				{
					byte templateNumber1 = dataDest3[(ytile*128 + xtile)*2];
					byte templateNumber2 = dataDest3[1 + (ytile*128 + xtile)*2];
					byte tileNumber = dataDest3[ytile*128 + xtile + (128*128*2)];
					
					// Create new Tile data information
					TileData theTileData = new TileData();
					theTileData.setTemplateNumber1(templateNumber1);
					theTileData.setTemplateNumber2(templateNumber2);
					theTileData.setTileNumber(tileNumber);
					
					this.mapPackDataList.add(theTileData);
				}
			}
		}
	}
	
	/**
	 * Function to save all variables in the simple section of the map file
	 * 
	 * @param iniFile
	 */
	private void saveSimpleSection(Ini mapIniFile) {

		Ini.Section basicSection = mapIniFile.add("Basic");
		
		// Write Basic/Name
		basicSection.put("Name",  missionData.getMapname());
		
		// Write Basic/Intro
		String intro = missionData.getIntro();
		if (intro == null) {
			basicSection.put("Intro", "<none>");
		} else {
			basicSection.put("Intro", missionData.getIntro());
		}

		// Write Brief
		basicSection.put("Brief",  missionData.getBrief());

		// Write Win
		basicSection.put("Win", missionData.getWinmov());
		// Write Lose
		basicSection.put("Lose", missionData.getLosemov());
		// Write Action
		basicSection.put("Action", missionData.getAction());

		// Write Player
		basicSection.put("Player", missionData.getPlayer());

		// Write Theme
		basicSection.put("Theme", missionData.getTheme());

		// Write CarryOverMoney
		basicSection.put("CarryOverMoney", Double.toString(this.carryOverMoney));
		
		// Write ToCarryOver
		openra.core.IniFile.putYesNo(mapIniFile, "Basic", "ToCarryOver", this.toCarryOver);
		
	}
	
	private void saveDigest(final Ini mapIniFile) {

		Ini.Section basicSection = mapIniFile.add("Digest");
		
		// Write Basic/Name
		basicSection.put("1",  this.digest);
	}
	
	/**
	 * Function to load all vars in the simple sections of the inifile
	 * 
	 * TODO add key name in the log (to throw LoadMapError
	 * 
	 * @param inifile pointer
	 *            to the inifile
	 * @throws LoadMapError
	 */
	private void simpleSections(IniFile inifile) throws LoadMapError {
		try {
			// Try to read Basic/Name
			missionData.setName(inifile.node("Basic").get("Name", "<none>"));
			
			// Try to read Basic/BRIEF
			missionData.setBrief(inifile.node("Basic").get("Brief", "<none>"));
			
			// Try to read Basic/BRIEF
			missionData.setBrief(inifile.node("Basic").get("Brief", "<none>"));

			// Try to read Basic/ACTION
			missionData.setAction(inifile.node("Basic").get( "Action", "<none>"));

			// Try to read Basic/PLAYER
			missionData.setPlayer(inifile.node("Basic").get( "Player", ""));

			// Try to read Basic/THEME
			missionData.setTheme(inifile.node("Basic").get( "Theme", "No theme"));

			// Try to read Basic/WIN
			missionData.setWinmov(inifile.node("Basic").get(  "Win", "<none>"));

			// Try to read Basic/LOSE
			missionData.setLosemov(inifile.node("Basic").get( "Lose", "<none>"));

			// Try to read MAP/HEIGHT
			setHeight(inifile.node("Map").getInt(  "Height", -1));

			// Try to read Width
			setWidth(inifile.node("Map").getInt(  "Width", -1));

			// Try to read x coordinates in the map
			x = inifile.node("Map").getInt("X", -1);

			// Try to read y coordinates in the map
			y = inifile.node("Map").getInt("Y", -1);

			// Try to read the theme
			missionData.setTheater(inifile.node("Map").get("Theater", ""));

			// Save if it's the last mission
			if (inifile.node("Basic").get("EndOfGame", "No").equals("Yes")) {
				missionData.setEndOfGame(true);
			} else {
				missionData.setEndOfGame(false);
			}
			
			// Try to read the carry over money value
			this.carryOverMoney = inifile.node("Basic").getDouble("CarryOverMoney", -1);
			if (this.carryOverMoney < 0)
				throw new Exception("carryOverMoney < 0");
		}
		catch (Exception ex)
		{
			System.err.println("Error loading map: " + ex.getMessage() +"\n");
			throw new LoadMapError("Error loading map: " + ex.getMessage());
		}
	}

	/**
	 * The advanced section of the ini
	 * @param inifile
	 * @throws BackingStoreException 
	 */
	private void advancedSections(IniFile inifile) throws BackingStoreException {
			
	    // TODO Loading of art.ini (use in TERRAIN loading)
	 /*   INIFile* arts = 0;
	    arts = GetConfig("art.ini");

	    // Loading of [TERRAIN] section of the inifile
	    int numKeys = inifile->getNumberOfKeysInSection("TERRAIN");
	    for (int keynum = 0; keynum<numKeys; keynum++)
	    {
	        bool bad = false;
	        
	        // Try to load the number ?keynum? section
	        INISection::const_iterator key = inifile->readKeyValue("TERRAIN", keynum);
	        
	        // Get the name of the TERRAIN item
	        string nameTerrain = (*key).second;
	        string positionString = (*key).first;        
		
	        // Set the next entry in the terrain vector to the correct values.
	        // the map-array and shp files vill be set later
	        unsigned int posNumber;
	        unsigned int posX;
	        unsigned int posY;
	        stringstream positionSs;
	        positionSs << positionString;
	        positionSs >> posNumber;
	        translateCoord(posNumber, &posX, &posY);

	        // Check that position are in width/height
	        if (posX < x || posY < y || posX > x+width || posY > height+y)
	        {
	            continue;
	        }

	        // Detect Tree
	        if (nameTerrain[0] == 't' || nameTerrain[0] == 'T')
	        {
	            ttype = t_tree;
	        }
	        else if (nameTerrain[0] == 'r' || nameTerrain[0] == 'R')
	            ttype = t_rock;
		else
	            ttype = t_other_nonpass;

	        // calculate the new pos based on size and blocked
	        xsize = arts->readInt(nameTerrain, "XSIZE", 1);
	        ysize = arts->readInt(nameTerrain, "YSIZE", 1);

	        for (ywalk = 0; ywalk < ysize && ywalk + posY < height + y; ywalk++)
	        {
	            for (xwalk = 0; xwalk < xsize && xwalk + posX < width + x; xwalk++)
	            {
	                sprintf(type, "NOTBLOCKED%d", ywalk * xsize + xwalk);
	                try
	                {
	                    arts->readInt(nameTerrain, type);
	                }
	                catch (INI::KeyNotFound&)
	                {
	                    terraintypes[(ywalk + posY - y) * width + xwalk + posX - x] = ttype;
	                }
	            }
	        }

	        linenum = xsize*ysize;
	        int done = 0;
	        do
	        {
	            if (linenum == 0)
	            {
	                logger->error("BUG: Could not find an entry in art.ini for %s\n", nameTerrain.c_str());
	                bad = true;
	                break;
	            }
	            linenum--;
	            sprintf(type, "NOTBLOCKED%d", linenum);

	            // Try to read
	            try
	            {
	                arts->readInt(nameTerrain, type);
	            }
	            catch (...)
	            {
	                done = 1;
	            }
	        }
	        while (done == 0);

	        if (bad)
	        {
	            continue;
	        }

	        tmpterrain.xoffset = -(linenum % ysize)*24;
	        tmpterrain.yoffset = -(linenum / ysize)*24;

	        posX += linenum % ysize;
	        if (posX >= width + x)
	        {
	            tmpterrain.xoffset += 1 + posX - (width + x);
	            posX = width + x - 1;
	        }

	        posY += linenum / ysize;
	        if (posY >= height + y)
	        {
	            tmpterrain.yoffset += 1 + posY - (height + y);
	            posY = height + y - 1;
	        }

	        linenum = normaliseCoord(posX, posY);
	        string shpnameTerrain = nameTerrain + "." + missionData->theater.substr(0, 3);

	        // search the map for the image
	        imgpos = imagelist.find(shpnameTerrain);

	        // set up the overlay matrix and load some shps
	        if (imgpos != imagelist.end())
	        {
	            // this tile already has a number
	            overlaymatrix[linenum] |= HAS_TERRAIN;
	            tmpterrain.shpnum = imgpos->second << 16;
	            terrains[linenum] = tmpterrain;
	        }
	        else
	        {
	            // a new tile
	            imagelist[shpnameTerrain] = pc::imagepool->size();
	            overlaymatrix[linenum] |= HAS_TERRAIN;
	            tmpterrain.shpnum = pc::imagepool->size() << 16;
	            terrains[linenum] = tmpterrain;
	            try
	            {
	                image = new SHPImage(shpnameTerrain.c_str(), -1);
	            }
	            catch (ImageNotFound&)
	                {
	                logger->error("Could not load \"%s\"\n", shpnameTerrain.c_str());
	                throw LoadMapError("Could not load " + shpnameTerrain);
	            }
	            pc::imagepool->push_back(image);
	        }
	    }
	    // Log it (end of TERRAIN decode
	    logger->note("Terrain loaded...\n");
*/

	    // TODO decode OverlayPack section
	/*    if (maptype == GAME_RA)
	    {
	        unOverlayPack(inifile);
	    }
	    else
	    {
	        loadOverlay(inifile);
	    }
	    // Log it (end of waypoint decode
	    logger->note("OverlayPack loaded...\n");
*/

	    // TODO Try to set SMUDGE
	/*    try
	    {
	        for (int keynumb = 0;; keynumb++)
	        {
	            INISection::const_iterator key = inifile->readKeyValue("SMUDGE", keynumb);
	            // , is the char which separate terraintype from action.
	            if (sscanf(key->first.c_str(), "%d", &linenum) == 1 &&
	                    sscanf(key->second.c_str(), "SC%d", &smudgenum) == 1)
	            {
	                unsigned int txb;
	                unsigned int tyb;
	                translateCoord(linenum, &txb, &tyb);
	                if (txb < x || tyb < y || txb > x + width || tyb > height + y)
	                {
	                    continue;
	                }
	                linenum = (tyb - y) * width + txb - x;
	                overlaymatrix[linenum] |= (smudgenum << 4);
	            }
	            else if (sscanf(key->first.c_str(), "%d", &linenum) == 1 &&
	                    sscanf(key->second.c_str(), "CR%d", &smudgenum) == 1)
	            {
	                unsigned int txc;
	                unsigned int tyc;
	                translateCoord(linenum, &txc, &tyc);
	                if (txc < x || tyc < y || txc > x + width || tyc > height + y)
	                {
	                    continue;
	                }

	                linenum = (tyc - y) * width + txc - x;
	                overlaymatrix[linenum] |= ((smudgenum + 6) << 4);
	            }
	        }
	    }
	    catch (...)
	    {
	    }*/

	    // Try to read techs levels < 30
	    try
	    {
	    	IniFile rulesIni = new IniFile(new File("data/redalert/local/rules.ini"));
	        this.uspool.preloadUnitAndStructures(rulesIni);
	    }
	    catch (Exception ex)
	    {
	        System.err.println("Error during preloadUnitAndStructures() in [advancedSections()]");
	    }
	    // Log it
	    //System.out.println("UnitAndStructurePool::preloadUnitAndStructures() ok\n");

	

	    //
	    // TODO :STRUCTURES
	    //
	    // If their are a section called "STRUCTURES"
	   /* if (inifile->isSection(string("STRUCTURES")) == true)
	    {
	        try
	        {
	            for (int keynumd = 0;; keynumd++)
	            {
	                if (maptype == GAME_RA)
	                {
	                    INISection::const_iterator key = inifile->readKeyValue("STRUCTURES", keynumd);
	                    // ',' is the char which separate terraintype from action.
	                    if (sscanf(key->first.c_str(), "%d", &tmpval) == 1 &&
	                            sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%[^,]", owner, type,
	                                   &health, &linenum, &facing, trigger) == 6)
	                    {
	                        unsigned int txd;
	                        unsigned int tyd;
	                        translateCoord(linenum, &txd, &tyd);
	                        facing = min(31, facing >> 3);
	                        if (txd < x || tyd < y || txd > x + width || tyd > height + y)
	                        {
	                            continue;
	                        }
	                        linenum = (tyd - y) * width + txd - x;

	                        //printf("CnCMap::loadIni(%s)\n", owner);

	                        p::uspool->createStructure(type, linenum, playerPool->getPlayerNum(owner), health, facing, false, trigger);
	                        //                    printf ("%s line %i: createStructure STRUCTURE %s, trigger = %s\n", __FILE__, __LINE__, type, trigger);
	                    }
	                }
	                /*else if (maptype == GAME_TD)
	                {
	                    INISection::const_iterator key = inifile->readKeyValue("STRUCTURES", keynumd);
	                    // ',' is the char which separate terraintype from action.
	                    memset(trigger, 0, sizeof (trigger));
	                    if (sscanf(key->first.c_str(), "%d", &tmpval) == 1 &&
	                            sscanf(key->second.c_str(), "%[^,],%[^,],%d,%d,%d,%s", owner, type,
	                                   &health, &linenum, &facing, trigger) == 6)
	                    {
	                        translateCoord(linenum, &tx, &ty);
	                        facing = min(31, facing >> 3);
	                        if (tx < x || ty < y || tx > x + width || ty > height + y)
	                        {
	                            continue;
	                        }
	                        linenum = (ty - y) * width + tx - x;

	                        //printf("CnCMap::advancedSections(%s)\n", owner);
	                        p::uspool->createStructure(type, linenum, p::ccmap->getPlayerPool()->getPlayerNum(owner),
	                                                   health, facing, false, trigger);
	                    }
	                }*//*
	            }
	        }
	        catch (...)
	        {
	            // Log it
	            logger->error("error in CncMap::advanced...STRUCTURES ok\n");
	        }
	        // Log it
	        logger->note("CncMap::advanced...STRUCTURES ok\n");
	    }*/


		

	

	

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

	/** Load the bin part of the map (TD) */
	private void loadBin() {
	}

	/** Load the overlay section of the map (TD) */
	private void loadOverlay(IniFile inifile) {
	}

	/** Extract RA map data */
	private void unMapPack(IniFile inifile) {
	}

	/** Extract RA overlay data */
	private void unOverlayPack(IniFile inifile) {
	}

	/** Load RA TeamTypes 
	 * @throws BackingStoreException */
	private void loadTeamTypes(IniFile file) throws BackingStoreException {
		
		this.teamtypeData = new ArrayList<TeamTypeData>();
		
		
		// Load the section
		Preferences toto = file.node("TeamTypes");
		String[] teamTypesNames = toto.keys();
		
		for (int i = 0; i < teamTypesNames.length; i++)
		{
			String strToDecode = toto.get(teamTypesNames[i], "");
			//String[] values = strToDecode.split(",");
			
			TeamTypeData theData = new TeamTypeData(teamTypesNames[i], strToDecode);
			this.teamtypeData.add(theData);
		}
	}

	/**
	 * load the palette
	 * 
	 * The only thing map specific about this function is the theatre (whose
	 * palette is then loaded into SHPBase).
	 */
	private void loadPal(String paln, SDL_Color palette) {
	}

	/** Parse the BIN part of the map (RA or TD) */
	private void parseBin(TileList bindata) {
	}

	/** Parse the overlay part of the map */
	private void parseOverlay(int linenum, String name) {
	}

	/** load a specified tile */
	private SDL_Surface loadTile(IniFile templini, int templ, int tile,
			int tiletype) {
		return null;
	}

	/** width of map in tiles */
	private int width;
	/** height of map in tiles */
	private int height;
	/** X coordinate for the first tile (inner map) */
	private int x;
	/** Y coordinate for the first tile (inner map) */
	private int y;

	/** Are we loading the map? */
	private boolean loading;

	/** Have we loaded the map? */
	private boolean loaded;

	private ScrollData scrollpos;
	/**
	 * A array of tiles and a vector containing the images for the tiles The
	 * matrix used to store terrain information.
	 */
	private Vector<Integer> tilematrix;
	/**
	 * A array of tiles and a vector containing the images for the tiles The
	 * matrix used to store terrain information.
	 */
	private Vector<Integer> terrainoverlay;

	// Client only
	private TemplateCache templateCache; // Holds cache of TemplateImage*s

	private Vector<SDL_Surface> tileimages; // Holds the SDL_Surfaces of the
											// TemplateImage
	private Vector<SDL_Surface> tileimages_backup; // Holds the SDL_Surfaces of
													// the TemplateImage
	private TemplateTileCache templateTileCache; // Stores the TemplateImage*
													// and Tile# of each
													// SDL_Surface in tileimages

	private int numShadowImg;
	private Vector<SDL_Surface> shadowimages;



	private Vector<Integer> overlaymatrix;

	private Vector<RA_Tiggers> RaTriggers;

	private Vector<RA_Teamtype> RaTeamtypes;

	

	private Vector<Integer> resourcematrix;
	private Vector<Integer> resourcebases;
	private Map<String, Integer> resourcenames;

	private Vector<Integer> terraintypes;

	private Map<Integer, TerrainEntry> terrains;
	private Map<Integer, Integer> overlays;

	// / @todo We get this from the game loader part, investigate if there's a
	// better approach.
	private int maptype;
	/** 0 - single player, 1 - skirmish, 2 - multiplayer */
	//private GameMode gamemode;

	/** @todo These need a better (client side only) home, (ui related) */
	private SDL_Surface minimap, oldmmap;
	private MiniMapClipping miniclip;

	// / @todo These need a better (client side only) home (ui related)
	private ScrollVector scrollvec;
	private boolean toscroll;
	/** stores which directions can be scrolled */
	private int valscroll;
	/** stores certain map locations */
	private ScrollBookmark[] scrollbookmarks = new ScrollBookmark[NUMMARKS];
	private int scrollstep, maxscroll, scrolltime;

	// / @todo These need a better (client side only) home (ui/gfx related)
	private int pipsnum;
	private SHPImage pips;
	private int flashnum;
	private SHPImage moveflash;

	/**
	 * RA -> translate_64 = false TD -> translate_64 = true When converting WW
	 * style linenum values, do we use 64 or 128 as our modulus/divisor?
	 */
	boolean translate_64;

	/** only used in: int CnCMap::absScroll(Sint16 dx, Sint16 dy, int border) */
	double fmax;

	/** Pool of the player of the map */
	private PlayerPool playerPool;
	private String briefing;

	/**
	 * Indicates if the map match with Interior theme.  
	 * <p>
	 * This function use the <code>"[Basic]-Theater"</code> variable of .ini file of the map.
	 * 
	 * @return <code>true</code> if the Theater is Interior
	 */
	public boolean isInteriorTheme() {
		// Check if theater is "INTERIOR"
		return missionData.getTheater().equals("INTERIOR");
	}

	/**
	 * @param width the width to set
	 */
	public void setWidth(int width) {
		this.width = width;
	}

	/**
	 * @param height the height to set
	 */
	public void setHeight(int height) {
		this.height = height;
	}

	/**
	 * Indicates if the map match with Temperate theme.  
	 * <p>
	 * This function use the <code>"[Basic]-Theater"</code> variable of .ini file of the map.
	 * 
	 * @return <code>true</code> if the Theater is Temperate
	 */
	public boolean isTemperateTheme() {
		// Check if theater is "TEMPERATE"
		return missionData.getTheater().equals("TEMPERATE");
	}

	public UnitAndStructurePool getUnitAndStructurePool() {
		// return the unit and structure pool of this map
		return this.uspool;
	}

	public ArrayList<Trigger> getTriggerPool() {
		return this.triggerpool;
	}

	/**
	 * Save a map to a file
	 * @param string
	 * @throws BackingStoreException 
	 * @throws IOException 
	 * @throws FileNotFoundException 
	 * @throws InvalidIniFormatException 
	 */
	public void save(File mapFile) throws BackingStoreException, InvalidIniFormatException, FileNotFoundException, IOException {
		
		Ini ini = new Ini();

        //ini.load(new FileReader(mapFile));
		
		// Save the simple section
		saveSimpleSection(ini);
		
		// Save the team types
		saveTeamTypes(ini);
		
		// Save the triggers
		saveTriggers(ini);
		
		// Save the map settings
		saveMap(ini);
		
		// Save the waypoints
		saveWaypoints(ini);
		
		// Save the cell triggers
		saveCellTrigger(ini);
		
		// Save the map pack
		saveMapPack(ini);
		
		// Save Briefing
		saveBreifing(ini);
		
		// Save the digest
		saveDigest(ini);
		
		ini.store(new FileWriter(mapFile));
	}

	private void saveMapPack(Ini ini) {
		
		Ini.Section mapPackSection = ini.add("MapPack");
		
		
		byte[] dataDest3 = new byte[49152];
		
		for (int ytile = 0; ytile < 128; ytile++)
		{
			for (int xtile = 0; xtile < 128; xtile++)
			{
				/*int templateNumber = dataDest3[(ytile*128 + xtile)*2] + dataDest3[1 + (ytile*128 + xtile)*2];
				int tileNumber = dataDest3[ytile*128 + xtile + (128*128*2)];
				
				// Create new Tile data information
				TileData theTileData = new TileData();
				theTileData.setTemplateNumber(templateNumber);
				theTileData.setTileNumber(tileNumber);
				
				this.mapPackDataList.add(theTileData);*/
				
				TileData theTileData = this.mapPackDataList.get(xtile + 128*ytile);
				dataDest3[ytile*128 + xtile + (128*128*2)] = theTileData.getTileNumber();
				
				if (theTileData.getTemplateNumber1() == -128 && theTileData.getTemplateNumber2() == -128) {
					dataDest3[(ytile*128 + xtile)*2] = 44;
					dataDest3[1 + (ytile*128 + xtile)*2] = 44;
				} else {
					dataDest3[(ytile*128 + xtile)*2] = theTileData.getTemplateNumber1();
					dataDest3[1 + (ytile*128 + xtile)*2] = theTileData.getTemplateNumber2();
				}
			}
		}
		
		// Debug
		ByteBuffer bbuf2 = ByteBuffer.wrap(dataDest3);
		File file2 = new File("jdecoded_save.dat");
		try {
	        // Create a writable file channel
	        FileChannel wChannel = new FileOutputStream(file2, false).getChannel();		    
	        // Write the ByteBuffer contents; the bytes between the ByteBuffer's
	        // position and the limit is written to the file
	        wChannel.write(bbuf2);		    
	        // Close the file
	        wChannel.close();
	    } catch (IOException e) {
	    }
	}

	private void saveCellTrigger(Ini ini) {
		
		Ini.Section cellTriggerSection = ini.add("CellTriggers");
		
		for (int i=0; i < this.cellTriggerDataList.size(); i++)
		{
			CellTriggerData theData = this.cellTriggerDataList.get(i);
			
			cellTriggerSection.put(Integer.toString(theData.getLineNum()), theData.getTrigger());
		}
	}

	private void saveWaypoints(Ini ini) {
		
		Ini.Section waypointsSection = ini.add("Waypoints");
		
		for (int i=0; i < this.waypoints.length; i++)
		{
			if (this.waypoints[i] != -1)
				waypointsSection.put(Integer.toString(i), Integer.toString(this.waypoints[i]));
		}
	}

	private void saveMap(Ini ini) {
		
		Ini.Section trigsSection = ini.add("Map");
		
		// Write theater
		// TODO : change that
		trigsSection.put("Theater", this.missionData.getTheater());
		
		// Write abscisse
		trigsSection.put("X", Integer.toString(this.x));
		// Write ordinate
		trigsSection.put("Y", Integer.toString(this.y));
		// Write Width
		trigsSection.put("Width", Integer.toString(this.width));
		// Write Height
		trigsSection.put("Height", Integer.toString(this.height));
	}

	private void saveTriggers(Ini ini) {
		Ini.Section trigsSection = ini.add("Trigs");
		
		for (int i=0; i<this.triggerDataList.size(); i++)
		{
			TriggerData data = triggerDataList.get(i);
			
			// Write a trigger
			data.put(trigsSection);
		}	
	}

	private void saveTeamTypes(Ini ini) {
		
		Ini.Section teamTypesSection = ini.add("TeamTypes");
		
		for (int i=0; i<this.teamtypeData.size(); i++)
		{
			// Write team type
			teamTypesSection.put(this.teamtypeData.get(i).getName(), this.teamtypeData.get(i).getData());
		}		
	}

	private void saveBreifing(Ini ini) {
		
		Ini.Section basicSection = ini.add("Briefing");
		String str = this.briefing;
		String[] strSplit = str.split(" ");
		String dat = "";
		int j = 0;
		for (int i=0; i < strSplit.length; i++)
		{
			if (dat.trim().length() + strSplit[i].length() > 71)
			{				
				// Write Basic/Name
				basicSection.put(Integer.toString(j), dat.trim());
				dat = "";
				j++;
			}
			
			if (i != 0)
				dat += " ";
			dat += strSplit[i];
		}
		// Write Basic/Name
		basicSection.put(Integer.toString(j), dat.trim());
	}

	/**
	 * @return the mapPackDataList
	 */
	@Deprecated
	public ArrayList<TileData> getMapPackDataList() {
		return mapPackDataList;
	}

	public Vector<RA_Teamtype> getTeamTypes() {
		return this.RaTeamtypes;
	}
}
