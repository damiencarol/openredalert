// GlobalClearTriggerAction.cpp
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

#ifndef GLOBALCLEARTRIGGERACTION_H
#define GLOBALCLEARTRIGGERACTION_H

#include "TriggerAction.h"

/**
 * Action that CLEAR a Global variable.
 * 
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r371
 */
class GlobalClearTriggerAction : public TriggerAction {
public:
    /** Constructor */
    GlobalClearTriggerAction(int number);

    /** Execute the action, clear the global variable set */
    void execute();

private:
    /** Number of the global variable to Clear */
    int number;
};

#endif //GLOBALCLEARTRIGGERACTION_H
