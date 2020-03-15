#include "core/input/inputcontext.h"

#include <easylogging++.h>

using namespace std;

namespace eversim {	namespace core { namespace input {

	InputContext::InputContext(const string& n) : name(n)
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

	void InputContext::register_action(const string& type, const string& action, const string& rawcode)
	{
		auto type_enum = InputConstants::input_type::_from_string(type.c_str());

		//LOG(INFO) << type << " " << action << " " << rawcode;

		const auto raw_enum = RawInputConstants::input::_from_string(rawcode.c_str());
		const auto action_enum = action_enums.add(action);

		auto* map_ptr = &buttons; map_ptr = nullptr;

		switch(type_enum)
		{
		case InputConstants::input_type::BUTTON :
			map_ptr = &buttons;
			break;
		case InputConstants::input_type::STATE :
			map_ptr = &states;
			break;
		case InputConstants::input_type::RANGE :
			map_ptr = &ranges;
			break;
		default:
			return;
		}

		if (map_ptr != nullptr) {
			auto& map_ref = *map_ptr;
			auto it = map_ref.find(raw_enum);
			if (it == map_ref.end())
				map_ref[raw_enum] = {};
			map_ref[raw_enum].emplace(action_enum);
		}
	}

	void InputContext::register_button_to_range(
		const std::string& action,
		const std::string& rawcode_a,
		const std::string& rawcode_b,
		const glm::vec2 values,
		const glm::vec2 interval,
		std::function<double(double)> function
	)
	{
		buttonpair_ranges.emplace_back(
			rawcode_a, rawcode_b, values, interval, function, action, *this
		);
		register_handled_input(rawcode_a);
		register_handled_input(rawcode_b);
	}

	vector<RawInputConstants::input> InputContext::get_buttons() const
	{
		vector<RawInputConstants::input> res;
		for (auto& bpair : buttons)
			res.push_back(bpair.first);
		return res;
	}

	vector<RawInputConstants::input> InputContext::get_states() const
	{
		vector<RawInputConstants::input> res;
		for (auto& spair : states)
			res.push_back(spair.first);
		return res;
	}

	vector<RawInputConstants::input> InputContext::get_ranges() const
	{
		vector<RawInputConstants::input> res;
		for (auto& rpair : ranges)
			res.push_back(rpair.first);
		return res;
	}

	void InputContext::reset_states(const vector<RawInputConstants::input>& ss)
	{
		for (const auto& b : ss)
		{
			auto it = states.find(b);

			if (it == states.end()) continue;
			auto& actions = *it;

			for (const auto& action : actions.second)
			{
				for_each(begin(state_states), end(state_states),
					[&](pair<const InputConstants::action, bool>& element)
				{
					if(element.first == action)
						element.second = false;
				});
			}
		}
	}

	void InputContext::reset_ranges(const vector<RawInputConstants::input>& rs)
	{
		for (const auto& b : rs)
		{
			auto it = ranges.find(b);

			if (it == ranges.end()) continue;
			auto& actions = *it;

			for (const auto& action : actions.second)
			{
				for_each(begin(range_states), end(range_states),
					[&](pair<const InputConstants::action, glm::vec2>& element)
				{
					if (element.first == action)
						element.second = {0.,0.};
				});
			}
		}
	}

	void InputContext::reset_states(const vector<vector<RawInputConstants::input>>& v)
	{
		for (const auto& s : v)
			reset_states(s);
	}

	void InputContext::reset_ranges(const vector<vector<RawInputConstants::input>>& v)
	{
		for (const auto& s : v)
			reset_ranges(s);
	}

	InputConstants::input_type InputContext::get_input_type(const RawInputConstants::input& i) const
	{
		if(buttons.find(i) != buttons.end()) return InputConstants::input_type::BUTTON;
		if(states.find(i) != states.end()) return InputConstants::input_type::STATE;
		if(ranges.find(i) != ranges.end()) return InputConstants::input_type::RANGE;
		return InputConstants::input_type::INVALID;
	}

