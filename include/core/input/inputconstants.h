#pragma once


#include "enum.h"

#include <vector>
#include <map>
#include <algorithm>


namespace eversim {	namespace core { namespace input {

	template<typename T>
	class dynamic_enum {
	private:
		size_t idx = 1;
		std::map<std::string, T> enums = { {"INVALID",0} };
	public:
		std::string to_string(const T& v) const
		{
			const auto it = std::find_if(
				begin(enums),
				end(enums), 
				[&](const std::pair<std::string,T>& p) 
			{
				return p.second == v;
			});
			if (it != enums.end())
				return it->first;
			return "INVALID";
		}
		T to_enum(const std::string& s) const
		{
			const auto it = enums.find(s);
			if (it != enums.end())
				return it->second;
			return 0;
		}
		T add(const std::string& s)
		{
			auto it = enums.find(s);
			if(it == enums.end()) {
				enums[s] = idx++;
				return idx - 1;
			}
			return it->second;
		}
	};

	namespace InputConstants
	{
		typedef uint8_t action;

		BETTER_ENUM(input_type, uint8_t,
			BUTTON = 0,
			STATE,
			RANGE,
			INVALID
		)
	}

}}}