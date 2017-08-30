#pragma once


#include "enum.h"
/*
 * Enums for every possible action in every context
 */

namespace eversim {	namespace core { namespace input {

	namespace InputConstants
	{
		//merge
		BETTER_ENUM(action, uint8_t,
			CONFIRM = 0,
			BACK,
			JUMP,
			DOUBLEJUMP,
			MENU,
			FART_LEFT,
			FART_RIGHT,
			DLEFT,
			DRIGHT,
			DUP,
			DDOWN,
			DROP,
			DUCK,
			GOLEM,
			STEER_X,
			STEER_Y,
			INVALID
		)

		BETTER_ENUM(input_type, uint8_t,
			BUTTON = 0,
			STATE,
			RANGE,
			INVALID
		)
	}

}}}