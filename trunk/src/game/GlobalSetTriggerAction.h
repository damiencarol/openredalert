// GlobalSetTriggerAction.h
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

#ifndef GLOBALSETTRIGGERACTION_H
#define GLOBALSETTRIGGERACTION_H

#include "TriggerAction.h"

/**
 * Action that set a Global variable.
 * 
 * @version 1.0, may 2008
 * @author Damien Carol (OpenRedAlert)
 * @since r376
 */
class GlobalSetTriggerAction : public TriggerAction {
public:
    /** Build a Global Set Action for a number */
    GlobalSetTriggerAction(int number);

    /** Execute the action */
    void execute();

private:

    /**
     * Number of the variable of the TriggerAction 
     */
    int number;
};

#endif //GLOBALSETTRIGGERACTION_H
