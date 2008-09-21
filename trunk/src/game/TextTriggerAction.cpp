// TextTriggerAction.cpp
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

#include "TriggerAction.h"
#include "TextTriggerAction.h"

#include <string>

#include "video/MessagePool.h"

using std::string;

/**
 * Build a TextTriggerAction with a message in string
 * and a reference to the Message Pool
 *
 * @param message Message to show
 * @param pool The MessagePool to use
 */
TextTriggerAction::TextTriggerAction(const string& message, MessagePool* pool) :
	TriggerAction(TriggerAction::TEXT)
{
	// Set the message
	this->message = message;
	// Set the pool to use
	this->pool = pool;
}

/**
 * Destructor
 */
TextTriggerAction::~TextTriggerAction()
{
	// Set pointer to NULL
	this->pool = 0;
}

/**
 * Execute the action
 *
 * For this TextTriggerAction this method post the saved message
 *
 * @see MessagePool::postMessage()
 */
void TextTriggerAction::execute()
{
	// Post the message in the pool
	this->pool->postMessage(this->message);
}
