#pragma once

#include "Enums.h"

struct Action
{
	Action_Type			type		= Action_Type::NO_TYPE;
	Action_Event_Type	eventType   = Action_Event_Type::NO_TYPE;

	Action(Action_Type Type, Action_Event_Type EventType) : type{ Type }, eventType{ EventType } {}
	Action() {}
};