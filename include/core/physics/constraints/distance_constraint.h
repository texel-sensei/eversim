#pragma once

#include "core/physics/constraint.h"
#include "core/physics/particle.h"
#include <array>

namespace eversim { namespace core { namespace physics {

	class distance_constraint : public physics::constraint<2> {
	public:

		explicit distance_constraint(float distance)
			: distance(distance)
		{
			type = physics::constraint_type::equality;
		}

		float operator()() const override
		{
			return length(particles[0]->projected_position - particles[1]->projected_position) - distance;
		}

		std::array<glm::vec2, arity()> grad() const override
		{
			const auto n = normalize(particles[0]->projected_position - particles[1]->projected_position);
			return{ n, -n };
		}

	private:
		float distance;
	};

}}}
