// CursorPool.cpp
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

#include "CursorPool.h"

#include <iterator>

#include "SDL/SDL_types.h"

#include "misc/INIFile.h"
#include "include/Logger.h"
#include "include/config.h"
#include "CursorInfo.h"

#include "misc/INIFile.h"

using std::string;

/**
 * Create a CursorPool with all cursor from RedAlert
 */
CursorPool::CursorPool() 
{
	Uint16 index = 0; // Index of the cursor info being created
	CursorInfo* data = 0; // Ref to the cursor being created
	string cname; // Name of the cursor being created

	// Populate the pool with RA cursors
	//	"standard" normal          0
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(0);
	data->setAnEnd(0);
	cname = "standard";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	scroll north    1
	//	. . .
	//	scroll ne       8
	
	//	"nomove" empty no        9
	
	//	"standard" normal          0
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(9);
	data->setAnEnd(9);
	cname = "nomove";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	"move" move            10->13
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(10);
	data->setAnEnd(13);
	cname = "move";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	nomove          14
	
	//	select          15->20
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(15);
	data->setAnEnd(20);
	cname = "select";
	cursorpool.push_back(data);
	name2index[cname] = index;

	
	//	attack(!in rng) 21->28
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(21);
	data->setAnEnd(28);
	cname = "attack";
	cursorpool.push_back(data);
	name2index[cname] = index;

	
	//	mini-move       29->32
	//	mini blank no   33
	//	blank           34
	
	//	"repair" spanner         35->58
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(35);
	data->setAnEnd(58);
	cname = "repair";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	deploy          59->67
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(59);
	data->setAnEnd(67);
	cname = "deploy";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	sell            68->79
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(68);
	data->setAnEnd(79);
	cname = "sell";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	minicursor      80
	//	miniscrollbox   81
	
	//	superdeploy     82->89
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(82);
	data->setAnEnd(89);
	cname = "superdeploy";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	nuke            90->96
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(90);
	data->setAnEnd(96);
	cname = "nuke";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	chrono-select   97->104
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(97);
	data->setAnEnd(104);
	cname = "chrono-select";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	 chrono-target   105->112
	
	//	 "enter" green-enter     113->115
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(113);
	data->setAnEnd(115);
	cname = "enter";
	cursorpool.push_back(data);
	name2index[cname] = index;
	
	//	 "bom" C4              116->118
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(116);
	data->setAnEnd(118);
	cname = "bom";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	 "nosell" no-sell         119
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(119);
	data->setAnEnd(119);
	cname = "nosell";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	 "norepair" no-fix          120
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(120);
	data->setAnEnd(120);
	cname = "norepair";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	 mini C4         121->123
	//	 noscrollnorth   124
	//	 . . .
	//	 noscrollne      131
	//	 flashinglight   132->133
	//	 mini atk !rng   134->141
	//	 minigreen enter 142->144
	//	 miniminiscroll  145
	//	 miniguard       146
	//	 guard           147
	//	 green sell      148->159
	
	//	 heal            160->163
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(160);
	data->setAnEnd(163);
	cname = "heal";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	 red enter       164->166
	//	 mini red enter  167->169
	//	 gold spanner    170->194
	//	 mini heal       194
	//	 attack          195->202
	//	 mini attack     203->210
	
	//	 no deploy       211
	index = static_cast<Uint16>(cursorpool.size());
	data = new CursorInfo();
	data->setAnStart(211);
	data->setAnEnd(211);
	cname = "nodeploy";
	cursorpool.push_back(data);
	name2index[cname] = index;

	//	 no enter        212
	//	 no gold fix     213
	//	 mini deploysupr 214-221
}

/**
 * Free each cursor in the list
 */
CursorPool::~CursorPool() 
{
	// Free all cursor info
	for (Uint32 i = 0; i < cursorpool.size(); ++i) {
		delete cursorpool[i];
	}
}

/**
 * Return a CursorInfo by this name
 */
CursorInfo* CursorPool::getCursorByName(string cname) 
{
	map<string, Uint16>::iterator cursorentry;
	CursorInfo* datum = 0; // Ref to the CursorInfo wanted
	Uint16 index; // Index of the cursor ref if found


	// Try to find the cursor in the list
	cursorentry = name2index.find(cname);

	// If found return it
	if (cursorentry != name2index.end()) {
		index = cursorentry->second;
		datum = cursorpool[index];
	}

	// Return result
	return datum;
}
