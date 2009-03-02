package openra.core;

public class Player {
	
	private String name;
	private int techLevel;

	/**
	 * 
	 */
	public Player(PlayerData theData) {
		this.name = theData.getName();
		this.techLevel = theData.getTechLevel();
	}

	public void setMoney(int startMoney) {
		// TODO Auto-generated method stub
		
	}

	/**
	 * @param name the name to set
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	public int getTechLevel() {
		return this.techLevel;
	}

}
