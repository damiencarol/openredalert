#ifndef NOACTIONTRIGGERACTION_H
#define NOACTIONTRIGGERACTION_H

#include "TriggerAction.h"

/**
 * Action that do nothing. 
 */
class NoActionTriggerAction : public TriggerAction
{
public:    

    NoActionTriggerAction();

    void execute();
};
#endif //NOACTIONTRIGGERACTION_H