	void InputContext::enqueue_event(const InputEvent& event)
	{
		auto const& event_enum = event.get_input_enum();
		auto const& mpair = input_map.emplace(event_enum,event);

		auto const& it = mpair.first;

		if (!mpair.second) {
			auto new_end = remove_if(begin(input_iterators), end(input_iterators), 
				[&](const map<RawInputConstants::input, InputEvent>::iterator& eit)
			{
				return eit->first == event.get_input_enum();
			});

			input_iterators = vector<map<RawInputConstants::input, InputEvent>::iterator>
				(begin(input_iterators), new_end);
		}
		input_iterators.push_back(it);
	}

	bool InputContext::handle_event(const InputEvent& event)
	{
		bool handeled = false;

		if (event.get_event_type() == +RawInputConstants::event_type::INVALID) return handeled;

		vector<map<RawInputConstants::input, set<InputConstants::action>>*> v =
		{ &buttons,&states,&ranges };

		const auto input_enum = event.get_input_enum();

		for (auto* ptr : v) {
			auto& m = *ptr;
			auto action_it = m.find(input_enum);
			handeled = handeled || (action_it != m.end());
		}

		handeled = handeled || (handled_inputs.count( event.get_input_enum() ) > 0);

		if (handeled)
			enqueue_event(event);

		return handeled;
	}

	void InputContext::register_function_button(const string& a, button_function f)
	{
		const auto enumerator = action_enums.to_enum(a);
		register_function_button(enumerator, f);
	}

	void InputContext::register_function_state(const string& a, state_function f)
	{
		const auto enumerator = action_enums.to_enum(a);
		register_function_state(enumerator, f);
	}

	void InputContext::register_function_range(const string& a, range_function f)
	{
		const auto enumerator = action_enums.to_enum(a);
		register_function_range(enumerator, f);
	}

	void InputContext::register_function_button(const InputConstants::action& a, button_function f)
	{
		const auto name = action_enums.to_string(a);
		if (name == "INVALID") {
			LOG(ERROR) << "unknown action " << name;
			return;
		}
		button_functions[a] = f;
	}

	void InputContext::register_function_state(const InputConstants::action& a, state_function f)
	{
		const auto name = action_enums.to_string(a);
		if (name == "INVALID") {
			LOG(ERROR) << "unknown action " << name;
			return;
		}
		state_functions[a] = f;
	}

	void InputContext::register_function_range(const InputConstants::action& a, range_function f)
	{
		const auto name = action_enums.to_string(a);
		if (name == "INVALID") {
			LOG(ERROR) << "unknown action " << name;
			return;
		}
		range_functions[a] = f;
	}

	void InputContext::register_handled_input(const std::string& a)
	{
		handled_inputs.emplace(RawInputConstants::input::_from_string(a.c_str()));
	}

	void InputContext::create_range_pair(
		RawInputConstants::input a, 
		RawInputConstants::input b, 
		RawInputConstants::input c)
	{
		auto const& lsx = input_map.find(a);
		auto const& lsy = input_map.find(b);
		const auto lsxb = lsx != input_map.end();
		const auto lsyb = lsy != input_map.end();
		if (lsxb || lsyb)
		{
			glm::vec2 val(0);
			if (lsxb) {
				auto& event = lsx->second;
				val.x = event.get_range_value().x;
			}
			if (lsyb) {
				auto& event = lsy->second;
				val.y = event.get_range_value().x;
			}

			auto action_it = ranges.find(c);
			if (action_it != ranges.end()) {
				for (auto& action : action_it->second)
				{
					auto& range_state_value = range_states[action];
					if (!lsxb) {
						val.x = range_state_value.x;
					}
					if (!lsyb) {
						val.y = range_state_value.y;
						break;
					}
					if (!lsxb) break;
				}
			}
			enqueue_event(InputEvent::create_range(c, val));
		}
	}

