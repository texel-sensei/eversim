#pragma once

#include "enum.h"
#include "core/input/inputconstants.h"
#include "core/input/rawinputconstants.h"
#include "core/input/inputevent.h"

#include <SDL2/SDL.h>

#include <string>
#include <map>
#include <set>
#include <list>
#include <functional>

#include "enum.h"

namespace eversim { namespace core { namespace input {
	class InputContext;
	class InputEvent;

	BETTER_ENUM(state_func_type,uint8_t,
		PRESS=0,
		HOLD,
		RELEASE,
		INVALID
	)

	typedef std::function<void(InputContext&)> button_function;
	typedef std::function<void(InputContext&, state_func_type)> state_function;
	typedef std::function<void(InputContext&, glm::vec2)> range_function;
	typedef std::map <
		RawInputConstants::input,
		std::pair<RawInputConstants::input, RawInputConstants::input>
	> pairmap;

	class InputContext
	{
	private:
		std::string name;

		//dynamic action enums
		dynamic_enum<InputConstants::action> action_enums;

		// map <MappedInput,actions>
		std::map<RawInputConstants::input,std::set<InputConstants::action>> buttons;
		std::map<RawInputConstants::input,std::set<InputConstants::action>> states;
		std::map<RawInputConstants::input,std::set<InputConstants::action>> ranges;

		//map <action,state>
		std::map<InputConstants::action, bool> button_states;
		std::map<InputConstants::action, bool> state_states;
		std::map<InputConstants::action, glm::vec2> range_states;

		// map <action,function>
		std::map<InputConstants::action, button_function> button_functions;
		std::map<InputConstants::action, state_function> state_functions;
		std::map<InputConstants::action, range_function> range_functions;

		std::map<RawInputConstants::input,InputEvent> input_map;
		std::vector<std::map<RawInputConstants::input, InputEvent>::iterator> input_iterators;

		pairmap input_range_pairs;

		InputConstants::input_type get_input_type(const RawInputConstants::input& b) const;

		void create_range_pair(RawInputConstants::input a, RawInputConstants::input b, RawInputConstants::input c);
		void create_grouped_inputs();

		void add_input_pair(
			const std::string& a,
			const std::string& b,
			const std::string& c,
			pairmap& m
		);
	public:
		
		InputContext(){}
		explicit InputContext(const std::string&);
		InputContext& operator=(const InputContext& other) noexcept;

		void register_action(const std::string& type,const std::string& action, const std::string& rawcode);
		
		void register_function_button(const std::string& a, button_function f);
		void register_function_state(const std::string& a, state_function f);
		void register_function_range(const std::string& a, range_function f);

		void register_function_button(const InputConstants::action& a, button_function f);
		void register_function_state(const InputConstants::action& a, state_function f);
		void register_function_range(const InputConstants::action& a, range_function f);

		void list_actions() const;

		void execute();

		bool handle_event(const InputEvent& event);
		void enqueue_event(const InputEvent& event);

		std::string get_name() const { return name; }

		std::vector<RawInputConstants::input> get_buttons() const;
		std::vector<RawInputConstants::input> get_states() const;
		std::vector<RawInputConstants::input> get_ranges() const;

		void reset_states(const std::vector<RawInputConstants::input>&);
		void reset_ranges(const std::vector<RawInputConstants::input>&);

		void reset_states(const std::vector<std::vector<RawInputConstants::input>>&);
		void reset_ranges(const std::vector<std::vector<RawInputConstants::input>>&);

		void add_input_pair_range(
			const std::string& a,
			const std::string& b,
			const std::string& c);
	};
}}}
