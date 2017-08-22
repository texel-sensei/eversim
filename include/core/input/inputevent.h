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
				static const std::map < size_t, RawInputConstants::button> dpad_mapping;

				RawInputConstants::event_type type = RawInputConstants::event_type::INVALID;

				RawInputConstants::raw_type button_or_range = RawInputConstants::raw_type::INVALID;

				RawInputConstants::button button = RawInputConstants::button::INVALID;
				RawInputConstants::range range = RawInputConstants::range::INVALID;

				double range_value = 0.0;

			public:

				RawInputConstants::event_type get_event_type() const;
				RawInputConstants::raw_type get_raw_type() const;

				RawInputConstants::button get_button() const;
				RawInputConstants::range get_range() const;

				double get_range_value() const { return range_value; }

				static std::vector<InputEvent> map_event(const SDL_Event&);
				static InputEvent create_button(
					RawInputConstants::event_type type
					, RawInputConstants::button button);
				static InputEvent create_range(
					RawInputConstants::event_type type,
					RawInputConstants::range range, double value);

				void print(std::ostream& out) const;

		};

		std::ostream& operator<<(std::ostream& out, const InputEvent& event);

		}
	}
}
