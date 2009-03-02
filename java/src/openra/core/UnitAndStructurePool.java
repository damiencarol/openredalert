package openra.core;

import java.util.ArrayList;
import java.util.Hashtable;

import org.ini4j.IniFile;

public class UnitAndStructurePool {

	public UnitAndStructurePool() {
		
		this.unitPool = new ArrayList<Unit>();

		this.unitOrStructureType = new Hashtable<String, UnitOrStructureType>();
		this.unitTypeIniData = new ArrayList<UnitTypeData>();
		this.structureTypeIniData = new ArrayList<StructureTypeData>();
	}
	
	private ArrayList<Unit> unitPool;

	private Hashtable<String, UnitOrStructureType> unitOrStructureType;
	private ArrayList<UnitTypeData> unitTypeIniData;
	private ArrayList<StructureTypeData> structureTypeIniData;

	private void loadIni(IniFile inifile) {

		//
		// VEHICULE TYPE
		//
		// Load V2 rocket launcher
		preloadUnitType(inifile, "V2RL");
		// Load light tank
		preloadUnitType(inifile, "1TNK");
		// Load heavy tank
		preloadUnitType(inifile, "3TNK");
		// Load medium tank
		preloadUnitType(inifile, "2TNK");
		// Load mammoth tank
		preloadUnitType(inifile, "4TNK");
		// Load mobile radar jammer
		preloadUnitType(inifile, "MRJ");
		
		
		// Load mobile artillery
		preloadUnitType(inifile, "ARTY");
		// Load harvester
		preloadUnitType(inifile, "HARV");
		// Load Mobile Construction Vehicle
		preloadUnitType(inifile, "MCV");
		// Load Ranger (as in "Ford", not "W.W. II Commando")
		preloadUnitType(inifile, "JEEP");
		
		// Load mine layer
		preloadUnitType(inifile, "MNLY");
		// Load convoy truck
		preloadUnitType(inifile, "TRUK");
		
		
		//
		// SHIP TYPE
		//
		// Load submarine
		preloadUnitType(inifile, "SS");
		
		// Load cruiser
		preloadUnitType(inifile, "CA");
		// Load transport
		preloadUnitType(inifile, "LST");
		
		// Load gun boat
		preloadUnitType(inifile, "PT");
		
		//
		// INFANTRY TYPE
		//
		// Load attack dog
		preloadUnitType(inifile, "DOG");
		// Load rifle soldier
		preloadUnitType(inifile, "E1");
		// Load grenadier
		preloadUnitType(inifile, "E2");
		// Load rocket soldier
		preloadUnitType(inifile, "E3");
		// Load flamethrower
		preloadUnitType(inifile, "E4");
		// Load engineer
		preloadUnitType(inifile, "E6");
		// Load spy
		preloadUnitType(inifile, "SPY");
		// Load thief
		preloadUnitType(inifile, "THF");
		// Load Tanya
		preloadUnitType(inifile, "E7");		
		// Load field medic
		preloadUnitType(inifile, "MEDI");
		// Load field marshal
		preloadUnitType(inifile, "GNRL");		
		// Load civilians
		preloadUnitType(inifile, "C1");
		preloadUnitType(inifile, "C2");
		preloadUnitType(inifile, "C3");
		preloadUnitType(inifile, "C4");
		preloadUnitType(inifile, "C5");
		preloadUnitType(inifile, "C6");
		preloadUnitType(inifile, "C7");
		preloadUnitType(inifile, "C8");
		preloadUnitType(inifile, "C9");
		preloadUnitType(inifile, "C10");
		
		// Load special agent
		preloadUnitType(inifile, "CHAN");
		
	}

	/**
	 * 
	 * @param inifile
	 * @deprecated use {@link UnitAndStructurePool#loadIni(IniFile)
	 *             UnitAndStructurePool.loadIni(IniFile)} instead.
	 */
	public void preloadUnitAndStructures(IniFile inifile) {
		loadIni(inifile);
	}

	public void preloadUnitType(IniFile inifile, String unitName) {
		UnitTypeData theUnitData = UnitTypeData.load(inifile, unitName);
		this.unitTypeIniData.add(theUnitData);
	}

	public void createUnit(String string, int i, int j, Player player, int k,
			int l, int m, String string2) {
		// TODO Auto-generated method stub

	}

	/**
	 * Searches the UnitType pool for a unit type with a given name. if the type
	 * can not be found, it is read in from units.ini
	 * 
	 * @param unitname
	 *            Name of the unit type to retrieve
	 * @return pointer to the UnitType value
	 */
	public UnitType getUnitType(String string) {
		if (this.unitOrStructureType.get(string) != null) {
			return (UnitType) this.unitOrStructureType.get(string);
		}
		// nothing found
		return null;
	}

	public void Init(GameMode gameMode) {

		for (int i = 0; i < this.unitTypeIniData.size(); i++) {
			UnitTypeData theData = this.unitTypeIniData.get(i);
			UnitType theUnitType = new UnitType(theData);
			this.unitOrStructureType.put(theUnitType.getName(), theUnitType);
		}
	}

	/**
	 * @return the unitPool
	 */
	public ArrayList<Unit> getUnitPool() {
		return unitPool;
	}

	public int postMove(Unit un, int newpos) {
		// TODO Auto-generated method stub
		return 0;
	}
}
