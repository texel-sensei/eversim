#pragma once

#include "core/error.h"
#include <enum.h>

namespace eversim::editor::core {

	BETTER_ENUM(editor_error, uint8_t,	\
		DuplicateWindow = 1				\
	);

	// Required to convert body_template_erorr to std::error_code
	std::error_code make_error_code(editor_error::_enumerated);

}

namespace std {
	template<>
	struct is_error_code_enum<eversim::editor::core::editor_error::_enumerated> : true_type {};
}