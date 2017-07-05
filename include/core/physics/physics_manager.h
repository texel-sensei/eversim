#pragma once
#include "core/physics/particle.h"
#include "core/physics/AABB.h"
#include "core/physics/constraint.h"
#include <tuple>
#include <vector>
#include <memory>


namespace eversim { namespace core { namespace physics {

	class physics_manager {
	public:
		void add_particle(particle const& p);
		void integrate(float dt);

		particle& get_particle(int idx) { return particles.at(idx); }
		std::vector<particle> const& get_particles() const { return particles; }

		template<size_t N, typename Constraint,
			typename = std::enable_if_t<std::is_base_of<constraint<N>, Constraint>::value>
		>
		void add_constraint(Constraint const& c)
		{
			std::get<N-1>(constraints).push_back(std::make_unique<Constraint>(c));
		}

	private:
		std::vector<particle> particles;

		std::tuple<
			std::vector<std::unique_ptr<constraint<1>>>,
			std::vector<std::unique_ptr<constraint<2>>>
		> constraints;

		void apply_external_forces(float dt);
		void damp_velocities();
		void project_constraints();
	};
}}}
