package openra.server;

import openra.core.Unit;

public class UnitAnimEvent extends ActionEvent
{
	private Unit un;
private	UnitAnimEvent scheduled;

	public UnitAnimEvent(int p, Unit un) {
		super(p);
		
	    //logger->debug("UAE cons: this:%p un:%p\n",this,un);
	    this.un = un;
	    //un.referTo();
	    scheduled = null;
	}

void	destUnitAnimEvent()
	{
	    //logger->debug("UAE dest: this:%p un:%p sch:%p\n",this,un,scheduled);
	    if (scheduled != null) {
	        this.getAequeue().scheduleEvent(scheduled);
	    }
	    //un->unrefer();
	}

	protected void setSchedule(UnitAnimEvent e)
	{
	    //logger->debug("Scheduling an event. (this: %p, e: %p)\n",this,e);
	    if (scheduled != null) {
	        scheduled.setSchedule(null);
	        scheduled.stop();
	    }
	    scheduled = e;
	}

	void stopScheduled()
	{
	    if (scheduled != null) {
	        scheduled.stop();
	    }
	}

	void update()
	{
	}

}
