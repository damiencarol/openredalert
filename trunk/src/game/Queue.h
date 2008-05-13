#ifndef QUEUE_H
#define QUEUE_H

#include <list>

class UnitOrStructureType;

using std::list;

class Queue : public list<const UnitOrStructureType*>
{
};

#endif //QUEUE_H
