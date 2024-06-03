#pragma once

#include "Enums.h"

struct Action
{
	ActionType			type		= ActionType::NO_TYPE;
	ActionEventType		eventType   = ActionEventType::NO_TYPE;

	Action(ActionType Type, ActionEventType EventType) : type{ Type }, eventType{ EventType } {}
	Action() {}
};