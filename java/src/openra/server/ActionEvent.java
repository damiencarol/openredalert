package openra.server;

public class ActionEvent {
	
	long delay;
	long prio;
	private ActionEventQueue aequeue;

	void stop()
	{
	}

	ActionEvent(long p)
	{
	    delay = p;
	}

	void addCurtick(long curtick)
	{
	    prio = delay + curtick;
	}

	void run()
	{
	}

	

	void setDelay(long p)
	{
	    delay = p;
	}

	long getPrio()
	{
	    return prio;
	}

	public void setActionEventQueue(ActionEventQueue actionEventQueue) {
		this.aequeue = actionEventQueue;
	}

	/**
	 * @return the aequeue
	 */
	public ActionEventQueue getAequeue() {
		return aequeue;
	}

	
}
