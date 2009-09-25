package openra.tests;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.prefs.BackingStoreException;

import openra.core.CncMap;
import openra.core.GameMode;
import openra.core.LoadMapError;
import openra.core.Trigger;

import org.ini4j.IniFile;
import org.ini4j.InvalidIniFormatException;
import org.ini4j.IniFile.Mode;
import org.junit.Test;

public class CncMapTest {

	@Test
	public void testLoadMapAllies1a() throws LoadMapError {
		// Try to Load a new map
		CncMap mapAllies1a = CncMap.loadMap(new File("data/main/general/SCG01EA.INI"));
		assertNotNull(mapAllies1a);
		// Initiate the map
		mapAllies1a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies1a.isSnowTheme());

		assertEquals(30, mapAllies1a.getWidth());
		assertEquals(36, mapAllies1a.getHeight());
		assertEquals(49, mapAllies1a.getX());
		assertEquals(45, mapAllies1a.getY());


		Trigger theTrig = mapAllies1a.getTrigger("elos");
		assertNotNull(theTrig);

		// Check that their are 11 players
		assertNotNull(mapAllies1a.getPlayerPool());
		assertEquals(19, mapAllies1a.getPlayerPool().getNumberOfPlayer());
		// Check that the first player is "Spain"
		assertEquals(mapAllies1a.getPlayerPool().getPlayer(0).getName(),
				"Spain");
		// Check that the Country of the campaign is Greece for the first
		// mission
		assertEquals(mapAllies1a.getPlayerPool().getLPlayer().getName(),
				"Greece");
		
		// Check that they 19 triggers in the map
		assertEquals(19, mapAllies1a.getTriggerPool().size());
		
