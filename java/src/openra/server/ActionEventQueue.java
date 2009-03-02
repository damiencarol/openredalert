package openra.server;

import java.util.Queue;

import sdljava.SDLTimer;

public class ActionEventQueue {

	private long starttick;
	private Queue<ActionEvent> eventqueue;
	
	/**
	 * Constructor, starts the timer
	 */
	ActionEventQueue(){
		starttick = SDLTimer.getTicks();
	}


	/**
	 * Schedules event for later execution.
	 *
	 * @param event ActionEvent object to run.
	 */
	void scheduleEvent(ActionEvent event)
	{
	    // Check if event != NULL
	    if (event == null)
	    {
	        // Return to avoid error
	        return;
	    }

	    event.addCurtick(getElapsedTime());

	    eventqueue.add(event);
	    event.setActionEventQueue(this);
	}

	/**
	 * Run all events in the action queue.
	 */
	void runEvents()
	{
	    long curtick = getElapsedTime();

	    // run all events in the queue with a priority lower than current tick
	    while( !eventqueue.isEmpty() && eventqueue.peek().getPrio() <= curtick ) 
	    {
	        eventqueue.peek().run();
	        eventqueue.poll();
	    }
	}

	long getElapsedTime()
	{
	    return SDLTimer.getTicks() - starttick;
	}
}
