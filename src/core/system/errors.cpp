#include "core/system/errors.h"

namespace eversim {namespace core {namespace system {
	
	namespace {
		struct gameobject_error_category : std::error_category {

			const char* name() const noexcept override {
				return "gameobject";
			}

			std::string message(int _Errval) const override {
				switch(static_cast<game_error>(_Errval))
				{
				case game_error::DuplicateComponent: 
					return "Duplicate component";
				case game_error::ComponentMissingClone: 
					return "do_clone() is not implemented!";
				default:
					return "(unknown error)";
				}
			}
		};

		const gameobject_error_category gameobject_errors;
	}


	std::error_code make_error_code(game_error err)
	{
		return {static_cast<int>(err), gameobject_errors };
	}
} } }