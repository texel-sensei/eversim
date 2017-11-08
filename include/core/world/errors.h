#pragma once

#include "core/error.h"

namespace eversim { namespace core { namespace world {

	enum class level_error {
		// 0 means no error
		UnknownID = 1,	// Level contains an ID without corresponding name
		UnknownTile,	// Tile name is not known
		MissingIDTable, // ID table file in level is missing
		MissingLayout,	// Layout file in level is missing
	};

	enum class tile_error {
		// 0 means no error
		InheritanceCycle = 1, // Cyclic inheritance is not allowed
		UnknownTile,	// Tile name is not known
	};

	std::error_code make_error_code(level_error);
	std::error_code make_error_code(tile_error);
}}}

namespace std {
	template<>
	struct is_error_code_enum<eversim::core::world::level_error> : true_type {};

	template<>
	struct is_error_code_enum<eversim::core::world::tile_error> : true_type {};
}