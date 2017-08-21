#pragma once

#include "core/input/rawinputconstants.h"
#include "core/input/inputconstants.h"

#include <SDL2/SDL.h>

#include <vector>
#include <map>

namespace eversim {
	namespace core {
		namespace input {
			class InputEvent
			{
			protected:

				static std::vector<RawInputConstants::event_type> dpad_states;
				static const std::map<size_t,std::vector<size_t>> dpad_combinations;
				static const std::map < size_t, RawInputConstants::button> dpad_mapping;

				RawInputConstants::event_type type = RawInputConstants::event_type::INVALID;

				RawInputConstants::raw_type button_or_range = RawInputConstants::raw_type::INVALID;

				RawInputConstants::button button = RawInputConstants::button::INVALID;
				RawInputConstants::range range = RawInputConstants::range::INVALID;

			public:
				
				RawInputConstants::event_type get_event_type() const;
				RawInputConstants::raw_type get_raw_type() const;

				RawInputConstants::button get_button() const;
				RawInputConstants::range get_range() const;

				static std::vector<InputEvent> map_event(const SDL_Event&);

				
			};
		}
	}
}
