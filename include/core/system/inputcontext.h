#pragma once

#include "enum.h"
#include "core/system/inputconstants.h"
#include "core/system/rawinputconstants.h"

#include <string>
#include <map>


namespace eversim { namespace core { namespace system {
	
	
	class InputContext
	{
	private:
		std::map<RawInputConstants::button,InputConstants::button> buttons;
		std::map<RawInputConstants::button,InputConstants::state> states;
		std::map<RawInputConstants::range,InputConstants::range> ranges;
	public:
		const std::string name;

		InputContext(const std::string&);

		void register_action(const std::string& type,const std::string& action, const std::string& rawcode);
	};


}}}
