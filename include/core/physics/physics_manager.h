#pragma once
#include "core/physics/particle.h"
#include "core/physics/constraints/constraint_base.h"
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/body.h"

#include "core/utility/object_pool.h"

#include <boost/range/adaptor/filtered.hpp>
#include <boost/poly_collection/base_collection.hpp>
#include <boost/range.hpp>

#include <bitset>
#include <vector>
#include <memory>



namespace eversim { namespace core { namespace physics {
	class body_template;

	class physics_manager {
	public:
		constexpr static size_t max_constraint_arity = 3;
		using body_container = utility::object_pool<body>;

		body* add_body(body_template const&, glm::vec2 pos, float scale = 1.f);
		void remove_body(body* b);

		void set_particle_size(float f) { particle_radius = f; }
		float get_particle_size() const { return particle_radius; }

		template<typename C>
		void insert_constraint(C const& c)
		{
			constraints.insert(c);
		}

		void add_constraint(std::unique_ptr<constraint> c);

		template<typename... Cs>
		void register_constraint_types()
		{
			constraints.register_types<Cs...>();
		}

		auto get_bodies()
		{
			const auto range = boost::iterator_range<body_container::iterator>{ bodies.begin(), bodies.end() };
			const auto is_alive = [](auto const& b) {return b.is_alive(); };
			return range | boost::adaptors::filtered(is_alive);
		}
		auto get_bodies() const
		{
			auto range = boost::iterator_range<body_container::const_iterator>{ bodies.begin(), bodies.end() };
			const auto is_alive = [](auto const& b) {return b.is_alive(); };
			return range | boost::adaptors::filtered(is_alive);
		}

		void integrate(float dt);

		void set_gravity(glm::vec2 const& g) { gravity = g; }
		glm::vec2 get_gravity() const { return gravity; }

		void set_damping_coefficient(float f) { damping = 1-f; }
		float get_damping_coefficient() const { return 1-damping; }

		particle& get_particle(int idx) { return particles.at(idx); }
		std::vector<particle>& get_particles() { return particles; }

		size_t get_num_bodies() const { return bodies.size() - num_dead_bodies; }

		// debug functions
		void atomic_step(float dt);
		std::string get_step_name() const;
		bool finished_frame() const { return current_state == simulation_state::external; }
		void draw_constraints(std::bitset<max_constraint_arity> to_render = ~0UL);
		
		void cleanup_dead_bodies();

	private:
		int solver_iterations = 5;
		std::vector<particle> particles;
		std::vector<distance_constraint> collision_constraints;
		body_container bodies;
		glm::vec2 gravity = {0.f,-1.f};
		float damping = 0.99f;
		float particle_radius = 0.02f;
		
		int num_dead_bodies = 0;

		boost::base_collection<constraint> constraints;

		utility::array_view<particle> allocate_particles(size_t num);


		enum class simulation_state {
			external,
			damp,
			apply_velocity,
			check_collisions,
			constraint_iteration,
			apply_changes
		};
		simulation_state current_state = simulation_state::external;
		int current_iteration = 0;

		void apply_external_forces(float dt);
		void check_collisions();
		void damp_velocities();
		void project_constraints();
		void finalize_changes(float dt);
	};
}}}
