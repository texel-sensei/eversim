#pragma once

#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {
	
	struct body;

	struct particle {
		glm::vec2 pos;
		glm::vec2 vel;

		float inv_mass = 1.f;

		glm::vec2 projected_position;
		body* owner;

		bool is_alive() const;
	};

}}}