#pragma once

#include "core/input/rawinputconstants.h"
#include "core/input/inputconstants.h"

#include <SDL2/SDL.h>
#include <easylogging++.h>

#include <vector>
#include <map>
#include <iostream>
#include <string>

namespace eversim {
	namespace core {
		namespace input {
			class InputEvent
			{
			protected:

				static std::vector<RawInputConstants::event_type> dpad_states;
				static const std::map<size_t, std::vector<size_t>> dpad_combinations;
				static const std::map < size_t, RawInputConstants::input> dpad_mapping;

				RawInputConstants::event_type type = RawInputConstants::event_type::INVALID;
			
				RawInputConstants::input input_enum = RawInputConstants::input::INVALID;

				double range_value = 0.0;

			public:

				RawInputConstants::event_type get_event_type() const;

				RawInputConstants::input get_input_enum() const;

				double get_range_value() const { return range_value; }

				static std::vector<InputEvent> map_event(const SDL_Event&);
				static InputEvent create_button(RawInputConstants::input,bool);
				static InputEvent create_range(RawInputConstants::input,double);

				void print(std::ostream& out) const;

		};

		std::ostream& operator<<(std::ostream& out, const InputEvent& event);

		}
	}
}
