package openra.core;

import java.util.ArrayList;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import org.ini4j.IniFile;

public class PlayerPool extends ArrayList<Player> {
	
	private Player localPlayer;
	private String localPlayerName;

	/**
	 * 
	 */
	public PlayerPool() {
		super();
		
		// Build structures to store raw ini data
		this.iniData = new ArrayList<PlayerData>();
	}

	private ArrayList<PlayerData> iniData;
	
	/**
	 * 
	 */
	private static final long serialVersionUID = -6082076072183446037L;

	public void LoadIni(IniFile inifile) throws BackingStoreException {
		// Check info for Players/country
		loadPlayerdata(inifile, "Spain");
		loadPlayerdata(inifile, "Greece");
		loadPlayerdata(inifile, "USSR");
		loadPlayerdata(inifile, "England");
		loadPlayerdata(inifile, "Germany");
		loadPlayerdata(inifile, "France");
		loadPlayerdata(inifile, "Turkey");

		// Check info for Players/people
		loadPlayerdata(inifile, "GoodGuy");
		loadPlayerdata(inifile, "BadGuy");
		loadPlayerdata(inifile, "Neutral");

		// Check info for Player/Special
		loadPlayerdata(inifile, "Special");
		
		// Check info for Player/MultiPlayer
		loadPlayerdata(inifile, "Multi1");
		loadPlayerdata(inifile, "Multi2");
		loadPlayerdata(inifile, "Multi3");
		loadPlayerdata(inifile, "Multi4");
		loadPlayerdata(inifile, "Multi5");
		loadPlayerdata(inifile, "Multi6");
		loadPlayerdata(inifile, "Multi7");
		loadPlayerdata(inifile, "Multi8");
		
		
		// Save the name of local player
		Preferences basicNode = inifile.node("Basic");
		localPlayerName = basicNode.get("Player", "");
	}
	
	private void loadPlayerdata(IniFile inifile, String playerSectionName)
			throws BackingStoreException {
		// Check that the section exist
		if (inifile.nodeExists(playerSectionName) == false) {
			PlayerData theData = new PlayerData();
			theData.setName(playerSectionName);
			iniData.add(theData);
			return;
		} else {

			// Get the PlayerData
			PlayerData theData = PlayerData.load(inifile, playerSectionName);
			// Add the data to the ini info list
			iniData.add(theData);
		}
	}

	public void setLPlayer(String string) {
		// TODO Auto-generated method stub
		
	}

	public Player getPlayer(int i) {
		// TODO Auto-generated method stub
		return this.get(i);
	}

	/**
	 * Return the number of players in the pool
	 * @return number of players in the pool
	 */
	public int getNumberOfPlayer() {
		// Return the number of players
		return this.size();
	}

	public void Init(GameMode pGameMode) {
		// Create Each player
		for (int j = 0; j < this.iniData.size(); j++)
		{
			PlayerData theData = this.iniData.get(j);
			Player thePlayer = new Player(theData);
			this.add(thePlayer);
		}
		
		// If we are in campaign mode
		if (pGameMode == GameMode.GAME_MODE_SINGLE_PLAYER)
		{
			// Set the local player
			this.localPlayer = getPlayerByName(this.localPlayerName);
		}
		// If we are in Multiplayer mode
		if (pGameMode == GameMode.MULTIPLAYER)
		{
			// Set All player to the technology level
			// TODO : Change the tech level in multiplayer game
		}
		
		// Initiate alliance
	}

	private Player getPlayerByName(String localPlayerName2) {
		for (int i = 0; i<this.size(); i++)
		{
			if (this.get(i).getName().equals(localPlayerName2)){
				return this.get(i);
			}
		}
		return null;
	}

	public Player getLPlayer() {
		return this.localPlayer;
	}

	public Player getPlayer(String ownerPlayer) {
		// Encapsulate the method getByName
		return getPlayerByName(ownerPlayer);
	}

}
