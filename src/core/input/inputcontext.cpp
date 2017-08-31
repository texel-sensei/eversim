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

	std::vector<RawInputConstants::input> InputContext::get_buttons() const
	{
		std::vector<RawInputConstants::input> res;
		for (auto& bpair : buttons)
			res.push_back(bpair.first);
		return res;
	}

	std::vector<RawInputConstants::input> InputContext::get_states() const
	{
		std::vector<RawInputConstants::input> res;
		for (auto& spair : states)
			res.push_back(spair.first);
		return res;
	}

	std::vector<RawInputConstants::input> InputContext::get_ranges() const
	{
		std::vector<RawInputConstants::input> res;
		for (auto& rpair : ranges)
			res.push_back(rpair.first);
		return res;
	}

	void InputContext::reset_states(const std::vector<RawInputConstants::input>& ss)
	{
		for (const auto& b : ss)
		{
			auto it = states.find(b);

			if (it == states.end()) continue;
			auto& actions = *it;

			for (const auto& action : actions.second)
			{
				std::for_each(begin(state_states), end(state_states),
					[&](std::pair<const InputConstants::action, bool>& element)
				{
					if(element.first == action)
						element.second = false;
				});
			}
		}
	}

	void InputContext::reset_ranges(const std::vector<RawInputConstants::input>& rs)
	{
		for (const auto& b : rs)
		{
			auto it = ranges.find(b);

			if (it == ranges.end()) continue;
			auto& actions = *it;

			for (const auto& action : actions.second)
			{
				std::for_each(begin(range_states), end(range_states),
					[&](std::pair<const InputConstants::action, glm::vec2>& element)
				{
					if (element.first == action)
						element.second = {0.,0.};
				});
			}
		}
	}

	void InputContext::reset_states(const std::vector<std::vector<RawInputConstants::input>>& v)
	{
		for (const auto& s : v)
			reset_states(s);
	}

	void InputContext::reset_ranges(const std::vector<std::vector<RawInputConstants::input>>& v)
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
		input_queue.push_back(event);
	}

	bool InputContext::handle_event(const InputEvent& event)
	{
		bool handeled = false;

		if (event.get_event_type() == +RawInputConstants::event_type::INVALID) return handeled;

		std::vector<std::map<RawInputConstants::input, std::set<InputConstants::action>>*> v =
		{ &buttons,&states,&ranges };

		const auto input_enum = event.get_input_enum();

		for (auto* ptr : v) {
			auto& m = *ptr;
			auto action_it = m.find(input_enum);
			handeled = handeled || (action_it != m.end());
		}

		if (handeled) enqueue_event(event);

		return handeled;
	}

	void InputContext::register_function_button(const std::string& a, button_function f)
	{
		const auto enumerator = action_enums.to_enum(a);
		register_function_button(enumerator, f);
	}

	void InputContext::register_function_state(const std::string& a, state_function f)
	{
		const auto enumerator = action_enums.to_enum(a);
		register_function_state(enumerator, f);
	}

	void InputContext::register_function_range(const std::string& a, range_function f)
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

	void InputContext::create_grouped_inputs()
	{
		auto find_enum = [&](const RawInputConstants::input e)
		{
			return find_if(begin(input_queue), end(input_queue),
				[&](const InputEvent& event)
			{
				return event.get_input_enum() == +e;
			});
		};

		auto it_left_stick_x = find_enum(RawInputConstants::input::GAMEPAD_ANALOGUE_LEFT_STICK_X);
		auto it_left_stick_y = find_enum(RawInputConstants::input::GAMEPAD_ANALOGUE_LEFT_STICK_Y);

		if (it_left_stick_x != input_queue.end() || it_left_stick_y != input_queue.end())
		{

			glm::vec2 val(0);

			if (it_left_stick_x != input_queue.end()) {
				auto& event = *it_left_stick_x;
				val.x = event.get_range_value().x;
			}

			if (it_left_stick_y != input_queue.end()) {
				auto& event = *it_left_stick_y;
				val.y = event.get_range_value().x;
			}

			input_queue.push_back(InputEvent::create_range(RawInputConstants::input::GAMEPAD_ANALOGUE_LEFT_STICK, val));
		}

		auto it_right_stick_x = find_enum(RawInputConstants::input::GAMEPAD_ANALOGUE_RIGHT_STICK_X);
		auto it_right_stick_y = find_enum(RawInputConstants::input::GAMEPAD_ANALOGUE_RIGHT_STICK_Y);

		if (it_right_stick_x != input_queue.end() || it_right_stick_y != input_queue.end())
		{

			glm::vec2 val(0);

			if (it_right_stick_x != input_queue.end()) {
				auto& event = *it_right_stick_x;
				val.x = event.get_range_value().x;
			}

			if (it_right_stick_y != input_queue.end()) {
				auto& event = *it_right_stick_y;
				val.y = event.get_range_value().x;
			}

			input_queue.push_back(InputEvent::create_range(RawInputConstants::input::GAMEPAD_ANALOGUE_RIGHT_STICK, val));
		}
	}

	void InputContext::execute()
	{
		create_grouped_inputs();

		for (const auto& event : input_queue) {
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

		input_queue.clear();

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