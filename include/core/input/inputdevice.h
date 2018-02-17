#pragma once

#include "enum.h"

#include <any>

namespace eversim::core::input {
	
	BETTER_ENUM(device_type, uint8_t,
		GAMEPAD = 0,
		KEYBOARD,
		INVALID
	)
	
	struct InputDevice {
		int id;
		device_type type = device_type::INVALID;
		std::any handle;
	};
}