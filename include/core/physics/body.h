#pragma once
#include "core/utility/array_view.h"
#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {

	struct particle;

	struct body {
		friend class physics_manager;

		body() = default;
		explicit body(utility::array_view<particle> ps) : particles(ps){}

		glm::vec2 position;
		glm::vec2 velocity;

		float gravity_scale = 1.f;

		particle& get_particle(size_t idx) const;
		utility::array_view<particle> get_particles() const { return particles; }

	private:
		glm::vec2 old_position;
		glm::vec2 old_velocity;

		utility::array_view<particle> particles;
	};

}}}
