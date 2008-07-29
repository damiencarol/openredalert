// MessagePool.h
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

#ifndef MESSAGEPOOL_H
#define MESSAGEPOOL_H

#include <list>
#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "ui/RA_Label.h"

class Message;

using std::list;
using std::string;

/**
 * Object which manage all in-game message 
 */
class MessagePool
{
public:
	MessagePool();
	~MessagePool();

	void setWidth(Uint32 width);
	Uint32 getWidth() const;
	SDL_Surface *getMessages();
	void postMessage(string msg);
	void clear();
	void refresh();

private:
	list<Message> msglist;
	bool updated;
	SDL_Surface* textimg;
	/** Current Label */
	RA_Label* msglabel;
	Uint32 width;
};

#endif //MESSAGEPOOL_H
