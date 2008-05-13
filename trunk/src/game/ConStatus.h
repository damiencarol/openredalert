#ifndef CONSTATUS_H
#define CONSTATUS_H

/** What each state means:
 * INVALID: Build Queue in an inconsistent state, expect "undefined behaviour".
 * EMPTY: Nothing to construct.
 * RUNNING: Construction is running.  Should proceed to either READY or PAUSED.
 * PAUSED: Construction paused.  Requires user intervention to return to RUNNING.
 * READY: Construction finished, waiting for destination to be confirmed.
 * CANCELLED: Status shouldn't be set to this, but is used to inform the ui
 * when production is cancelled.
 */
enum ConStatus
{	
	BQ_INVALID, 
	BQ_EMPTY,
	BQ_RUNNING,
	BQ_PAUSED, 
	BQ_READY, 
	BQ_CANCELLED, 
	BQ_ALL_PAUSED
};

#endif //CONSTATUS_H
