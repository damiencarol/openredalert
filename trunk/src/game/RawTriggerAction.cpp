// RawTriggerAction.cpp
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

#include "RawTriggerAction.h"

RawTriggerAction::RawTriggerAction(int action, int param1, int param2, int param3) :
	TriggerAction(action)
{
	this->param1 = param1;
	this->param2 = param2;
	this->param3 = param3;
}

/** Execute the action */
void RawTriggerAction::execute()
{
}

int RawTriggerAction::getParam1()
{
	return param1;
}

int RawTriggerAction::getParam2()
{
	return param2;
}

int RawTriggerAction::getParam3()
{
	return param3;
}

