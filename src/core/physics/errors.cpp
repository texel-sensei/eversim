#include "core/physics/errors.h"

namespace eversim {namespace core {namespace physics {
	
	namespace {
		struct body_template_error_category : std::error_category {

			const char* name() const noexcept override {
				return "body_template_loader";
			}

			std::string message(int _Errval) const override {
				switch(static_cast<body_template_error>(_Errval))
				{
				case body_template_error::NotEnoughParticles:
					return "Not enough particles!";
				case body_template_error::NotEnoughConstraints:
					return "Not enough constraints";
				case body_template_error::SyntaxError:
					return "Syntax error";
				case body_template_error::InvalidIndex:
					return "Invalid index in constraint!";
				default: ;
					return "(unknown error)";
				}
			}
		};

		const body_template_error_category body_errors;
	}


	std::error_code make_error_code(body_template_error err)
	{
		return {static_cast<int>(err), body_errors};
	}
} } }