#include "ActionEvent.h"

#include "SDL/SDL_types.h"

void ActionEvent::stop() {
}

ActionEvent::ActionEvent(Uint32 p) {
	delay = p;
}

void ActionEvent::addCurtick(Uint32 curtick) {
	prio = delay + curtick;
}

void ActionEvent::run() { }

ActionEvent::~ActionEvent() {
}

void ActionEvent::setDelay(Uint32 p) {
	delay = p;
}

Uint32 ActionEvent::getPrio() {
	return prio;
}