	void InputContext::create_button_range_inputs(ButtonPair &bp)
	{
		
		auto a = bp.rawcode_a;
		auto b = bp.rawcode_b;

		auto it_a = find_if(begin(input_iterators), end(input_iterators), 
		[&](const auto& event_it) 
		{ 
			const auto& event = event_it->second;
			const auto input_enum = event.get_input_enum();
			return a == input_enum;  
		}
		);

		auto it_b = find_if(begin(input_iterators), end(input_iterators),
			[&](const auto& event_it)
		{
			const auto& event = event_it->second;
			const auto input_enum = event.get_input_enum();
			return b == input_enum;
		}
		);

		
		auto fa = it_a != input_iterators.end();
		auto fb = it_b != input_iterators.end();

		auto val_a = (fa) ? bp.values[0] : 0.;
		auto val_b = (fb) ? bp.values[1] : 0.;

		if (fa || fb) {

			auto handle = [&](auto it, auto& val){
				auto run = it != input_iterators.end();
				if (!run) return 0;
				if (run && (*it)->second.get_event_type()._to_integral() == RawInputConstants::event_type::BUTTON_UP)
				{
					auto& ev = (*it)->second;
					val = 0.;
					return 0;
				}
				return 1;
			};

			auto limit = [](auto& val, auto bp_interval) {
				val = std::max(bp_interval[0], val);
				val = std::min(bp_interval[1], val);
			};

			auto cnt = handle(it_a,val_a) + handle(it_b,val_b);
			float val = 0.;
			if (cnt == 1) {
				val = (fa) ? -val_a : val_b;
				limit(val, bp.interval);
			}
			else {
				val = (bp.function(val_b) - bp.function(val_a)) / 2;
				limit(val, bp.interval);
			}
			range_states[bp.linked_action] = { val,val };
		}
		
	}

	void InputContext::create_grouped_inputs()
	{
		using namespace RawInputConstants;

		for (auto& e : input_range_pairs) {
			create_range_pair(e.second.first,e.second.second,e.first);
		}	

		for (auto& button_pair : buttonpair_ranges) {
			create_button_range_inputs(button_pair);
		}
	}

	void InputContext::add_input_pair(
		const string& a,
		const string& b,
		const string& c,
		pairmap& m
	) 
	{
		using namespace RawInputConstants;

		auto is_valid = [](const string& s)
		{
			return input::_is_valid(s.c_str());
		};

		if (is_valid(a) && is_valid(b) && is_valid(c)) {

			auto ae = input::_from_string(a.c_str());
			auto be = input::_from_string(b.c_str());
			auto ce = input::_from_string(c.c_str());

			m.emplace(ce, pair<input, input>(ae,be));
		}
	}

	void InputContext::add_input_pair_range
	(
		const string& a,
		const string& b,
		const string& c
	)
	{
		add_input_pair(a, b, c, input_range_pairs);
	}

	void InputContext::execute()
	{
		create_grouped_inputs();

		for (const auto& event_it : input_iterators) {
			const auto& event = event_it->second;
			const auto input_enum = event.get_input_enum();
			{
				//find in buttons

				auto action_it = buttons.find(input_enum);
				if (action_it != buttons.end()) {
					for (auto& action : action_it->second)
					{
						button_states[action] = (event.get_event_type() == +RawInputConstants::event_type::BUTTON_DOWN);
					}
				}
			}
			{
				//find in states
				auto action_it = states.find(input_enum);
				if (action_it != states.end()) {
					for (auto& action : action_it->second)
					{
						state_states[action] = (event.get_event_type() == +RawInputConstants::event_type::BUTTON_DOWN);
					}
				}
			}
			{
				//find in ranges
				auto action_it = ranges.find(input_enum);
				if (action_it != ranges.end()) {
					for (auto& action : action_it->second)
					{
						range_states[action] = event.get_range_value();
					}
				}
			}
		}

		input_iterators.clear();
		input_map.clear();

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
					it->second(*this,state_func_type::HOLD);
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
			LOG(INFO) << "\tBUTTON " << RawInputConstants::input::_from_integral(button.first)._to_string();
			for (auto& action : button.second)
			{
				LOG(INFO) << "\t\t" << action_enums.to_string(action);
			}
		}

		for (auto& state : states)
		{
			LOG(INFO) << "\tSTATE " << RawInputConstants::input::_from_integral(state.first)._to_string();
			for (auto& action : state.second)
			{
				LOG(INFO) << "\t\t" << action_enums.to_string(action);
			}
		}

		for (auto& range : ranges)
		{
			LOG(INFO) << "\tRANGE " << RawInputConstants::input::_from_integral(range.first)._to_string();
			for (auto& action : range.second)
			{
				LOG(INFO) << "\t\t" << action_enums.to_string(action);
			}
		}
	}
}}}
