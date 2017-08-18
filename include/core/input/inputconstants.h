#pragma once


#include "enum.h"
/*
 * Enums for every possible action in every context
 */

namespace eversim {	namespace core { namespace input {

	namespace InputConstants
	{
		BETTER_ENUM(type, uint8_t,
			button = 0, state, range
		)

		BETTER_ENUM(button, uint8_t,
			CONFIRM = 0,
			BACK,
			JUMP,
			MENU,
			FART_LEFT,
			FART_RIGHT
		)

		BETTER_ENUM(state, uint8_t,
			DUCK = 0
		)

		BETTER_ENUM(range, uint8_t,
			STEER_X = 0,
			STEER_Y
		)
	}

}}}