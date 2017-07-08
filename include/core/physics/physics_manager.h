#pragma once
#include "core/physics/particle.h"
#include "core/physics/constraint.h"
#include "core/rendering/render_manager.h"
#include "core/utility/helper.h"

#include <bitset>
#include <tuple>
#include <vector>
#include <memory>



namespace eversim { namespace core { namespace physics {
	


	class physics_manager {
	public:
		constexpr static size_t max_constraint_arity = 3;

		void add_particle(particle const& p);
		void integrate(float dt);

		particle& get_particle(int idx) { return particles.at(idx); }
		std::vector<particle> const& get_particles() const { return particles; }

		template<typename Constraint,
			typename = std::enable_if_t<std::is_base_of<abstract_constraint, Constraint>::value>
		>
		void add_constraint(Constraint const& c)
		{
			static_assert(c.arity() <= max_constraint_arity, "Constraint arity is too big, increase max_constraint_arity!");
			std::get<c.arity() - 1>(constraints).push_back(std::make_unique<Constraint>(c));
		}

		// debug functions
		void atomic_step(float dt);
		bool finished_frame() const { return current_state == simulation_state::external; }
		void draw_constraints(std::bitset<max_constraint_arity> to_render = -1);

	private:
		int solver_iterations = 5;
		std::vector<particle> particles;

		struct container {
			template<size_t I>
			using constraint_container = std::vector<std::unique_ptr<constraint<I+1>>>;

			template<size_t... Is >
			static auto make(std::index_sequence<Is...>)->std::tuple<constraint_container<Is>...>;

			template<size_t MaxArity>
			using type = decltype(make(std::make_index_sequence<MaxArity>{}));
		};

		
		container::type<max_constraint_arity> constraints;

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
