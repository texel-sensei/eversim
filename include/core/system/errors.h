#pragma once
#include "core/error.h"

namespace eversim {namespace core{namespace system {
	
	enum class game_error {
		// 0 is never an error
		DuplicateComponent = 1,	// Component in gameobject already exists
		ComponentMissingClone	// Component class doesn't implement do_clone()
	};

	// Required to convert body_template_erorr to std::error_code
	std::error_code make_error_code(game_error);

} } }

namespace std {
	template<>
	struct is_error_code_enum<eversim::core::system::game_error> : true_type {};
}