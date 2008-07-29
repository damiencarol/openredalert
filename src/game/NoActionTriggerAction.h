// NoActionTriggerAction.h
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

#ifndef NOACTIONTRIGGERACTION_H
#define NOACTIONTRIGGERACTION_H

#include "TriggerAction.h"

/**
 * Action that do nothing.
 * 
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r379
 */
class NoActionTriggerAction : public TriggerAction
{
public:
    /** Constructor */
    NoActionTriggerAction();

    /** Execute the action */
    void execute();
};

#endif //NOACTIONTRIGGERACTION_H
