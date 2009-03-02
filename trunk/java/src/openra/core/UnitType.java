package openra.core;

public class UnitType extends UnitOrStructureType {

	private String name;

	public UnitType(UnitTypeData theData) {
		this.name = theData.getName();
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

	public boolean isInfantry() {
		// TODO Auto-generated method stub
		return false;
	}

	public int getSpeed() {
		// TODO Auto-generated method stub
		return 0;
	}

	public int getNumLayers() {
		// TODO Auto-generated method stub
		return 0;
	}

	

}
