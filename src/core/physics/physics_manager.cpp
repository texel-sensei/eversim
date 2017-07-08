#include "core/physics/physics_manager.h"
#include "core/physics/constraint.h"
#include "core/rendering/render_manager.h"
#include <glm/gtx/norm.hpp>
#include <bitset>

namespace eversim { namespace core { namespace physics {

	namespace {
		template <typename Constraints, typename Func, size_t... Is>
		void iterate(Constraints const& c, std::index_sequence<Is...>, Func&& fun)
		{
			std::initializer_list<int>{
				(std::forward<Func>(fun)(std::get<Is>(c), Is), 0)...
			};
		}
	}

	void physics_manager::add_particle(particle const& p)
	{
		particles.push_back(p);
	}

	void physics_manager::integrate(float dt)
	{
		apply_external_forces(dt);
		damp_velocities();
		for (auto& p : particles)
		{
			p.projected_position = p.pos + dt * p.vel;
		}

		for (auto i = 0; i < solver_iterations; ++i)
		{
			project_constraints();
		}

		for (auto& p : particles)
		{
			p.vel = (p.projected_position - p.pos) / dt;
			p.pos = p.projected_position;
		}
	}

	void physics_manager::atomic_step(float dt)
	{
		switch (current_state)
		{
		case simulation_state::external:
			apply_external_forces(dt);
			current_state = simulation_state::damp;
			break;
		case simulation_state::damp:
			damp_velocities();
			current_state = simulation_state::apply_velocity;
			break;
		case simulation_state::apply_velocity:
			for (auto& p : particles)
			{
				p.projected_position = p.pos + dt * p.vel;
			}
			current_state = simulation_state::constraint_iteration;
			break;
		case simulation_state::constraint_iteration:
			if(current_iteration < solver_iterations)
			{
				project_constraints();
				current_iteration++;
			}else
			{
				current_iteration = 0;
				current_state = simulation_state::apply_changes;
			}
			break;
		case simulation_state::apply_changes:
			for (auto& p : particles)
			{
				p.vel = (p.projected_position - p.pos) / dt;
				p.pos = p.projected_position;
			}
			current_state = simulation_state::external;
			break;
		default: ;
			assert(!"unknown state!");
		}
	}

	void physics_manager::draw_constraints(std::bitset<max_constraint_arity> to_render)
	{
		iterate(constraints, std::make_index_sequence<max_constraint_arity>{}, [to_render](auto const& cs, size_t I)
		{
			if(to_render[I])
			{
				for (auto const& c : cs) {
					for (int i = 0; i < c->arity(); ++i)
					{
						for (int j = 0; j < i; ++j)
						{
							auto p1 = c->particles[i];
							auto p2 = c->particles[j];
							rendering::draw_line(p1->projected_position, p2->projected_position);
						}
					}
				}
			}
		});
	}


	void physics_manager::apply_external_forces(float dt)
	{
		for (auto& p : particles)
		{
			if (p.inv_mass == 0.f)
				continue;
			p.vel += dt * glm::vec2{0,-1};
		}
	}

	void physics_manager::damp_velocities()
	{
		for (auto& p : particles)
		{
			p.vel *= 0.99; // TODO: improve
		}
	}

	namespace {
		template <size_t N>
		void project_single_constraint(constraint<N> const& c)
		{
			const auto err = c();
			switch (c.get_type())
			{
			case constraint_type::equality:
				{
					if (err == 0)
						return;
					break;
				}
			case constraint_type::inequality:
				{
					if (err >= 0)
						return;
					break;
				}
			default:
				{
					assert(!"Unhandled constraint type!");
				}
			}

			const auto grad = c.grad();

			const auto sum = [&]
			{
				auto sum = 0.f;
				for (auto i = 0; i < N; ++i)
				{
					sum += c.particles[i]->inv_mass * length2(grad[i]);
				}
				return sum;
			}();
			const auto scale = err / sum;

			for (auto i = 0; i < N; ++i)
			{
				auto& p = c.particles[i];
				const auto correction = -scale * p->inv_mass * grad[i];
				rendering::draw_line(p->projected_position, p->projected_position + correction,60);
				p->projected_position += correction;
			}
		}
	}

	void physics_manager::project_constraints()
	{
		iterate(constraints, std::make_index_sequence<max_constraint_arity>{}, [](auto&& cs, auto){
			for(auto const& c : cs)
			{
				project_single_constraint(*c);
			}
		});
	}
}}}
