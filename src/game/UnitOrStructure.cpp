
#include "UnitOrStructure.h"
#include "include/Logger.h"

extern Logger * logger;

void UnitOrStructure::remove() {
	deleted = true;
	if (references == 0) {
		delete this;
	}
}
bool UnitOrStructure::isAlive() {
	return !deleted;
}
void UnitOrStructure::select() {
	selected = true;
	showorder_timer = 0;
}
void UnitOrStructure::unSelect() {
	selected = false;
	showorder_timer = 0;
}
bool UnitOrStructure::isSelected() {
	return selected;
}
void UnitOrStructure::unrefer() {
	//    assert(references > 0);
	if (references > 0) {
		--references;
		if (deleted && references == 0) {
			delete this;
		}
	} else {
		logger->error("%s line %i: Unrefer while not refered \n",__FILE__ , __LINE__);
	}
}

UnitOrStructure::~UnitOrStructure() {
	//    assert(references == 0);
	if (references != 0) {
		logger->error("%s line %i: References is not 0 \n",__FILE__ , __LINE__);
	}
}

UnitOrStructure::UnitOrStructure() :
	references(0), deleted(false), selected(false), targetcell(0),
			target(NULL), showorder_timer(0) {
}
UnitOrStructure * UnitOrStructure::getTarget() {return target;}
void UnitOrStructure::setYoffset(Sint8 yo) {}
void UnitOrStructure::setXoffset(Sint8 xo) {}
void UnitOrStructure::referTo() {++references;}
Uint32 UnitOrStructure::getExitCell() const {return 0;}
Uint16 UnitOrStructure::getTargetCell() const {return targetcell;}
