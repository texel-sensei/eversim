#pragma once

#include "core/input/rawinputconstants.h"
#include "core/input/inputconstants.h"

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
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

				glm::vec2 range_value = { 0.,0. };

			public:

				bool operator==(const InputEvent& event) const 
					{ return get_input_enum() == event.get_input_enum(); }

				RawInputConstants::event_type get_event_type() const;

				RawInputConstants::input get_input_enum() const;

				glm::vec2 get_range_value() const { return range_value; }

				static std::vector<InputEvent> map_event(const SDL_Event&);
				static InputEvent create_button(RawInputConstants::input,bool);
				static InputEvent create_range(RawInputConstants::input, glm::vec2);

				void print(std::ostream& out) const;

		};

		std::ostream& operator<<(std::ostream& out, const InputEvent& event);

		}
	}
}
