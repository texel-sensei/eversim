#pragma once
#include <string>

namespace eversim { namespace core { namespace world {

	enum class collision_type {
		none,
		solid,
		extra
	};

	struct tile_descriptor {
		std::string name;
		collision_type collision = collision_type::none;

		std::string texture_name;
	};

	

}}}
