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

		LOG(INFO) << "register " << type << " " << action << " " << rawcode;
		switch(type_enum)
		{
		case InputConstants::type::button :
		{
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::button::_from_string(action.c_str());
		//	buttons[raw_enum] = action_enum;
		}
			break;
	
			case InputConstants::type::state :
		{
			const auto raw_enum = RawInputConstants::button::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::state::_from_string(action.c_str());
			//states.at(raw_enum) = action_enum;
		}
			break;
		case InputConstants::type::range :
		{
			const auto raw_enum = RawInputConstants::range::_from_string(rawcode.c_str());
			const auto action_enum = InputConstants::range::_from_string(action.c_str());
			//ranges.at(raw_enum) = action_enum;
		}
			break;
		default:
			//TODO
			break;
		}
	}
}}}