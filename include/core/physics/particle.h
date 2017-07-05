#pragma once

#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {
	
	struct particle {
		glm::vec2 pos;
		glm::vec2 vel;

		float inv_mass;

		glm::vec2 projected_position;
	};

}}}