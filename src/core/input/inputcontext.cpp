#include "core/input/inputcontext.h"

#include <easylogging++.h>

namespace eversim {	namespace core { namespace input {

	InputContext::InputContext(const std::string& n) : name(n)
	{
		LOG(INFO) << "create InputContext \"" << name << "\""; 
	}

	void InputContext::register_action(const std::string& type, const std::string& action, const std::string& rawcode)
	{
		auto type_enum = InputConstants::type::_from_string(type.c_str());

		switch(type_enum)
		{
		case InputConstants::type::button :
		{
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::button::_from_string(action.c_str());

			buttons[static_cast<uint8_t>(raw_enum)] = static_cast<uint8_t>(action_enum);
		}
			break;
		case InputConstants::type::state :
		{
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::state::_from_string(action.c_str());
			
			states[static_cast<uint8_t>(raw_enum)] = static_cast<uint8_t>(action_enum);
		}
			break;
		case InputConstants::type::range :
		{
			const auto raw_enum = RawInputConstants::range::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::range::_from_string(action.c_str());
			
			ranges[static_cast<uint8_t>(raw_enum)] = static_cast<uint8_t>(action_enum);
		}
			break;
		default:
			//TODO
			break;
		}
	}

	void InputContext::list_actions() const
	{
		LOG(INFO) << "Context \"" << name << "\"";
		for(auto& button : buttons)
		{
			const auto raw_enum = RawInputConstants::button::_from_integral(button.first);
			auto it = RawInputConstants::sdl_button_map.end();

			for(it = RawInputConstants::sdl_button_map.begin(); it != RawInputConstants::sdl_button_map.end(); ++it)
			{
				if(it->second == raw_enum)
				{
					break;
				}
			}
	
			if(it == RawInputConstants::sdl_button_map.end())
			{
				LOG(ERROR) << "sdl to raw mapping not found";
				continue;
			}
			LOG(INFO) << "\t\t" << raw_enum._to_string() << " / " << it->first << 
				" -> " << InputConstants::button::_from_integral(button.second)._to_string();
		}

		for (auto& state : states)
		{
			const auto raw_enum = RawInputConstants::button::_from_integral(state.first);
			auto it = RawInputConstants::sdl_button_map.end();

			for (it = RawInputConstants::sdl_button_map.begin(); it != RawInputConstants::sdl_button_map.end(); ++it)
			{
				if (it->second == raw_enum)
				{
					break;
				}
			}

			if (it == RawInputConstants::sdl_button_map.end())
			{
				LOG(ERROR) << "sdl to raw mapping not found";
				continue;
			}

			LOG(INFO) << "\t\t" << raw_enum._to_string() << " / " << it->first <<
				" -> " << InputConstants::state::_from_integral(state.second)._to_string();
		}

		for (auto& range : ranges)
		{
			const auto raw_enum = RawInputConstants::button::_from_integral(range.first);
			auto it = RawInputConstants::sdl_range_map.end();

			for (it = RawInputConstants::sdl_range_map.begin(); it != RawInputConstants::sdl_range_map.end(); ++it)
			{
				if (it->second == raw_enum)
				{
					break;
				}
			}

			if (it == RawInputConstants::sdl_range_map.end())
			{
				LOG(ERROR) << "sdl to raw mapping not found";
				continue;
			}

			LOG(INFO) << "\t\t" << raw_enum._to_string() << " / " << it->first <<
				" -> " << InputConstants::range::_from_integral(range.second)._to_string();
		}
	}
}}}