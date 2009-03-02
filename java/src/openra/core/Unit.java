package openra.core;

import openra.server.ActionEvent;
import openra.server.WalkAnimEvent;

public class Unit extends UnitOrStructure {

	private int facing;
	private int health;
	
	private int subPos;
	
	private int xPos;
	private int yPos;

	/**
	 * @param theType
	 * @param thePlayer
	 * @throws Exception 
	 */
	public Unit(UnitType theType, Player thePlayer) {
		super();
		
		this.unitType = theType;
		this.player = thePlayer;

		this.facing = -1;
		this.health = 256;
		//this.trigger = null;
	}

	private UnitType unitType;
	private Player player;
	//private Trigger trigger;
	public int xoffset;
	public int yoffset;
	public InfantryGroup infgrp;
	private ActionEvent attackAnim;
	private WalkAnimEvent walkanim;
	private	TurnAnimEvent turnAnim1;
	private   TurnAnimEvent turnAnim2;

	/**
	 * Constructor is private to force construction of the unit with a player
	 * and a type of Unit
	 */
	@SuppressWarnings("unused")
	private Unit() {

	}

	/**
	 * @return the theType
	 */
	public UnitType getUnitType() {
		return unitType;
	}

	/**
	 * @param theType
	 *            the theType to set
	 */
	public void setUnitType(UnitType theType) {
		this.unitType = theType;
	}

	/**
	 * @return the thePlayer
	 */
	public Player getPlayer() {
		return player;
	}

	/**
	 * @param thePlayer
	 *            the thePlayer to set
	 */
	public void setPlayer(Player thePlayer) {
		this.player = thePlayer;
	}

	/**
	 * @return the facing
	 */
	public int getFacing() {
		return facing;
	}

	/**
	 * @param facing
	 *            the facing to set
	 */
	public void setFacing(int facing) {
		this.facing = facing;
	}

	/**
	 * @return the health
	 */
	public int getHealth() {
		return health;
	}

	/**
	 * @param health
	 *            the health to set
	 */
	public void setHealth(int health) {
		this.health = health;
	}
/*
	/**
	 * @return the theTrigger
	 *
	public Trigger getTrigger() {
		return trigger;
	}

	/**
	 * @param theTrigger
	 *            the theTrigger to set
	 *
	public void setTrigger(Trigger theTrigger) {
		this.trigger = theTrigger;
	}
*/
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

	/**
	 * @return the xPos
	 */
	public int getXPos() {
		return xPos;
	}

	/**
	 * @param pos the xPos to set
	 */
	public void setXPos(int pos) {
		xPos = pos;
	}

	/**
	 * @return the yPos
	 */
	public int getYPos() {
		return yPos;
	}

	/**
	 * @param pos the yPos to set
	 */
	public void setYPos(int pos) {
		yPos = pos;
	}

	/**
	 * Return if a Unit is alive
	 * @return
	 */
	public boolean isAlive() {
		// TODO Auto-generated method stub
		return false;
	}

	public int getTargetCell() {
		// TODO Auto-generated method stub
		return 0;
	}

	public int getPos() {
		// TODO Auto-generated method stub
		return yPos * 128 + xPos;
	}

	/**
	 * @return the attackAnim
	 */
	public ActionEvent getAttackAnim() {
		return attackAnim;
	}

	/**
	 * @param attackAnim the attackAnim to set
	 */
	public void setAttackAnim(ActionEvent attackAnim) {
		this.attackAnim = attackAnim;
	}

	/**
	 * @return the walkanim
	 */
	public WalkAnimEvent getWalkanim() {
		return walkanim;
	}

	/**
	 * @param walkanim the walkanim to set
	 */
	public void setWalkanim(WalkAnimEvent walkanim) {
		this.walkanim = walkanim;
	}

	public void turn(int face, int i) {
		// TODO Auto-generated method stub
		
	}

	public int getImageNum(int i) {
		// TODO Auto-generated method stub
		return 0;
	}

	/**
	 * @return the turnAnim1
	 */
	public TurnAnimEvent getTurnAnim1() {
		return turnAnim1;
	}

	/**
	 * @param turnAnim1 the turnAnim1 to set
	 */
	public void setTurnAnim1(TurnAnimEvent turnAnim1) {
		this.turnAnim1 = turnAnim1;
	}

	/**
	 * @return the turnAnim2
	 */
	public TurnAnimEvent getTurnAnim2() {
		return turnAnim2;
	}

	/**
	 * @param turnAnim2 the turnAnim2 to set
	 */
	public void setTurnAnim2(TurnAnimEvent turnAnim2) {
		this.turnAnim2 = turnAnim2;
	}

}
