package openra.core;

public class UnitData {
private String ownerPlayer;
private String unitType;
private int health;
private int lineNum;
private int facing;
private String mission;
private String triggerName;

private int subPos;
/**
 * @return the ownerPlayer
 */
public String getOwnerPlayer() {
	return ownerPlayer;
}
/**
 * @param ownerPlayer the ownerPlayer to set
 */
public void setOwnerPlayer(String ownerPlayer) {
	this.ownerPlayer = ownerPlayer;
}
/**
 * @return the unitType
 */
public String getUnitType() {
	return unitType;
}
/**
 * @param unitType the unitType to set
 */
public void setUnitType(String unitType) {
	this.unitType = unitType;
}
/**
 * @return the health
 */
public int getHealth() {
	return health;
}
/**
 * @param health the health to set
 */
public void setHealth(int health) {
	this.health = health;
}
/**
 * @return the lineNum
 */
public int getLineNum() {
	return lineNum;
}
/**
 * @param lineNum the lineNum to set
 */
public void setLineNum(int lineNum) {
	this.lineNum = lineNum;
}
/**
 * @return the facing
 */
public int getFacing() {
	return facing;
}
/**
 * @param facing the facing to set
 */
public void setFacing(int facing) {
	this.facing = facing;
}
/**
 * @return the mission
 */
public String getMission() {
	return mission;
}
/**
 * @param mission the mission to set
 */
public void setMission(String mission) {
	this.mission = mission;
}
/**
 * @return the triggerName
 */
public String getTriggerName() {
	return triggerName;
}
/**
 * @param triggerName the triggerName to set
 */
public void setTriggerName(String triggerName) {
	this.triggerName = triggerName;
}
/**
 * @return the subPos
 */
public int getSubPos() {
	return subPos;
}
/**
 * @param subPos the subPos to set
 */
public void setSubPos(int subPos) {
	this.subPos = subPos;
}
}
