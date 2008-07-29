// RawTriggerAction.h
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

#ifndef RAWTRIGGERACTION_H
#define RAWTRIGGERACTION_H

#include "TriggerAction.h"

/**
 * Raw TriggerAction
 *
 * This object give acces to parameters
 *
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r374
 */
class RawTriggerAction : public TriggerAction {
public:
    RawTriggerAction(int action, int param1, int param2, int param3);
    int getParam3();
    int getParam2();
    int getParam1();

    /** Execute the action */
    void execute();

private:
    int param1;
    int param2;
    int param3;
};

#endif //RAWTRIGGERACTION_H
