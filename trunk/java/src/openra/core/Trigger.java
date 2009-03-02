package openra.core;

import java.util.ArrayList;

public class Trigger {
	private String name;
	private Player player;
	
	public Trigger(String triggerName, Player thePlayer){
		this.player = thePlayer;
		this.name = triggerName;
		
		this.cellTriggerReferences = new ArrayList<CellTrigger>();
		this.unitOrStructureReferences= new ArrayList<UnitOrStructure>();
	}

	private ArrayList<UnitOrStructure> unitOrStructureReferences;
	private ArrayList<CellTrigger> cellTriggerReferences;
	
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
	 * @return the player
	 */
	public Player getPlayer() {
		return player;
	}

	/**
	 * @param player the player to set
	 */
	public void setPlayer(Player player) {
		this.player = player;
	}

	/**
	 * @return the unitOrStructureReferences
	 */
	public ArrayList<UnitOrStructure> getUnitOrStructureReferences() {
		return unitOrStructureReferences;
	}

	/**
	 * @param unitOrStructureReferences the unitOrStructureReferences to set
	 */
	public void setUnitOrStructureReferences(
			ArrayList<UnitOrStructure> unitOrStructureReferences) {
		this.unitOrStructureReferences = unitOrStructureReferences;
	}

	/**
	 * @return the cellTriggerReferences
	 */
	public ArrayList<CellTrigger> getCellTriggerReferences() {
		return cellTriggerReferences;
	}

	/**
	 * @param cellTriggerReferences the cellTriggerReferences to set
	 */
	public void setCellTriggerReferences(
			ArrayList<CellTrigger> cellTriggerReferences) {
		this.cellTriggerReferences = cellTriggerReferences;
	}
}
