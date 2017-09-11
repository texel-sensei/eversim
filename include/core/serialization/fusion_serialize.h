#pragma once

#include "core/reflection/fusion_access.h"
#include <easylogging++.h>

namespace boost::serialization
{

	template<typename T, typename Archive>
	void serialize(Archive& ar, T&& object, unsigned int version)
	{
		if constexpr(std::is_polymorphic_v<T>) {
			LOG(ERROR) << "Serialization of derived classes is not yet implemented!";
		}
		eversim::core::reflection::iterate_members(std::forward<T>(object),
			[](auto&& member)
		{
			ar & member.value;
		});
	}
	
}