// RA_Tigger.h
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

#ifndef RA_TIGGER_H
#define RA_TIGGER_H

#include "SDL/SDL_types.h"

/**
 * Event data for trigger
 * 
 * @see triggers.h for list of TRIGGER_EVENT_TYPE
 */
class RA_Tigger
{
public:	
    /** Trigger event type */
    int event;
    /** first parameter, -1 == off */
    int param1;
    /** second parameter, 0 == off */
    int param2;
};

#endif //RA_TIGGER_H

