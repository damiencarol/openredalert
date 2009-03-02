package openra.core;

import java.util.prefs.BackingStoreException;

import org.ini4j.IniFile;

public class UnitTypeData {
	
	private String name;
	private int TechLevel;
	
	private UnitTypeData(){
		
	}

	public static UnitTypeData load(IniFile inifile, String unitTypeName) {
		try {
			if (inifile.nodeExists(unitTypeName) == false) {
				return null;
			}
		} catch (BackingStoreException e) {
			return null;
		}

		UnitTypeData theData = new UnitTypeData();
		// Set the name
		theData.setName(unitTypeName);
		theData.setTechLevel(inifile.node(unitTypeName).getInt("TechLevel", -1));
		return theData;
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	/**
	 * @param name the name to set
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * @return the techLevel
	 */
	public int getTechLevel() {
		return TechLevel;
	}

	/**
	 * @param techLevel the techLevel to set
	 */
	public void setTechLevel(int techLevel) {
		TechLevel = techLevel;
	}

}
