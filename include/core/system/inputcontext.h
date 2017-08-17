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
		// map <RawInput,MappedInput>
		std::map<uint8_t,uint8_t> buttons;
		std::map<uint8_t,uint8_t> states;
		std::map<uint8_t,uint8_t> ranges;

	public:
		const std::string name;

		explicit InputContext(const std::string&);

		void register_action(const std::string& type,const std::string& action, const std::string& rawcode);
		void list_actions() const;
	};


}}}
