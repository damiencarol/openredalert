package openra.core;

import org.ini4j.Ini.Section;

/**
 *  Trigger in RedAlert missions
 * @author D.Carol
 *
 */
public class TriggerData {
	private String  name;
	/** The first number tells RA whether this is a repeating trigger or not.
  If it has the value 0, then the trigger will only ever be activated once
  (not strictly true as the trigger can be fired again if associated with
  a teamtype).  If it has a value greater than 0, then the trigger is a
  repeating trigger (ie it will be fired more than once).
    For repeating triggers, there are two types.  When the repeating
  trigger has a value of 1, the trigger will only occur once the trigger
  event has happened to all items (units and buildings) to which this
  trigger has been attached.  This is useful if you want, for example,
  some action to occur after a specific set of buildings have been
  destroy.
    The second type of repeating trigger is the free repeater.  When this
  item has the value of 2, it will continue to repeat itself whenever its
  trigger event is true.  This is of use if you want, for example, to have
  the trigger activate every 20 time units.  */
    private int repeatable;
    /** which country trigger applies to */
    private int country;
    /**
     * When to activate trigger
     * 0 = Only trigger event 1 must be true
     * 1 = Triggger event one and two must be true
     * 2 = Either the first or the second trigger event
     *      must be true (activate all associated action on trigger1
     *      or trigger2)
     * 3 = Either the first or the second trigger event must be true
     *      (activate action 1 for trigger1, activate action2 for
     *      trigger2)
     */
    private int activate;
    /** which actions are triggered,
     * 0 == only one trigger action,
     * 1 == both trigger actions are activated
     */
    private int actions;
    /** first trigger */
    private int trigger1;
    private int trigger1Param1;
    private int trigger1Param2;
    
    /** second trigger */
    /** first trigger */
    private int trigger2;
    private int trigger2Param1;
    private int trigger2Param2;
    
    /** 
     * first trigger action
     */    
    private int action1;
    private int action1Param1;
    private int action1Param2;
    private int action1Param3;
    
