#pragma once

/*
 * Enums for every possible action in every context
 */

namespace eversim {	namespace core { namespace system {

	namespace InputConstants
	{
		enum ACTION
		{
			CONFIRM = 0,
			ABORT,
			JUMP
		};

		enum STATE
		{
			DUCK = 0
		};

		enum RANGE
		{
			STEER_PLAYER = 0
		};
	}

}}}