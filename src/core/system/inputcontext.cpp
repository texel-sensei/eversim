#include "core/system/inputcontext.h"

#include <easylogging++.h>

namespace eversim {	namespace core { namespace system {

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
			LOG(INFO) << "\t\t" << RawInputConstants::button::_from_integral(button.first)._to_string() << 
				" -> " << InputConstants::button::_from_integral(button.second)._to_string();
		}

		for (auto& state : states)
		{
			LOG(INFO) << "\t\t" << RawInputConstants::button::_from_integral(state.first)._to_string() <<
				" -> " << InputConstants::state::_from_integral(state.second)._to_string();
		}

		for (auto& range : ranges)
		{
			LOG(INFO) << "\t\t" << RawInputConstants::range::_from_integral(range.first)._to_string() <<
				" -> " << InputConstants::range::_from_integral(range.second)._to_string();
		}
	}
}}}