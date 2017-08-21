#pragma once

#include "enum.h"
#include "core/input/inputconstants.h"
#include "core/input/rawinputconstants.h"
#include "core/input/inputevent.h"

#include <SDL2/SDL.h>

#include <string>
#include <map>
#include <functional>


namespace eversim { namespace core { namespace input {
	class InputEvent;


	class InputContext
	{
	private:
		std::string name;

		// map <RawInput,MappedInput>
		std::map<uint8_t,uint8_t> buttons;
		std::map<uint8_t,uint8_t> states;
		std::map<uint8_t,uint8_t> ranges;

		//map <MappedInput,state>
		std::map<uint8_t, bool> button_states;
		std::map<uint8_t, bool> state_states;
		std::map<uint8_t, double> range_states;

		// map <MappedInput,function>
		std::map<uint8_t, std::function<void()>> button_functions;
		std::map<uint8_t, std::function<void()>> state_functions;
		std::map<uint8_t, std::function<void()>> range_functions;

		InputConstants::input_type get_input_type(const RawInputConstants::button& b) const;
		InputConstants::input_type get_input_type(const RawInputConstants::range& r) const;
	public:
		
		InputContext(){}
		explicit InputContext(const std::string&);
		InputContext& operator=(const InputContext& other) noexcept;

		void register_action(const std::string& type,const std::string& action, const std::string& rawcode);
		
		void register_function(const InputConstants::button button, std::function<void()> f);
		void register_function(const InputConstants::state state, std::function<void()> f);
		void register_function(const InputConstants::range range, std::function<void()> f);

		void list_actions() const;

		void execute();

		bool handle_event(const InputEvent& event);

		std::string get_name() const { return name; }
	};


}}}
