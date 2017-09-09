#pragma once

#include "core/reflection/fusion_access.h"

#include <map>
#include <typeindex>

namespace eversim::core::reflection {

	struct dynamic_member {
		std::string name;
		std::type_index type;
		ptrdiff_t byte_offset = 0;
		size_t byte_size = 0;
		int index;

		explicit dynamic_member(std::type_index type)
			: type(type)
		{}

		friend bool operator==(dynamic_member const& a, dynamic_member const& b) {
			return std::tie(a.name, a.type, a.byte_offset, a.byte_size, a.index)
				== std::tie(b.name, b.type, b.byte_offset, b.byte_size, b.index);
		}

		friend bool operator<(dynamic_member const& a, dynamic_member const& b) {
			return std::tie(a.name, a.type, a.byte_offset, a.byte_size, a.index)
				<  std::tie(b.name, b.type, b.byte_offset, b.byte_size, b.index);
		}
	};

	using dynamic_member_map = std::map<std::string, dynamic_member>;

	template<typename Object,
		typename = require_fusion_sequence<Object>
	>
		dynamic_member_map get_member_map()
	{
		Object const* obj = nullptr; // this is a bit of a hack, but it's the same hack offsetof uses
		const auto base = reinterpret_cast<const char*>(obj);

		dynamic_member_map mm;
		int idx = 0;
		iterate_members(*obj, [&](auto const& member)
		{
			auto info = dynamic_member(member.type.type_index());
			info.name = member.name;
			info.index = idx;
			info.byte_offset = reinterpret_cast<const char*>(&member.value) - base;
			info.byte_size = sizeof(member.value);
			mm.insert(std::make_pair(info.name, info));
			idx++;
		});
		return mm;
	}
}
