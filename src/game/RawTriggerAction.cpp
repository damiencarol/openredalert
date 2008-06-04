#include "RawTriggerAction.h"

RawTriggerAction::RawTriggerAction(int action, int param1, int param2, int param3) :
	TriggerAction(action)
{
	this->param1 = param1;
	this->param2 = param2;
	this->param3 = param3;
}

/** Execute the action */
void RawTriggerAction::execute()
{
}

int RawTriggerAction::getParam1()
{
	return param1;
}

int RawTriggerAction::getParam2()
{
	return param2;
}

int RawTriggerAction::getParam3()
{
	return param3;
}

