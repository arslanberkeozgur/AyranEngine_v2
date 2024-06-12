#pragma once

enum class Material
{
	STANDARD,
	EMERALD,
	JADE,
	OBSIDIAN,
	PEARL,
	RUBY,
	TURQUOISE,
	BRASS,
	CHROME
};

enum class ActionType
{
	NO_TYPE,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	STRAFE_RIGHT,
	STRAFE_LEFT,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_FORWARD1,
	MOVE_BACKWARD1,
	STRAFE_RIGHT1,
	STRAFE_LEFT1,
	MOVE_UP1,
	MOVE_DOWN1,
	TOGGLE_FLASHLIGHT,
	RUN,
	ROTATE_X,
	ROTATE_Y,
	ROTATE_Z,
	USE
};

enum class ActionEventType
{
	NO_TYPE,
	BEGIN,
	CONTINUE,
	END
};

enum class ShaderType
{
	DEFAULT,
	LIGHT_SOURCE,
	OUTLINE
};