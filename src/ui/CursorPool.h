// CursorPool.h
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

#ifndef CURSORPOOL_H
#define CURSORPOOL_H

#include <string>
#include <map>
#include <vector>

#include "SDL/SDL_types.h"

using std::string;
using std::map;
using std::vector;

class CursorInfo;

/**
 * Red Alert

 Name            Frame(s)
 ----            --------
 normal          0
 scroll north    1
 . . .
 scroll ne       8
 empty no        9
 move            10->13
 nomove          14
 select          15->20
 attack(!in rng) 21->28
 mini-move       29->32
 mini blank no   33
 blank           34
 spanner         35->58
 deploy          59->67
 sell            68->79
 minicursor      80
 miniscrollbox   81
 deploysuper     82->89
 nuke            90->96
 chrono-select   97->104
 chrono-target   105->112
 green-enter     113->115
 C4              116->118
 no-sell         119
 no-fix          120
 mini C4         121->123
 noscrollnorth   124
 . . .
 noscrollne      131
 flashinglight   132->133
 mini atk !rng   134->141
 minigreen enter 142->144
 miniminiscroll  145
 miniguard       146
 guard           147
 green sell      148->159
 heal            160->163
 red enter       164->166
 mini red enter  167->169
 gold spanner    170->194
 mini heal       194
 attack          195->202
 mini attack     203->210
 no deploy       211
 no enter        212
 no gold fix     213
 mini deploysupr 214-221
 */
class CursorPool
{
public:
    CursorPool();
    ~CursorPool();
    CursorInfo* getCursorByName(string name);

private:
    vector<CursorInfo*> cursorpool;
    map<string, Uint16> name2index;
};

#endif //CURSORPOOL_H
