#pragma once

/*
* Enums for every possible rawinput
*/

namespace eversim { namespace core { namespace system {

	namespace RawInputConstants
	{
		enum Button
		{
			GAMEPAD_BUTTON_A = 0,
			GAMEPAD_BUTTON_B,
			GAMEPAD_BUTTON_X,
			GAMEPAD_BUTTON_Y,
			GAMEPAD_BUTON_LB,
			GAMEPAD_BUTTON_RB,
			GAMEPAD_BUTTON_START,
			GAMEPAD_BUTTON_SELECT
		};

		enum Axis
		{
			GAMEPAD_ANALOGUE_LEFT_STICK_X = 0,
			GAMEPAD_ANALOGUE_LEFT_STICK_Y,
			GAMEPAD_ANALOGUE_RIGHT_STICK_X,
			GAMEPAD_ANALOGUE_RIGHT_STICK_Y,
			GAMEPAD_ANALOGUE_LT,
			GAMEPAD_ANALOGUE_RT
		};
	}

}}}