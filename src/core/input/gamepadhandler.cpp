#include "core/input/gamepadhandler.h"

#include <easylogging++.h>

using namespace std;

namespace eversim {
	namespace core {
		namespace input {

			void GamepadHandler::handle_event(SDL_Event& event)
			{

				if (event.type == SDL_JOYDEVICEADDED) {
					
					const auto jidx = event.jdevice.which;
					joysticks[jidx] = SDL_JoystickOpen(jidx);
					LOG(INFO) << "Added Joystick " << jidx << ". #Joysticks = " << joysticks.size();

				}
				else if (event.type == SDL_JOYDEVICEREMOVED) {
					
					const auto jidx = event.jdevice.which;
					auto it = find_if(begin(joysticks), end(joysticks),
						[&](const std::pair<const int,SDL_Joystick*>& joystick) 
					{
						return joystick.first == jidx;
					}
					);
					if (it != end(joysticks)) {
						SDL_Joystick* ptr = it->second;
						SDL_JoystickClose(ptr);
						joysticks.erase(it);
						LOG(INFO) << "Removed Joystick " << jidx;
					}
					
				}
			}

		}

	}
}