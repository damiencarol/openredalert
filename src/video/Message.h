// Message.h
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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include "SDL/SDL_types.h"

using std::string;

/** 
 * @todo Replace this class with a std::pair<string, Uint32>
 */
class Message
{
public:
    Message(string msg, Uint32 deltime);
    
    const char* getMessage() const;
    bool expired(Uint32 time) const;
    
private:
    string message;
    Uint32 deltime;
};

#endif //MESSAGE_H
