#pragma once

#include "core/utility/sdl.h"

#include <map>

namespace eversim {
	namespace core {
		namespace input {
			class GamepadHandler {
			private:
				std::map<int,SDL_Joystick*> joysticks;
			public:
				void handle_event(SDL_Event& event);

				const std::map<int, SDL_Joystick*>& get_joysticks() const
				{
					return joysticks;
				}
			};
		}
	}
}