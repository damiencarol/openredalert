// GlobalSetTriggerAction.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GLOBALSETTRIGGERACTION_H
#define GLOBALSETTRIGGERACTION_H

#include "TriggerAction.h"

/**
 * Action that set a Global variable. 
 */
class GlobalSetTriggerAction : public TriggerAction {
public:    
	/** Build a Global Set Action for a number */
    GlobalSetTriggerAction(int number);
    /** Execute the action */
    void execute();

private:    
    int number;
};

#endif //GLOBALSETTRIGGERACTION_H
