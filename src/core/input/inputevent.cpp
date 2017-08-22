#include "core/input/inputevent.h"
#include <easylogging++.h>

#include <algorithm>

using namespace std;

namespace eversim {
	namespace core {
		namespace input {


			vector<RawInputConstants::event_type> InputEvent::dpad_states =
				vector<RawInputConstants::event_type>(4, RawInputConstants::event_type::BUTTON_UP);

			const map < size_t, RawInputConstants::button> InputEvent::dpad_mapping = 
			{
				{0,RawInputConstants::button::GAMEPAD_BUTTON_DPAD_LEFT},
				{1,RawInputConstants::button::GAMEPAD_BUTTON_DPAD_UP},
				{2,RawInputConstants::button::GAMEPAD_BUTTON_DPAD_RIGHT},
				{3,RawInputConstants::button::GAMEPAD_BUTTON_DPAD_DOWN}
			};

			const map<size_t,vector<size_t>> InputEvent::dpad_combinations =
			{
				{ static_cast<size_t>(SDL_HAT_LEFTUP),{0,1}}, //SDL_HAT_LEFTUP
				{ static_cast<size_t>(SDL_HAT_UP),{1}}, //SDL_HAT_UP
				{ static_cast<size_t>(SDL_HAT_RIGHTUP),{1,2}}, //SDL_HAT_RIGHTUP
				{ static_cast<size_t>(SDL_HAT_LEFT),{0}}, //SDL_HAT_LEFT
				{ static_cast<size_t>(SDL_HAT_CENTERED),{}}, //SDL_HAT_CENTERED
				{ static_cast<size_t>(SDL_HAT_RIGHT),{2}}, //SDL_HAT_RIGHT
				{ static_cast<size_t>(SDL_HAT_LEFTDOWN),{0,3}}, //SDL_HAT_LEFTDOWN
				{ static_cast<size_t>(SDL_HAT_DOWN),{3}}, //SDL_HAT_DOWN
				{ static_cast<size_t>(SDL_HAT_RIGHTDOWN),{2,3}} //SDL_HAT_RIGHTDOWN
			};

			InputEvent InputEvent::create_button(
				RawInputConstants::event_type type, 
				RawInputConstants::button button)
			{
				InputEvent event;
				event.type = type;
				event.button_or_range = RawInputConstants::raw_type::BUTTON;
				event.button = button;
				return event;
			}

			InputEvent InputEvent::create_range(
				RawInputConstants::event_type type,
				RawInputConstants::range range, 
				double value)
			{
				InputEvent event;
				event.type = type;
				event.button_or_range = RawInputConstants::raw_type::RANGE;
				event.range = range;
				event.range_value = value;
				return event;
			}
				
			std::vector<InputEvent> InputEvent::map_event(const SDL_Event& sdl_event)
			{
				std::vector<InputEvent> res;
				if (sdl_event.type == SDL_JOYHATMOTION)
				{
					auto jhat_value = static_cast<size_t>(sdl_event.jhat.value);

					auto pressed = dpad_combinations.at(jhat_value);

					vector<size_t> not_pressed = {					
						0,1,2,3
					};

					auto end_it = remove_if(begin(not_pressed), end(not_pressed),
						[&](const size_t i)
					{
						for (auto state : pressed) {
							if (state == i) return true;
						}
						return false;
					});

					not_pressed = vector<size_t>(begin(not_pressed),end_it);

					for (auto bidx : pressed) {
						auto& state = dpad_states.at(bidx);

						if (state == +RawInputConstants::event_type::BUTTON_UP) {
							state = RawInputConstants::event_type::BUTTON_DOWN;
							InputEvent event;
							event.type = state;
							event.button_or_range = RawInputConstants::raw_type::BUTTON;
							event.button = dpad_mapping.at(bidx);
							res.push_back(event);
						}
					}

					for (auto bidx : not_pressed) {
						auto& state = dpad_states.at(bidx);

						if (state == +RawInputConstants::event_type::BUTTON_DOWN) {
							state = RawInputConstants::event_type::BUTTON_UP;
							InputEvent event;
							event.type = state;
							event.button_or_range = RawInputConstants::raw_type::BUTTON;
							event.button = dpad_mapping.at(bidx);
							res.push_back(event);
						}
					}

				}
				else {
					InputEvent event;
					const auto it = RawInputConstants::sdl_event_map.find(static_cast<SDL_EventType>(sdl_event.type));
					if (it != RawInputConstants::sdl_event_map.end())
					{
						event.type = it->second;
					}

					if (event.type != +RawInputConstants::event_type::INVALID)
					{
						if (sdl_event.type == SDL_JOYBUTTONDOWN ||
							sdl_event.type == SDL_JOYBUTTONUP ||
							sdl_event.type == SDL_CONTROLLERBUTTONDOWN ||
							sdl_event.type == SDL_CONTROLLERBUTTONUP)
						{
							//gamepadbuttoninput
							event.button_or_range = RawInputConstants::raw_type::BUTTON;
							auto sdl_button = static_cast<SDL_GameControllerButton>(sdl_event.cbutton.button);

							auto bit = RawInputConstants::sdl_button_map.find(sdl_button);
							if (bit != RawInputConstants::sdl_button_map.end()) {
								event.button = bit->second;
								res.push_back(event);
								//LOG(INFO) << RawInputConstants::button::_from_integral(event.button)._to_string();

							}
						}
						else if (sdl_event.type == SDL_JOYAXISMOTION) {
							event.button_or_range = RawInputConstants::raw_type::RANGE;
							event.range = RawInputConstants::sdl_range_map.at(static_cast<SDL_GameControllerAxis>(sdl_event.jaxis.axis));
							event.range_value = static_cast<double>(
								(static_cast<signed int>(sdl_event.jaxis.value) + 32768)) 
								/ (32768.+32767.);
							event.range_value *= 2.;
							event.range_value -= 1.;
							res.push_back(event);
						}
					}
				}

				return res;
			}

			RawInputConstants::event_type InputEvent::get_event_type() const
			{
				return type;
			}

			RawInputConstants::raw_type  InputEvent::get_raw_type() const
			{
				return button_or_range;
			}

			RawInputConstants::button  InputEvent::get_button() const
			{
				return button;
			}

			RawInputConstants::range  InputEvent::get_range() const
			{
				return range;
			}

			void InputEvent::print(std::ostream& out) const
			{
				const InputEvent& event = *this;
				switch (event.get_event_type()) {
				case RawInputConstants::event_type::INVALID:
					out << "Event: INVALID";
					break;
				case RawInputConstants::event_type::BUTTON_DOWN:
					out << "Event: " << std::string(event.get_event_type()._to_string()) << " " <<
						std::string(event.get_button()._to_string());
					break;
				case RawInputConstants::event_type::BUTTON_UP:
					out << "Event: " << std::string(event.get_event_type()._to_string()) << " " <<
						std::string(event.get_button()._to_string());
					break;
				case RawInputConstants::event_type::AXIS:
					out << "Event: " << std::string(event.get_event_type()._to_string()) << " " <<
						std::string(event.get_range()._to_string()) << " " << std::to_string(event.get_range_value());
					break;
				default: break;
				}
			}

			std::ostream& operator<<(std::ostream& out,const InputEvent& event)
			{
				event.print(out);
				return out;
			}
		}
	}
}