    /** second trigger action,  If this trigger has more than
     * one trigger action associated with an event, then the
     * second trigger action is held here.*/
    private int action2;
    private int action2Param1;
    private int action2Param2;
    private int action2Param3;
	/**
	 * @return the actions
	 */
	public int getActions() {
		return actions;
	}
	/**
	 * @param actions the actions to set
	 */
	public void setActions(int actions) {
		this.actions = actions;
	}
	/**
	 * @return the action1
	 */
	public int getAction1() {
		return action1;
	}
	/**
	 * @param action1 the action1 to set
	 */
	public void setAction1(int action1) {
		this.action1 = action1;
	}
	/**
	 * @return the action1Param1
	 */
	public int getAction1Param1() {
		return action1Param1;
	}
	/**
	 * @param action1Param1 the action1Param1 to set
	 */
	public void setAction1Param1(int action1Param1) {
		this.action1Param1 = action1Param1;
	}
	/**
	 * @return the action1Param2
	 */
	public int getAction1Param2() {
		return action1Param2;
	}
	/**
	 * @param action1Param2 the action1Param2 to set
	 */
	public void setAction1Param2(int action1Param2) {
		this.action1Param2 = action1Param2;
	}
	/**
	 * @return the action1Param3
	 */
	public int getAction1Param3() {
		return action1Param3;
	}
	/**
	 * @param action1Param3 the action1Param3 to set
	 */
	public void setAction1Param3(int action1Param3) {
		this.action1Param3 = action1Param3;
	}
	/**
	 * @return the action2
	 */
	public int getAction2() {
		return action2;
	}
	/**
	 * @param action2 the action2 to set
	 */
	public void setAction2(int action2) {
		this.action2 = action2;
	}
	/**
	 * @return the action2Param1
	 */
	public int getAction2Param1() {
		return action2Param1;
	}
	/**
	 * @param action2Param1 the action2Param1 to set
	 */
	public void setAction2Param1(int action2Param1) {
		this.action2Param1 = action2Param1;
	}
	/**
	 * @return the action2Param2
	 */
	public int getAction2Param2() {
		return action2Param2;
	}
	/**
	 * @param action2Param2 the action2Param2 to set
	 */
	public void setAction2Param2(int action2Param2) {
		this.action2Param2 = action2Param2;
	}
	/**
	 * @return the action2Param3
	 */
	public int getAction2Param3() {
		return action2Param3;
	}
	/**
	 * @param action2Param3 the action2Param3 to set
	 */
	public void setAction2Param3(int action2Param3) {
		this.action2Param3 = action2Param3;
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
	 * @return the repeatable
	 */
	public int getRepeatable() {
		return repeatable;
	}
	/**
	 * @param repeatable the repeatable to set
	 */
	public void setRepeatable(int repeatable) {
		this.repeatable = repeatable;
	}
	/**
	 * @return the country
	 */
	public int getCountry() {
		return country;
	}
	/**
	 * @param country the country to set
	 */
	public void setCountry(int country) {
		this.country = country;
	}
	/**
	 * @return the activate
	 */
	public int getActivate() {
		return activate;
	}
	/**
	 * @param activate the activate to set
	 */
	public void setActivate(int activate) {
		this.activate = activate;
	}
	/**
	 * @return the trigger1
	 */
	public int getTrigger1() {
		return trigger1;
	}
	/**
	 * @param trigger1 the trigger1 to set
	 */
	public void setTrigger1(int trigger1) {
		this.trigger1 = trigger1;
	}
	/**
	 * @return the trigger1Param1
	 */
	public int getTrigger1Param1() {
		return trigger1Param1;
	}
	/**
	 * @param trigger1Param1 the trigger1Param1 to set
	 */
	public void setTrigger1Param1(int trigger1Param1) {
		this.trigger1Param1 = trigger1Param1;
	}
	/**
	 * @return the trigger1Param2
	 */
	public int getTrigger1Param2() {
		return trigger1Param2;
	}
	/**
	 * @param trigger1Param2 the trigger1Param2 to set
	 */
	public void setTrigger1Param2(int trigger1Param2) {
		this.trigger1Param2 = trigger1Param2;
	}
	/**
	 * @return the trigger2
	 */
	public int getTrigger2() {
		return trigger2;
	}
	/**
	 * @param trigger2 the trigger2 to set
	 */
	public void setTrigger2(int trigger2) {
		this.trigger2 = trigger2;
	}
	/**
	 * @return the trigger2Param1
	 */
	public int getTrigger2Param1() {
		return trigger2Param1;
	}
	/**
	 * @param trigger2Param1 the trigger2Param1 to set
	 */
	public void setTrigger2Param1(int trigger2Param1) {
		this.trigger2Param1 = trigger2Param1;
	}
	/**
	 * @return the trigger2Param2
	 */
	public int getTrigger2Param2() {
		return trigger2Param2;
	}
	/**
	 * @param trigger2Param2 the trigger2Param2 to set
	 */
	public void setTrigger2Param2(int trigger2Param2) {
		this.trigger2Param2 = trigger2Param2;
	}
	public void put(Section trigsSection) {
		
		String value = "";
		
		value += Integer.toString(this.repeatable);
		value += ",";
		value += Integer.toString(this.country);
		value += ",";
		value += Integer.toString(this.activate);
		value += ",";
		value += Integer.toString(this.actions);
		value += ",";
		
		value += Integer.toString(this.trigger1);
		value += ",";
		value += Integer.toString(this.trigger1Param1);
		value += ",";
		value += Integer.toString(this.trigger1Param2);
		value += ",";
		
		value += Integer.toString(this.trigger2);
		value += ",";
		value += Integer.toString(this.trigger2Param1);
		value += ",";
		value += Integer.toString(this.trigger2Param2);
		value += ",";	
		
		value += Integer.toString(this.action1);
		value += ",";
		value += Integer.toString(this.action1Param1);
		value += ",";
		value += Integer.toString(this.action1Param2);
		value += ",";
		value += Integer.toString(this.action1Param3);
		value += ",";
		
		value += Integer.toString(this.action2);
		value += ",";
		value += Integer.toString(this.action2Param1);
		value += ",";
		value += Integer.toString(this.action2Param2);
		value += ",";
		value += Integer.toString(this.action2Param3);
		
		
		trigsSection.put(this.name, value);
	}
}
