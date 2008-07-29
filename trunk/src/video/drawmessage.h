// DrawMessage.h
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

#ifndef DRAWMESSAGE_H
#define DRAWMESSAGE_H

#include "Message.h"

struct drawMessage : std::unary_function<void, Message>
{
	drawMessage(RA_Label* label, SDL_Surface* textimg, Uint32& msgy) :
		label(label), textimg(textimg), msgy(msgy)
	{
	}

	void operator()(const Message& msg)
	{
		label->Draw(msg.getMessage(), textimg, 2, msgy);
		msgy += label->getHeight()+1;
	}
	RA_Label* label;
	SDL_Surface* textimg;
	Uint32& msgy;
};

#endif //DRAWMESSAGE_H
