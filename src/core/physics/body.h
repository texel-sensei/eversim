#pragma once
#include "core/physics/particle.h"
#include "core/utility/array_view.h"
#include "core/utility/math/angular.h"
#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {

	struct particle;

	struct body {
		friend class physics_manager;

		body() = default;
		explicit body(utility::array_view<particle> ps);

		void init(
			utility::array_view<particle>
		);

		glm::vec2 position;
		glm::vec2 velocity;
		utility::math::orientation angle;


		float gravity_scale = 1.f;
		float linear_drag = 1.f;
		float quadratic_drag = 1.f;

		void* user_data = nullptr;

		particle& get_particle(size_t idx) const
		{
			EVERSIM_ASSERT(idx < particles.size());
			return particles[idx];
		}

		utility::array_view<particle> get_particles() const { return particles; }

		bool is_alive() const
		{
			return alive;
		}

		bool is_on_ground() const { return on_ground; }

		void kill() { alive = false; }

	private:
		glm::vec2 old_position;
		glm::vec2 old_velocity;
		float angle_sum = 0.f;
		bool alive = true;
		bool on_ground = false;

		utility::array_view<particle> particles;
	};

}}}