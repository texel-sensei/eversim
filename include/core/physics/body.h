#pragma once
#include "core/physics/particle.h"
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

		void* user_data = nullptr;

		particle& get_particle(size_t idx) const
		{
			assert(idx < particles.size());
			return particles[idx];
		}
		utility::array_view<particle> get_particles() const { return particles; }

		bool is_alive() const
		{
			return alive;
		}

		void kill() { alive = false; }

	private:
		glm::vec2 old_position;
		glm::vec2 old_velocity;
		bool alive = true;

		utility::array_view<particle> particles;
	};

}}}
