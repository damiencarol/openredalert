// TextTriggerAction.h
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

#ifndef TEXTTRIGGERACTION_H
#define TEXTTRIGGERACTION_H

#include <string>

#include "TriggerAction.h"

class MessagePool;

using std::string;

/**
 * Action that display a text to the screen
 */
class TextTriggerAction : public TriggerAction 
{
public:    
	/** Build a TextTriggerAction with a specified message and a messagepool */
    TextTriggerAction(string message, MessagePool* pool);
    ~TextTriggerAction();
    
    /** Execute this Action */
    void execute();
private:    
    string message;
    MessagePool* pool;
};

#endif //TEXTTRIGGERACTION_H