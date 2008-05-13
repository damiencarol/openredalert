#include "KeyComp.h"
#include "FibHeapEntry.h"

bool KeyComp::operator()(FibHeapEntry *x, FibHeapEntry *y) {
	return x->getKey()> y->getKey();
}

