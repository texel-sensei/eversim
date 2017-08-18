#include "core/input/inputcontext.h"

#include <easylogging++.h>

namespace eversim {	namespace core { namespace input {

	InputContext::InputContext(const std::string& n) : name(n)
	{
		LOG(INFO) << "create InputContext \"" << name << "\""; 
	}

	InputContext& InputContext::operator=(const InputContext& other) noexcept
	{
		name = other.name;
		buttons = other.buttons;
		ranges = other.ranges;
		states = other.states;
		return *this;
	}

	void InputContext::register_action(const std::string& type, const std::string& action, const std::string& rawcode)
	{
		auto type_enum = InputConstants::type::_from_string(type.c_str());

		//LOG(INFO) << type << " " << action << " " << rawcode;

		switch(type_enum)
		{
		case InputConstants::type::button :
		{
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::button::_from_string(action.c_str());

			buttons[+raw_enum] = +action_enum;
		}
			break;
		case InputConstants::type::state :
		{
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::state::_from_string(action.c_str());
			
			states[+raw_enum] = +action_enum;
		}
			break;
		case InputConstants::type::range :
		{
			const auto raw_enum = RawInputConstants::range::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::range::_from_string(action.c_str());
			
			ranges[+raw_enum] = +action_enum;
		}
			break;
		default:
			//TODO
			break;
		}
	}

	bool InputContext::handle_event(SDL_Event& sdl_event)
	{
		switch(sdl_event.type)
		{
		case SDL_JOYBUTTONDOWN:
			{
				auto sdl_button = static_cast<SDL_GameControllerButton>(sdl_event.cbutton.button);

				auto it = RawInputConstants::sdl_button_map.find(sdl_button);
				if (it != RawInputConstants::sdl_button_map.end()) {
					auto button = it->second;
					LOG(INFO) << RawInputConstants::button::_from_integral(button)._to_string();
					{auto action_it = buttons.find(button);
					if (action_it != buttons.end())
					{
						auto action = buttons.at(button);
						LOG(INFO) << "\tbuttonaction " << InputConstants::button::_from_integral(action)._to_string();

						button_states[action] = true;
						return true;
					}}
					{auto action_it = states.find(button);
					if (action_it != states.end())
					{
						auto action = states.at(button);
						LOG(INFO) << "\tstateaction " << InputConstants::state::_from_integral(action)._to_string();

						state_states[action] = true;
						return true;
					}}
				}
			}
			break;
		case SDL_JOYBUTTONUP:

			break;
		default:break;
		}

		return false;
	}

	void InputContext::register_function(const InputConstants::button button,std::function<void()> f)
	{
		button_functions[button] = f;
	}

	void InputContext::register_function(const InputConstants::state state, std::function<void()> f)
	{
		state_functions[state] = f;
	}

	void InputContext::register_function(const InputConstants::range range, std::function<void()> f)
	{
		range_functions[range] = f;
	}

	void InputContext::execute()
	{

		for(auto& s : button_states)
		{
			if(s.second)
			{
				auto it = button_functions.find(s.first);
				if(it != button_functions.end())
				{
					it->second();
				}
			}
		}
		button_states.clear();

		for (auto& s : state_states)
		{
			if (s.second)
			{
				auto it = state_functions.find(s.first);
				if (it != state_functions.end())
				{
					it->second();
				}
			}
		}

		/*for(auto& p : button_functions)
		{
			p.second();
		}*/
		//button_functions.clear();
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
		LOG(INFO) << "END PRINT";
	}
}}}