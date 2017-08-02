#pragma once
#include "core/error.h"

namespace eversim {namespace core{namespace physics {
	
	enum class body_template_error {
		// 0 is never an error
		NotEnoughParticles = 1,	// Number of particles was 0 or lower 
		NotEnoughConstraints,	// Number of constraints was negative
		SyntaxError,			// Parsing a line failed
		InvalidIndex			// Constraint indexed invalid Particle
	};

	// Required to convert body_template_erorr to std::error_code
	std::error_code make_error_code(body_template_error);

} } }

namespace std {
	template<>
	struct is_error_code_enum<eversim::core::physics::body_template_error> : true_type {};
}