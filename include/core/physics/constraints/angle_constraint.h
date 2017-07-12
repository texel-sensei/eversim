#pragma once

#include "core/physics/constraints/constraint_base.h"
#include "core/physics/particle.h"
#include <array>

namespace eversim { namespace core { namespace physics {

	class angle_constraint : public constraint {
	public:

		explicit angle_constraint(float target_angle)
			: constraint(3), target_angle(target_angle)
		{
			type = constraint_type::equality;
		}

		float operator()() const override
		{
			const auto v1 = normalize(particles[0]->projected_position - particles[1]->projected_position);
			const auto v2 = normalize(particles[2]->projected_position - particles[1]->projected_position);

			const auto angle = acosf(dot(v1, v2));
			return angle - target_angle;
		}

		std::vector<glm::vec2> grad() const override
		{
			//const auto n = normalize(particles[0]->projected_position - particles[1]->projected_position);
			//return{ n, -n };
			const auto grad_acos = [](float x) {return -1.f / sqrtf(1 - x*x); };
			const auto v1 = normalize(particles[0]->projected_position - particles[1]->projected_position);
			const auto v2 = normalize(particles[2]->projected_position - particles[1]->projected_position);

			const auto cos_angle = dot(v1, v2);
			const auto k = grad_acos(cos_angle);
			// acosf(dot(v1,v2)) => grad_acos(dot(..)) * grad(dot(..))

			// d/dp0 d/dp1 d/dp2
			// grad v1 = {v1 -v1 0}
			// grad v2 = {0 -v2 v2}
			return {
				glm::vec2{v1.y, -v1.x},
				{},
				glm::vec2{ v2.y, -v2.x }
			};
		}

	private:
		float target_angle;
	};

}}}
