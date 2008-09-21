// TextTriggerAction.h
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

#ifndef TEXTTRIGGERACTION_H
#define TEXTTRIGGERACTION_H

#include <string>

#include "TriggerAction.h"

class MessagePool;

using std::string;

/**
 * Action that display a text to the screen
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 */
class TextTriggerAction: public TriggerAction
{
public:
	/** Build a TextTriggerAction with a specified message and a MessagePool */
	TextTriggerAction(const string& message, MessagePool* pool);
	/** Destructor */
	virtual ~TextTriggerAction();

	/** Execute this Action */
	void execute();
private:
	/** Message to display */
	string message;
	/** Message Pool */
	MessagePool* pool;
};

#endif //TEXTTRIGGERACTION_H
