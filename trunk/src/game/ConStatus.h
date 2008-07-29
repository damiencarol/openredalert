// ConStatus.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

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
