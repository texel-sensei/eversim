#include "core/world/errors.h"

namespace eversim { namespace core { namespace world {
	
	namespace {
		struct level_error_category : std::error_category {

			const char* name() const noexcept override {
				return "level error";
			}
			std::string message(int _Errval) const override {
				switch(static_cast<level_error>(_Errval))
				{
				case level_error::UnknownID:
					return "unknown ID";
				case level_error::UnknownTile:
					return "unkown tile";
				default:
					return "(unknown)";
				}
			}
		};

		struct tile_error_category : std::error_category {

			const char* name() const noexcept override {
				return "tile error";
			}

			std::string message(int _Errval) const override {
				switch(static_cast<tile_error>(_Errval))
				{
				case tile_error::InheritanceCycle: 
					return "cyclic inheritance";
				case tile_error::UnknownTile:
					return "unkown tile name";
				default: 
					return "(unknown)";
				}
			}
		};

		const level_error_category level_err_cat;
		const tile_error_category tile_err_cat;
	}
	
	std::error_code make_error_code(level_error err)
	{
		return {static_cast<int>(err), level_err_cat};
	}

	std::error_code make_error_code(tile_error err)
	{
		return { static_cast<int>(err), tile_err_cat };
	}
}}}
