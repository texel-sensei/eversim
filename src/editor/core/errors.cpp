#include "editor/core/errors.h"

namespace eversim::editor::core{
	
	namespace {
		struct editor_error_category : std::error_category {

			const char* name() const noexcept override {
				return "editor";
			}

			std::string message(int _Errval) const override
			{
				if (!editor_error::_is_valid(_Errval)) {
					return "(unknown)";
				}
				return editor_error::_from_integral(_Errval)._to_string();
			}
		};

		const editor_error_category editor_errors;
	}


	std::error_code make_error_code(editor_error::_enumerated err)
	{
		return {static_cast<int>(err), editor_errors};
	}
}