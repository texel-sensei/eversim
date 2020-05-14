#pragma once

#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {
	
	struct AABB {
		glm::vec2 lower_left;
		glm::vec2 upper_right;
	};
	
}}}