#pragma once

#include "core/error.h"

namespace eversim { namespace core { namespace world {

	enum class level_error {
		// 0 means no error
		UnknownID = 1,	// Level contains an ID without corresponding name
		UnknownTile,	// Tile name is not known
	};

	std::error_code make_error_code(level_error);

}}}

namespace std {
	template<>
	struct is_error_code_enum<eversim::core::world::level_error> : true_type {};
}