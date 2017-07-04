#pragma once
#include "core/physics/particle.h"
#include <vector>


namespace eversim { namespace core { namespace physics {

	class physics_manager {
	public:
		void add_particle(particle const& p);
		void integrate(float dt);

		std::vector<particle> const& get_particles() const { return particles; }
	private:
		std::vector<particle> particles;

		void apply_external_forces(float dt);
		void damp_velocities();
		void project_constraints();
	};
}}}
