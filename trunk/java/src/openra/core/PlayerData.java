package openra.core;

import java.util.prefs.BackingStoreException;

import org.ini4j.IniFile;

public class PlayerData {


	private String name;
	private int techLevel;

	static public PlayerData load(IniFile inifile, String sectionName) {

		try {
			if (inifile.nodeExists(sectionName) == false) {
				return null;
			}
		} catch (BackingStoreException e) {
			return null;
		}

		PlayerData theData = new PlayerData();
		// Set the name
		theData.setName(sectionName);
		theData.setTechLevel(inifile.node(sectionName).getInt("TechLevel", -1));
		return theData;
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	/**
	 * @param name
	 *            the name to set
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * @return the techLevel
	 */
	public int getTechLevel() {
		return techLevel;
	}

	/**
	 * @param techLevel
	 *            the techLevel to set
	 */
	public void setTechLevel(int techLevel) {
		this.techLevel = techLevel;
	}

}
