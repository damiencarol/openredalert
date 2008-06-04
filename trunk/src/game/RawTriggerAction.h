
#ifndef RAWTRIGGERACTION_H
#define RAWTRIGGERACTION_H

#include "TriggerAction.h"

class RawTriggerAction : public TriggerAction
{
public:
	RawTriggerAction(int action, int param1, int param2, int param3);
	int getParam3();
	int getParam2();
	int getParam1();

	/** Execute the action */
	void execute();

private:


	int param1;
	int param2;
	int param3;
};

#endif //RAWTRIGGERACTION_H
