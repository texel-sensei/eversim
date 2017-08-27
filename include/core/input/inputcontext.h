#pragma once

#include "enum.h"
#include "core/input/inputconstants.h"
#include "core/input/rawinputconstants.h"
#include "core/input/inputevent.h"

#include <SDL2/SDL.h>

#include <string>
#include <map>
#include <set>
#include <functional>



namespace eversim { namespace core { namespace input {
	class InputContext;
	class InputEvent;
	typedef std::function<void(InputContext&)> button_function;
	typedef std::function<void(InputContext&, double)> range_function;
	
	class InputContext
	{
	private:
		std::string name;

		// map <MappedInput,actions>
		std::map<uint8_t,std::set<uint8_t>> buttons;
		std::map<uint8_t,std::set<uint8_t>> states;
		std::map<uint8_t,std::set<uint8_t>> ranges;

		//map <action,state>
		std::map<uint8_t, bool> button_states;
		std::map<uint8_t, bool> state_states;
		std::map<uint8_t, double> range_states;

		// map <action,function>
		std::map<uint8_t, button_function> button_functions;
		std::map<uint8_t, button_function> state_functions;
		std::map<uint8_t, range_function> range_functions;

		InputConstants::input_type get_input_type(const RawInputConstants::button& b) const;
		InputConstants::input_type get_input_type(const RawInputConstants::range& r) const;
	public:
		
		InputContext(){}
		explicit InputContext(const std::string&);
		InputContext& operator=(const InputContext& other) noexcept;

		void register_action(const std::string& type,const std::string& action, const std::string& rawcode);
		
		void register_function(const InputConstants::button button, 
			button_function f);
		void register_function(const InputConstants::state state, 
			button_function f);
		void register_function(const InputConstants::range range, 
			range_function f);

		void list_actions() const;

		void execute();

		bool handle_event(const InputEvent& event);

		std::string get_name() const { return name; }

		std::vector<RawInputConstants::button> get_buttons() const;
		std::vector<RawInputConstants::button> get_states() const;
		std::vector<RawInputConstants::range> get_ranges() const;

		void reset_states(const std::vector<RawInputConstants::button>&);
		void reset_ranges(const std::vector<RawInputConstants::range>&);

		void reset_states(const std::vector<std::vector<RawInputConstants::button>>&);
		void reset_ranges(const std::vector<std::vector<RawInputConstants::range>>&);
	};
}}}
