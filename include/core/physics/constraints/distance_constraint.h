#pragma once

#include "core/physics/constraints/constraint_base.h"
#include "core/physics/particle.h"

namespace eversim { namespace core { namespace physics {

	class distance_constraint : public physics::constraint {
		friend class distance_constraint_factory;
	public:
		explicit distance_constraint(float distance)
			: constraint(2), distance(distance)
		{
			type = physics::constraint_type::equality;
		}

		void set_type(constraint_type t)
		{
			type = t;
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

		float get_distance() const
		{
			return distance;
		}

	private:
		float distance;
	};

	class distance_constraint_factory : public constraint_factory {
	public:
		data_container parse(std::istream&) const override;
		std::unique_ptr<constraint> build(constraint_descriptor const&, body const*) const override;

	};

}}}
