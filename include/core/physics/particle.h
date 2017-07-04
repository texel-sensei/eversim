#pragma once

#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {
	
	struct particle {
		glm::vec2 pos;
		glm::vec2 vel;

		glm::vec2 projected_position;
	};

}}}