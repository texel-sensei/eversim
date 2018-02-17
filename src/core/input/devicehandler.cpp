#include "core/input/devicehandler.h"

#include <easylogging++.h>

using namespace std;

namespace eversim {
	namespace core {
		namespace input {

			void DeviceHandler::handle_event(SDL_Event& event)
			{

				if (event.type == SDL_JOYDEVICEADDED) {
					
					const auto jidx = event.jdevice.which;
					devices[jidx] = { jidx,device_type::GAMEPAD,SDL_JoystickOpen(jidx) };
					LOG(INFO) << "Added Joystick " << jidx << ". #Joysticks = " << devices.size();

				}
				else if (event.type == SDL_JOYDEVICEREMOVED) {
					
					const auto jidx = event.jdevice.which;
					auto it = find_if(begin(devices), end(devices),
						[&](const auto& device) 
					{
						return device.second.id == jidx;
					}
					);
					if (it != end(devices)) {

						if (it->second.type == +device_type::GAMEPAD) {
							SDL_Joystick* ptr = std::any_cast<SDL_Joystick*>(it->second.handle);
							SDL_JoystickClose(ptr);
							devices.erase(it);
							LOG(INFO) << "Removed Joystick " << jidx;
						}
					}
					
				}
			}

		}

	}
}