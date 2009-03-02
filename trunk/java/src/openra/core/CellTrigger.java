package openra.core;

public class CellTrigger {
	/**
	 * @param linenum
	 * @param trigger
	 */
	public CellTrigger(int linenum, Trigger trigger) {
		super();
		this.linenum = linenum;
		this.trigger = trigger;
	}
	private int linenum;
	private Trigger trigger;
	/**
	 * @return the linenum
	 */
	public int getLinenum() {
		return linenum;
	}
	/**
	 * @return the trigger
	 */
	public Trigger getTrigger() {
		return trigger;
	}
}
