package openra.server;

import openra.core.CncMap;
import openra.core.GameMode;

public class Game {
	
	private CncMap cncMap;
	
	private GameMode gameMode;

	/**
	 * @return the cncMap
	 */
	public CncMap getCncMap() {
		return cncMap;
	}

	/**
	 * @param cncMap the cncMap to set
	 */
	public void setCncMap(CncMap cncMap) {
		this.cncMap = cncMap;
	}

	/**
	 * @return the gameMode
	 */
	public GameMode getGameMode() {
		return gameMode;
	}

	/**
	 * @param gameMode the gameMode to set
	 */
	public void setGameMode(GameMode gameMode) {
		this.gameMode = gameMode;
	}
	
	
}