		// Check that first map pack cell is template 71
		/*assertEquals(71, mapAllies1a.getMapPackDataList().get(49+0 + (45*128)).getTileNumber());
		assertEquals(71, mapAllies1a.getMapPackDataList().get(49+1 + (45*128)).getTemplateNumber());
		assertEquals(71, mapAllies1a.getMapPackDataList().get(49+2 + (45*128)).getTemplateNumber());
	
		assertEquals(187, mapAllies1a.getMapPackDataList().get(49+14 + (45*128)).getTemplateNumber());*/
	}

	@Test
	public void testLoadMapAllies2a() throws LoadMapError {
		// Try to Load a new map
		CncMap mapAllies2a = CncMap.loadMap(new File("data/main/general/SCG02EA.INI"));
		assertNotNull(mapAllies2a);
		// Initiate the map
		mapAllies2a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies2a.isSnowTheme());

		assertEquals(mapAllies2a.getWidth(), 50);
		assertEquals(mapAllies2a.getHeight(), 42);
		
		Trigger theTrig = mapAllies2a.getTrigger("lose");
		assertNotNull(theTrig);

		// Check that their are 19 players
		assertEquals(19, mapAllies2a.getPlayerPool().getNumberOfPlayer());
		// Check that the first player is "Spain"
		assertEquals(mapAllies2a.getPlayerPool().getPlayer(0).getName(),
				"Spain");
		// Check that the Country of the campaign is Greece for the first
		// mission
		assertEquals(mapAllies2a.getPlayerPool().getLPlayer().getName(),
				"Greece");

		assertEquals(mapAllies2a.getPlayerPool().getLPlayer().getTechLevel(), 2);
	}

	@Test
	public void testLoadMapAllies3a() throws LoadMapError {
		CncMap mapAllies3a = CncMap.loadMap(new File("data/main/general/SCG03EA.INI"));
		assertNotNull(mapAllies3a);

		// Initiate the map
		mapAllies3a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies3a.isSnowTheme());

		// Check that their are 19 players
		assertEquals(19, mapAllies3a.getPlayerPool().getNumberOfPlayer());

		assertEquals(72, mapAllies3a.getWidth());
		assertEquals(36, mapAllies3a.getHeight());
		
		// Check that they 22 triggers in the map
		assertEquals(22, mapAllies3a.getTriggerPool().size());
	}

	@Test
	public void testLoadMapAllies3b() throws LoadMapError {
		CncMap mapAllies3b = CncMap.loadMap(new File("data/main/general/SCG03EB.INI"));
		assertNotNull(mapAllies3b);

		// Initiate the map
		mapAllies3b.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies3b.isSnowTheme());

		assertEquals(mapAllies3b.getWidth(), 75);
		assertEquals(mapAllies3b.getHeight(), 75);

		// Check that their are 19 players
		assertEquals(19, mapAllies3b.getPlayerPool().getNumberOfPlayer());
	}

	@Test
	public void testLoadMapAllies4a() throws LoadMapError {
		CncMap mapAllies4a = CncMap.loadMap(new File("data/main/general/SCG04EA.INI"));
		assertNotNull(mapAllies4a);
		// Initiate the map
		mapAllies4a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies4a.isSnowTheme());
		assertEquals(mapAllies4a.getWidth(), 56);
		assertEquals(mapAllies4a.getHeight(), 65);

	}

	@Test
	public void testLoadMapAllies5a() throws LoadMapError {
		CncMap mapAllies5a = CncMap.loadMap(new File("data/main/general/SCG05EA.INI"));
		assertNotNull(mapAllies5a);
		// Initiate the map
		mapAllies5a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the Theme
		assertFalse(mapAllies5a.isInteriorTheme());
		assertFalse(mapAllies5a.isSnowTheme());
		assertTrue(mapAllies5a.isTemperateTheme());

		assertEquals(mapAllies5a.getWidth(), 60);
		assertEquals(mapAllies5a.getHeight(), 64);
		// Check that their are 19 players
		assertEquals(19, mapAllies5a.getPlayerPool().getNumberOfPlayer());
	}

	@Test
	public void testLoadMapAllies5b() throws LoadMapError {
		CncMap mapAllies5b = CncMap.loadMap(new File("data/main/general/SCG05EB.INI"));
		assertNotNull(mapAllies5b);
		// Initiate the map
		mapAllies5b.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapAllies5b.isInteriorTheme());
		assertFalse(mapAllies5b.isSnowTheme());
		assertTrue(mapAllies5b.isTemperateTheme());

		assertEquals(mapAllies5b.getWidth(), 92);
		assertEquals(mapAllies5b.getHeight(), 55);

		// Check that their are 19 players
		assertEquals(19, mapAllies5b.getPlayerPool().getNumberOfPlayer());
	}

	@Test
	public void testLoadMapAllies5c() throws LoadMapError {
		CncMap mapAllies5c = CncMap.loadMap(new File("data/main/general/SCG05EC.INI"));
		assertNotNull(mapAllies5c);
		// Initiate the map
		mapAllies5c.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapAllies5c.isInteriorTheme());
		assertFalse(mapAllies5c.isSnowTheme());
		assertTrue(mapAllies5c.isTemperateTheme());

		// Test the Height/Width
		assertEquals(mapAllies5c.getWidth(), 64);
		assertEquals(mapAllies5c.getHeight(), 64);

		// Test to get the first trigger
		Trigger theTrig = mapAllies5c.getTrigger("SPYS");
		assertNotNull(theTrig);
	}

	@Test
	public void testLoadMapAllies6a() throws LoadMapError {
		CncMap mapAllies6a = CncMap.loadMap(new File("data/main/general/SCG06EA.INI"));
		assertNotNull(mapAllies6a);
		// Initiate the map
		mapAllies6a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapAllies6a.isInteriorTheme());
		assertTrue(mapAllies6a.isSnowTheme());
		assertFalse(mapAllies6a.isTemperateTheme());

		// Test the Height/Width
		assertEquals(mapAllies6a.getWidth(), 94);
		assertEquals(mapAllies6a.getHeight(), 54);

		// Test to get the first trigger
		Trigger theTrig = mapAllies6a.getTrigger("spy1");
		assertNotNull(theTrig);
	}

	@Test
	public void testLoadMapAllies6b() throws LoadMapError {
		CncMap mapAllies6b = CncMap.loadMap(new File("data/main/general/SCG06EB.INI"));
		assertNotNull(mapAllies6b);
		// Initiate the map
		mapAllies6b.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapAllies6b.isInteriorTheme());
		assertTrue(mapAllies6b.isSnowTheme());
		assertFalse(mapAllies6b.isTemperateTheme());

		// Test the Height/Width
		assertEquals(mapAllies6b.getWidth(), 83);
		assertEquals(mapAllies6b.getHeight(), 78);

		// Test to get the first trigger
		Trigger theTrig = mapAllies6b.getTrigger("spy1");
		assertNotNull(theTrig);
	}

	@Test
	public void testLoadMapAllies7a() throws LoadMapError {
		CncMap mapAllies7a = CncMap.loadMap(new File("data/main/general/SCG07EA.INI"));
		assertNotNull(mapAllies7a);
		// Initiate the map
		mapAllies7a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapAllies7a.isInteriorTheme());
		assertFalse(mapAllies7a.isSnowTheme());
		assertTrue(mapAllies7a.isTemperateTheme());

		// Test the Height/Width
		assertEquals(mapAllies7a.getWidth(), 98);
		assertEquals(mapAllies7a.getHeight(), 40);

		// Test to get the first trigger
		Trigger theTrig = mapAllies7a.getTrigger("win");
		assertNotNull(theTrig);
	}

	@Test
	public void testLoadMapAllies8a() throws LoadMapError {
		CncMap mapAllies8a = CncMap.loadMap(new File("data/main/general/SCG08EA.INI"));
		assertNotNull(mapAllies8a);
		// Initiate the map
		mapAllies8a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies8a.isSnowTheme());

	}

	@Test
	public void testLoadMapAllies8b() throws LoadMapError {
		CncMap mapAllies8b = CncMap.loadMap(new File("data/main/general/SCG08EB.INI"));
		assertNotNull(mapAllies8b);
		// Initiate the map
		mapAllies8b.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies8b.isSnowTheme());

	}

	@Test
	public void testLoadMapAllies9a() throws LoadMapError {
		CncMap mapAllies9a = CncMap.loadMap(new File("data/main/general/SCG09EA.INI"));
		assertNotNull(mapAllies9a);
		// Initiate the map
		mapAllies9a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertFalse(mapAllies9a.isSnowTheme());
	}

	@Test
	public void testLoadMapAllies9b() throws LoadMapError {
		CncMap mapAllies9b = CncMap.loadMap(new File("data/main/general/SCG09EB.INI"));
		assertNotNull(mapAllies9b);
		// Initiate the map
		mapAllies9b.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertFalse(mapAllies9b.isSnowTheme());
	}

	@Test
	public void testLoadMapAllies10a() throws LoadMapError {
		CncMap mapAllies10a = CncMap.loadMap(new File("data/main/general/SCG10EA.INI"));
		assertNotNull(mapAllies10a);
		// Initiate the map
		mapAllies10a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies10a.isSnowTheme());

		assertEquals(mapAllies10a.getHeight(), 56);
		assertEquals(mapAllies10a.getWidth(), 80);
	}

	@Test
	public void testLoadMapAllies10b() throws LoadMapError {
		CncMap mapAllies10b = CncMap.loadMap(new File("data/main/general/SCG10EB.INI"));
		assertNotNull(mapAllies10b);
		// Initiate the map
		mapAllies10b.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en intérieur
		assertTrue(mapAllies10b.isInteriorTheme());
	}

	@Test
	public void testLoadMapAllies11a() throws LoadMapError {
		CncMap mapAllies11a = CncMap.loadMap(new File("data/main/general/SCG11EA.INI"));
		assertNotNull(mapAllies11a);
		// Initiate the map
		mapAllies11a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies11a.isSnowTheme());
	}

	@Test
	public void testLoadMapAllies11b() throws LoadMapError {
		CncMap mapAllies11b = CncMap.loadMap(new File("data/main/general/SCG11EB.INI"));
		assertNotNull(mapAllies11b);
		// Initiate the map
		mapAllies11b.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies11b.isSnowTheme());
	}

	@Test
	public void testLoadMapAllies12a() throws LoadMapError {
		CncMap mapAllies12a = CncMap.loadMap(new File("data/main/general/SCG12EA.INI"));
		assertNotNull(mapAllies12a);
		// Initiate the map
		mapAllies12a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies12a.isSnowTheme());
	}

	@Test
	public void testLoadMapAllies13a() throws LoadMapError {
		CncMap mapAllies13a = CncMap.loadMap(new File("data/main/general/SCG13EA.INI"));
		assertNotNull(mapAllies13a);
		// Initiate the map
		mapAllies13a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en intérieur
		assertTrue(mapAllies13a.isInteriorTheme());
	}

	@Test
	public void testLoadMapAllies14a() throws LoadMapError {
		CncMap mapAllies14a = CncMap.loadMap(new File("data/main/general/SCG14EA.INI"));
		assertNotNull(mapAllies14a);
		// Initiate the map
		mapAllies14a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test si la map est en neige
		assertTrue(mapAllies14a.isSnowTheme());
	}

	@Test
	public void testLoadMapSoviet1a() throws LoadMapError {
		// Try to Load a new map
		CncMap mapSoviet1a = CncMap.loadMap(new File("data/main/general/SCU01EA.INI"));
		assertNotNull(mapSoviet1a);
		// Initiate the map
		mapSoviet1a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapSoviet1a.isInteriorTheme());
		assertTrue(mapSoviet1a.isSnowTheme());
		assertFalse(mapSoviet1a.isTemperateTheme());

		// Test the Height/Width
		assertEquals(mapSoviet1a.getWidth(), 32);
		assertEquals(mapSoviet1a.getHeight(), 38);

		// Test to get the first trigger
		Trigger theTrig = mapSoviet1a.getTrigger("win1");
		assertNotNull(theTrig);

		// Check that their are 11 players
		assertEquals(19, mapSoviet1a.getPlayerPool().getNumberOfPlayer());
		// Check that the first player is "Spain"
		assertEquals(mapSoviet1a.getPlayerPool().getPlayer(0).getName(),
				"Spain");
		// Check that the Country of the campaign is Greece for the first
		// mission
		assertEquals(mapSoviet1a.getPlayerPool().getLPlayer().getName(), "USSR");

		assertEquals(mapSoviet1a.getPlayerPool().getLPlayer().getTechLevel(), 0);
	}

	@Test
	public void testLoadMapSoviet2a() throws LoadMapError {
		// Try to Load a new map
		CncMap mapSoviet2a = CncMap.loadMap(new File("data/main/general/SCU02EA.INI"));
		assertNotNull(mapSoviet2a);
		// Initiate the map
		mapSoviet2a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapSoviet2a.isInteriorTheme());
		assertFalse(mapSoviet2a.isSnowTheme());
		assertTrue(mapSoviet2a.isTemperateTheme());

		// Test the Height/Width
		assertEquals(mapSoviet2a.getWidth(), 50);
		assertEquals(mapSoviet2a.getHeight(), 40);

		// Test to get the first trigger
		Trigger theTrig = mapSoviet2a.getTrigger("set");
		assertNotNull(theTrig);

		// Check that their are 11 players
		assertEquals(19, mapSoviet2a.getPlayerPool().getNumberOfPlayer());
		// Check that the first player is "Spain"
		assertEquals(mapSoviet2a.getPlayerPool().getPlayer(0).getName(),
				"Spain");
		// Check that the Country of the campaign is Greece for the first
		// mission
		assertEquals(mapSoviet2a.getPlayerPool().getLPlayer().getName(), "USSR");

		assertEquals(mapSoviet2a.getPlayerPool().getLPlayer().getTechLevel(), 2);

		assertNotNull(mapSoviet2a.getUnitAndStructurePool().getUnitType("V2RL"));
	}

	@Test
	public void testLoadMapSoviet3a() throws LoadMapError {
		// Try to Load a new map
		CncMap mapSoviet3a = CncMap.loadMap(new File("data/main/general/SCU03EA.INI"));
		assertNotNull(mapSoviet3a);
		// Initiate the map
		mapSoviet3a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapSoviet3a.isInteriorTheme());
		assertTrue(mapSoviet3a.isSnowTheme());
		assertFalse(mapSoviet3a.isTemperateTheme());

		// Test the Height/Width
		assertEquals(mapSoviet3a.getWidth(), 70);
		assertEquals(mapSoviet3a.getHeight(), 49);

		// Test to get the first trigger
		Trigger theTrig = mapSoviet3a.getTrigger("rev1");
		assertNotNull(theTrig);

		// Check that their are 11 players
		assertEquals(19, mapSoviet3a.getPlayerPool().getNumberOfPlayer());
		// Check that the first player is "Spain"
		assertEquals(mapSoviet3a.getPlayerPool().getPlayer(0).getName(),
				"Spain");
		// Check that the Country of the campaign is Greece for the first
		// mission
		assertEquals(mapSoviet3a.getPlayerPool().getLPlayer().getName(), "USSR");

		assertEquals(mapSoviet3a.getPlayerPool().getLPlayer().getTechLevel(), 3);

		// Control some unit type that are loaded
		assertNotNull(mapSoviet3a.getUnitAndStructurePool().getUnitType("E1"));
		assertNotNull(mapSoviet3a.getUnitAndStructurePool().getUnitType("E3"));
		assertNotNull(mapSoviet3a.getUnitAndStructurePool().getUnitType("SPY"));
		assertNotNull(mapSoviet3a.getUnitAndStructurePool().getUnitType("C6"));
		assertNotNull(mapSoviet3a.getUnitAndStructurePool().getUnitType("C7"));

		assertNotNull(mapSoviet3a.getUnitAndStructurePool().getUnitType("LST"));

		assertEquals("LST", mapSoviet3a.getUnitAndStructurePool().getUnitType(
				"LST").getName());

		// Check if their are 34 Unit in this map when loaded and initialized
		assertEquals(34, mapSoviet3a.getUnitAndStructurePool().getUnitPool()
				.size());
	}

	@Test
	public void testLoadMapSoviet4a() throws LoadMapError {
		// Try to Load a new map
		CncMap mapSoviet4a = CncMap.loadMap(new File("data/main/general/SCU04EA.INI"));
		assertNotNull(mapSoviet4a);
		// Initiate the map
		mapSoviet4a.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);

		// Test the theme
		assertFalse(mapSoviet4a.isInteriorTheme());
		assertTrue(mapSoviet4a.isSnowTheme());
		assertFalse(mapSoviet4a.isTemperateTheme());

		// Test the Height/Width
		assertEquals(72, mapSoviet4a.getWidth());
		assertEquals(46, mapSoviet4a.getHeight());

		// Test to get the first trigger
		assertNotNull(mapSoviet4a.getTrigger("awre"));
		assertEquals(mapSoviet4a.getPlayerPool().getPlayer("Greece"),
				mapSoviet4a.getTrigger("awre").getPlayer());
		assertNotNull(mapSoviet4a.getTrigger("win!"));

		// Check that their are 11 players
		assertEquals(19, mapSoviet4a.getPlayerPool().getNumberOfPlayer());
		// Check that the first player is "Spain"
		assertEquals(mapSoviet4a.getPlayerPool().getPlayer(0).getName(),
				"Spain");
		// Check that the Country of the campaign is Greece for the first
		// mission
		assertEquals(mapSoviet4a.getPlayerPool().getLPlayer().getName(), "USSR");

		assertEquals(mapSoviet4a.getPlayerPool().getLPlayer().getTechLevel(), 4);

		// Control some unit type that are loaded
		assertNotNull(mapSoviet4a.getUnitAndStructurePool().getUnitType("E1"));
		assertNotNull(mapSoviet4a.getUnitAndStructurePool().getUnitType("E3"));
		assertNotNull(mapSoviet4a.getUnitAndStructurePool().getUnitType("SPY"));
		assertNotNull(mapSoviet4a.getUnitAndStructurePool().getUnitType("C6"));
		assertNotNull(mapSoviet4a.getUnitAndStructurePool().getUnitType("C7"));

		assertNotNull(mapSoviet4a.getUnitAndStructurePool().getUnitType("LST"));

		assertEquals("LST", mapSoviet4a.getUnitAndStructurePool().getUnitType(
				"LST").getName());

		// Check if their are 34 Unit in this map when loaded and initialized
		assertEquals(26, mapSoviet4a.getUnitAndStructurePool().getUnitPool()
				.size());
	}
	
	@Test
	public void testLoadMapMulti1a() throws LoadMapError {
		// Try to Load a new map
		CncMap map = CncMap.loadMap(new File("data/main/general/scm01ea.ini"));
		assertNotNull(map);
		// Initiate the map
		map.Init(null, GameMode.GAME_MODE_MULTIPLAYER);

		// Test the theme
		assertFalse(map.isInteriorTheme());
		assertTrue(map.isSnowTheme());
		assertFalse(map.isTemperateTheme());

		// Test the Height/Width
		assertEquals(94, map.getWidth());
		assertEquals(94, map.getHeight());

		// Test their are no trigger
		assertEquals(0, map.getTriggerPool().size());

		// Check that their are 11 players
		assertEquals(19, map.getPlayerPool().getNumberOfPlayer());
	}
	
	@Test
	public void testSaveMapAllies1a() throws LoadMapError, BackingStoreException, InvalidIniFormatException, FileNotFoundException, IOException {
		// Try to Load a new map
		CncMap mapAllies1a = CncMap.loadMap(new File("data/main/general/SCG01EA.INI"));
		assertNotNull(mapAllies1a);
		
		// Try to save the map
		mapAllies1a.save(new File("bin/SCG01EA.INI"));
		
		// Compare two maps
		IniFile file1 = new IniFile(new File("data/main/general/SCG01EA.INI"));
		IniFile file2 = new IniFile(new File("bin/SCG01EA.INI"), Mode.RW);
		
		assertTrue(file1.equals(file2));
	}

	@Test
	public void getTeamTypesTest() throws LoadMapError, BackingStoreException, InvalidIniFormatException, FileNotFoundException, IOException {
	
		// Try to Load a new map
		CncMap map = CncMap.loadMap(new File("data/main/general/scg01ea.ini"));
		assertNotNull(map);
		// Initiate the map
		map.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);
	
		// Test the teamtypes
		assertEquals(12, map.getTeamTypes().size());
		
		assertEquals("sov1", map.getTeamTypes().get(0).getName());
		
	}
}
