#ifndef COMP_H
#define COMP_H

#include <queue>
#include <vector>
#include "ActionEvent.h"

//#include "renderer.h"
//#include "game/ActionEvent.h"
//#include "ActionEvent.h"


/** Friend class which compares ActionEvents priority */
class Comp
{
public:
    bool operator()(ActionEvent *x, ActionEvent *y)
    ;
};

#endif
