#pragma once

#include "core/utility/sdl.h"
#include "core/input/inputdevice.h"

#include <map>

namespace eversim {
	namespace core {
		namespace input {
			class DeviceHandler {
			private:
				std::map<int,InputDevice> devices;
			public:
				void handle_event(SDL_Event& event);

				const std::map<int, InputDevice>& get_devices() const
				{
					return devices;
				}
			};
		}
	}
}