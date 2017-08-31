#include "core/input/inputevent.h"
#include <easylogging++.h>

#include <algorithm>

using namespace std;

namespace eversim {
	namespace core {
		namespace input {


			vector<RawInputConstants::event_type> InputEvent::dpad_states =
				vector<RawInputConstants::event_type>(4, RawInputConstants::event_type::BUTTON_UP);

			const map < size_t, RawInputConstants::input> InputEvent::dpad_mapping = 
			{
				{0,RawInputConstants::input::GAMEPAD_BUTTON_DPAD_LEFT},
				{1,RawInputConstants::input::GAMEPAD_BUTTON_DPAD_UP},
				{2,RawInputConstants::input::GAMEPAD_BUTTON_DPAD_RIGHT},
				{3,RawInputConstants::input::GAMEPAD_BUTTON_DPAD_DOWN}
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
				RawInputConstants::input button,
				bool down)
			{
				InputEvent event;
				event.type = (down) ? RawInputConstants::event_type::BUTTON_DOWN : RawInputConstants::event_type::BUTTON_UP;
				event.input_enum = button;
				return event;
			}

			InputEvent InputEvent::create_range(
				RawInputConstants::input range,
				glm::vec2 value)
			{
				InputEvent event;
				event.type = RawInputConstants::event_type::RANGE;
				event.input_enum = range;
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

							res.push_back
							(
								create_button(dpad_mapping.at(bidx),  true)
							);
						}
					}

					for (auto bidx : not_pressed) {
						auto& state = dpad_states.at(bidx);

						if (state == +RawInputConstants::event_type::BUTTON_DOWN) {
							state = RawInputConstants::event_type::BUTTON_UP;

							res.push_back
							(
								create_button(dpad_mapping.at(bidx), false)
							);
						}
					}

				}
				else {
					const auto it = RawInputConstants::sdl_event_map.find(static_cast<SDL_EventType>(sdl_event.type));

					if (it != RawInputConstants::sdl_event_map.end()
						&&
						it->second != +RawInputConstants::event_type::INVALID)
					{
						if (sdl_event.type == SDL_JOYBUTTONDOWN ||
							sdl_event.type == SDL_JOYBUTTONUP ||
							sdl_event.type == SDL_CONTROLLERBUTTONDOWN ||
							sdl_event.type == SDL_CONTROLLERBUTTONUP)
						{
							//gamepadbuttoninput
							auto sdl_button = static_cast<SDL_GameControllerButton>(sdl_event.cbutton.button);

							auto bit = RawInputConstants::sdl_button_map.find(sdl_button);

							if (bit != RawInputConstants::sdl_button_map.end()) {
								res.push_back(
									create_button(
										bit->second, 
										(sdl_event.type == SDL_JOYBUTTONDOWN || sdl_event.type == SDL_CONTROLLERBUTTONDOWN)
									)
								);
							}
						}
						else if (sdl_event.type == SDL_JOYAXISMOTION) {
							auto sdl_range = static_cast<SDL_GameControllerAxis>(sdl_event.jaxis.axis);
							auto rit = RawInputConstants::sdl_range_map.find(sdl_range);
							

							auto value = static_cast<double>(
								(static_cast<signed int>(sdl_event.jaxis.value) + 32768)) 
								/ (32768.+32767.);
							value = 2.*value - 1.;

							if (rit != RawInputConstants::sdl_range_map.end()) {
								res.push_back(
									create_range(
										rit->second,
										{ value, 0. }
									)
								);
							}

						} 
						else if (sdl_event.type == SDL_MOUSEBUTTONDOWN ||
							sdl_event.type == SDL_MOUSEBUTTONUP) {
							auto mit = RawInputConstants::sdl_mouserange_map.find(sdl_event.button.button);
							if ( mit != RawInputConstants::sdl_mouserange_map.end())
							{
								res.push_back(create_range(
									mit->second,
									{ sdl_event.button.x,sdl_event.button.y }
								));
							}

						}
						else if (sdl_event.type == SDL_MOUSEMOTION) {
							res.push_back(create_range(
								RawInputConstants::input::MOUSE,
								{ sdl_event.motion.x,
								sdl_event.motion.y }
							));
						}
					}
				}

				return res;
			}

			RawInputConstants::event_type InputEvent::get_event_type() const
			{
				return type;
			}

			RawInputConstants::input  InputEvent::get_input_enum() const
			{
				return input_enum;
			}

			void InputEvent::print(std::ostream& out) const
			{
				const InputEvent& event = *this;
				switch (event.get_event_type()) {
				case RawInputConstants::event_type::INVALID:
					out << "Event: INVALID";
					break;
				case RawInputConstants::event_type::RANGE:
					out << "Event: " << std::string(event.get_event_type()._to_string()) << " " <<
						std::string(event.get_input_enum()._to_string()) << " " 
						<< event.get_range_value().x << " " << event.get_range_value().y;
					break;
				default:
					out << "Event: " << std::string(event.get_event_type()._to_string()) << " " <<
						std::string(event.get_input_enum()._to_string());
					break;
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
