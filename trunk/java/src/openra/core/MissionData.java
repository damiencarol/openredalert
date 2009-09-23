package openra.core;

/**
 * Represent all information contains in the [Base] section of the inifile of
 * the map
 */
public class MissionData {
	/** the name of the map */
	private String mapname;
	/** movie played after failed mission */
	private String losemov;
	/** movie played after completed mission */
	private String winmov;
	/** Specific music to play for this mission. */
	private String theme;
	/** The house of the player (Greece, USSR, England, etc) */
	private String player;
	/** the action movie */
	private String action;
	/** the briefing movie */
	private String brief;
	/** the theater of the map (can be "SNOW", "WINTER" or "INTERIOR") */
	private String theater;
	/** True if it's the last mission */
	private boolean endOfGame;
	private String intro;

	/**
	 * @param theater
	 *            the theater to set
	 */
	public void setTheater(String theater) {
		this.theater = theater;
	}

	/**
	 * @return the theater
	 */
	public String getTheater() {
		return theater;
	}

	/**
	 * @param mapname
	 *            the mapname to set
	 */
	public void setMapname(String mapname) {
		this.mapname = mapname;
	}

	/**
	 * @return the mapname
	 */
	public String getMapname() {
		return mapname;
	}

	/**
	 * @return the losemov
	 */
	public String getLosemov() {
		return losemov;
	}

	/**
	 * @param losemov the losemov to set
	 */
	public void setLosemov(String losemov) {
		this.losemov = losemov;
	}

	/**
	 * @return the winmov
	 */
	public String getWinmov() {
		return winmov;
	}

	/**
	 * @param winmov the winmov to set
	 */
	public void setWinmov(String winmov) {
		this.winmov = winmov;
	}

	/**
	 * @return the theme
	 */
	public String getTheme() {
		return theme;
	}

	/**
	 * @param theme the theme to set
	 */
	public void setTheme(String theme) {
		this.theme = theme;
	}

	/**
	 * @return the player
	 */
	public String getPlayer() {
		return player;
	}

	/**
	 * @param player the player to set
	 */
	public void setPlayer(String player) {
		this.player = player;
	}

	/**
	 * @return the action
	 */
	public String getAction() {
		return action;
	}

	/**
	 * @param action the action to set
	 */
	public void setAction(String action) {
		this.action = action;
	}

	/**
	 * @return the brief
	 */
	public String getBrief() {
		return brief;
	}

	/**
	 * @param brief the brief to set
	 */
	public void setBrief(String brief) {
		this.brief = brief;
	}

	/**
	 * @return the endOfGame
	 */
	public boolean isEndOfGame() {
		return endOfGame;
	}

	/**
	 * @param endOfGame the endOfGame to set
	 */
	public void setEndOfGame(boolean endOfGame) {
		this.endOfGame = endOfGame;
	}

	public String getIntro() {
		return this.intro;
	}

	public void setName(String string) {
		this.mapname = string;
	}
}
