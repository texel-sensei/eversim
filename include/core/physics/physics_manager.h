#pragma once
#include "core/physics/particle.h"
#include "core/physics/constraints/constraint_base.h"
#include "core/physics/body.h"

#include "core/utility/object_pool.h"

#include <bitset>
#include <vector>
#include <memory>



namespace eversim { namespace core { namespace physics {
	class body_template;

	class physics_manager {
	public:
		constexpr static size_t max_constraint_arity = 3;

		body* add_body(body_template const&, glm::vec2 pos, float scale = 1.f);
		void add_particle(particle const& p);
		void add_constraint(std::unique_ptr<constraint> c);

		void integrate(float dt);

		particle& get_particle(int idx) { return particles.at(idx); }
		std::vector<particle>& get_particles() { return particles; }

		// debug functions
		void atomic_step(float dt);
		bool finished_frame() const { return current_state == simulation_state::external; }
		void draw_constraints(std::bitset<max_constraint_arity> to_render = ~0UL);
		
	private:
		int solver_iterations = 5;
		std::vector<particle> particles;
		utility::object_pool<body> bodies;
		
		std::vector<std::unique_ptr<constraint>> constraints;

		utility::array_view<particle> allocate_particles(size_t num);

		enum class simulation_state {
			external,
			damp,
			apply_velocity,
			constraint_iteration,
			apply_changes
		};
		simulation_state current_state = simulation_state::external;
		int current_iteration = 0;

		void apply_external_forces(float dt);
		void damp_velocities();
		void project_constraints();
	};
}}}
