#pragma once
#include "core/utility/array_view.h"
#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {

	struct particle;

	struct body {
		glm::vec2 position;
		glm::vec2 velocity;

		utility::array_view<particle> particles;

		particle& get_particle(size_t idx) const;
	};

}}}
