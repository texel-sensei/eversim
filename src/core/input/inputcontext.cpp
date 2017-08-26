#include "core/input/inputcontext.h"

#include <easylogging++.h>

namespace eversim {	namespace core { namespace input {

	InputContext::InputContext(const std::string& n) : name(n)
	{
		//LOG(INFO) << "create InputContext \"" << name << "\""; 
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
		auto type_enum = InputConstants::input_type::_from_string(type.c_str());

		//LOG(INFO) << type << " " << action << " " << rawcode;

		switch(type_enum)
		{
		case InputConstants::input_type::BUTTON :
		{
			//LOG(INFO) << "\tbutton";
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::button::_from_string(action.c_str());

			if (action_enum == +InputConstants::button::DROP)
			{
				buttons_to_drop.push_back(raw_enum);
				break;
			}

			auto it = buttons.find(raw_enum);

			if (it == buttons.end())
				buttons[raw_enum] = {};

			buttons[raw_enum].emplace(action_enum);
		}
			break;
		case InputConstants::input_type::STATE :
		{
			//LOG(INFO) << "\tstate";
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::state::_from_string(action.c_str());
			
			if (action_enum == +InputConstants::state::DROP)
			{
				buttons_to_drop.push_back(raw_enum);
				break;
			}

			auto it = states.find(raw_enum);

			if (it == states.end())
				states[raw_enum] = {};

			states[raw_enum].emplace(action_enum);
		}
			break;
		case InputConstants::input_type::RANGE :
		{
			//LOG(INFO) << "\trange";
			const auto raw_enum = RawInputConstants::range::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::range::_from_string(action.c_str());
	
			if (action_enum == +InputConstants::range::DROP)
			{
				ranges_to_drop.push_back(raw_enum);
				break;
			}

			auto it = ranges.find(raw_enum);

			if (it == ranges.end())
				ranges[raw_enum] = {};

			ranges[raw_enum].emplace(action_enum);
		}
			break;
		default:
			//TODO
			break;
		}
	}

	InputConstants::input_type InputContext::get_input_type(const RawInputConstants::button& button) const
	{
		if (buttons.find(button) != buttons.end()) return InputConstants::input_type::BUTTON;
		if(states.find(button) != states.end()) return InputConstants::input_type::STATE;
		return InputConstants::input_type::INVALID;
	}

	InputConstants::input_type InputContext::get_input_type(const RawInputConstants::range& range) const
	{
		if (ranges.find(range) != ranges.end()) return InputConstants::input_type::RANGE;
		return InputConstants::input_type::INVALID;
	}

	bool InputContext::handle_event(const InputEvent& event)
	{

		bool handeled = false;

		if (event.get_raw_type() == +RawInputConstants::raw_type::INVALID) return handeled;

		{
			//find in buttons
			auto button = event.get_button();

			if (find(begin(buttons_to_drop), end(buttons_to_drop), button) != buttons_to_drop.end()) return true;

			auto action_it = buttons.find(button);
			if (action_it != buttons.end()) {
				handeled = true;
				for (auto& action : action_it->second)
				{
					button_states[action] = (event.get_event_type() == +RawInputConstants::event_type::BUTTON_DOWN);
				}
			}
		}
		{
			//find in states
			auto state = event.get_button();

			if (find(begin(buttons_to_drop), end(buttons_to_drop), state) != buttons_to_drop.end()) return true;

			auto action_it = states.find(state);
			if (action_it != states.end()) {
				handeled = true;
				for (auto& action : action_it->second)
				{
					state_states[action] = (event.get_event_type() == +RawInputConstants::event_type::BUTTON_DOWN);
				}
			}
		}
		{
			//find in ranges
			auto range = event.get_range();

			if (find(begin(ranges_to_drop),end(ranges_to_drop),range) != ranges_to_drop.end()) return true;

			auto action_it = ranges.find(range);
			if (action_it != ranges.end()) {
				handeled = true;
				for (auto& action : action_it->second)
				{
					range_states[action] = event.get_range_value();
				}
			}
		}

		return handeled;
	}

	void InputContext::register_function(const InputConstants::button button,
		button_function f)
	{
		button_functions[button] = f;
	}

	void InputContext::register_function(const InputConstants::state state, 
		button_function f)
	{
		state_functions[state] = f;
	}

	void InputContext::register_function(const InputConstants::range range,
		range_function f)
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
					it->second(*this);
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
					it->second(*this);
				}
			}
		}

		for (auto& s : range_states)
		{
			auto it = range_functions.find(s.first);
			if (it != range_functions.end())
			{
				it->second(*this,s.second);
			}
		}
	}

	void InputContext::list_actions() const
	{
		LOG(INFO) << "Context \"" << name << "\"";
		for(auto& button : buttons)
		{
			LOG(INFO) << "\tBUTTON " << RawInputConstants::button::_from_integral(button.first)._to_string();
			for (auto& action : button.second)
			{
				LOG(INFO) << "\t\t" << InputConstants::button::_from_integral(action)._to_string();
			}
		}

		for (auto& state : states)
		{
			LOG(INFO) << "\tSTATE " << RawInputConstants::button::_from_integral(state.first)._to_string();
			for (auto& action : state.second)
			{
				LOG(INFO) << "\t\t" << InputConstants::state::_from_integral(action)._to_string();
			}
		}

		for (auto& range : ranges)
		{
			LOG(INFO) << "\tRANGE " << RawInputConstants::range::_from_integral(range.first)._to_string();
			for (auto& action : range.second)
			{
				LOG(INFO) << "\t\t" << InputConstants::range::_from_integral(action)._to_string();
			}
		}
	}
}}}