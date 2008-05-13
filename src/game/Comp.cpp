#include "Comp.h"
#include "ActionEvent.h"

bool Comp::operator() (ActionEvent * x, ActionEvent * y) {
    return x->prio > y->prio;
}
