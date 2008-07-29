// BarrelExplosionActionEvent.h
// 1.2

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

#ifndef BARRELEXPLOSIONANIMEVENT_H
#define BARRELEXPLOSIONANIMEVENT_H

#include "ActionEvent.h"

/**
 * Animation of the barrels explosion
 * @author Damien Carol (OpenRedAlert)
 * @version 1.2
 * @since r388 
 */
class BarrelExplosionActionEvent : public ActionEvent
{
public:
	/**
	 * @param p the priority of this event
	 * @param pos The position of the explosion
	 */
	BarrelExplosionActionEvent(Uint32 p, Uint32 pos);
	
    /** */
    virtual void run();
private:

    /**
     * Position of the animation in the map 
     */
	Uint32 position;
};

#endif //BARRELEXPLOSIONANIMEVENT_H
