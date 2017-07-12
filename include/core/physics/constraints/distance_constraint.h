#pragma once

#include "core/physics/constraints/constraint_base.h"
#include "core/physics/particle.h"

namespace eversim { namespace core { namespace physics {

	class distance_constraint : public physics::constraint {
	public:

		explicit distance_constraint(float distance)
			: constraint(2), distance(distance)
		{
			type = physics::constraint_type::equality;
		}

		float operator()() const override
		{
			return length(particles[0]->projected_position - particles[1]->projected_position) - distance;
		}

		std::vector<glm::vec2> grad() const override
		{
			const auto n = normalize(particles[0]->projected_position - particles[1]->projected_position);
			return{ n, -n };
		}

	private:
		float distance;
	};

}}}
