#pragma once

#include "core/reflection/type.h"

namespace eversim::core::reflection
{
	
	template<typename Val>
	struct static_member {
		Val value;
		const char* name;
		Type<std::remove_reference_t<Val>> type;
		Type<Val> full_type;

		static_member(Val value, const char* name) : value(value), name(name) {}
	};

	template<typename T>
	decltype(auto) make_member(T&& t, const char* name)
	{
		return static_member<T>(std::forward<T>(t), name);
	}

}