#include "core/physics/physics_manager.h"
#include "core/physics/constraints/constraint_base.h"
#include "core/rendering/render_manager.h"
#include <glm/gtx/norm.hpp>
#include <bitset>

using namespace std;

namespace eversim { namespace core { namespace physics {

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

	void physics_manager::draw_constraints(bitset<max_constraint_arity> to_render)
	{
		for (auto const& c : constraints) {
			if (!to_render[c->get_arity()])
				continue;
			for (int i = 0; i < c->get_arity(); ++i)
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

	void physics_manager::apply_external_forces(float dt)
	{
		for (auto& p : particles)
		{
			if (p.inv_mass == 0.f)
				continue;
			p.vel += dt * glm::vec2{0,-1}; // TODO: don't hardcode, support for other forces
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
		void project_single_constraint(constraint const& c, int solver_iterations)
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

			auto grad = c.grad();

			const auto sum = [&]
			{
				auto sum = 0.f;
				for (auto i = 0; i < c.get_arity(); ++i)
				{
					sum += c.particles[i]->inv_mass * glm::length2(grad[i]);
				}
				return sum;
			}();
			const auto scale = err / sum;

			const auto k = 1.f - powf(1.f - c.stiffness, 1.f / solver_iterations);

			for (auto i = 0; i < c.get_arity(); ++i)
			{
				auto& p = c.particles[i];
				const auto correction = -scale * p->inv_mass * grad[i] * k;
				rendering::draw_line(p->projected_position, p->projected_position + correction,60);
				p->projected_position += correction;
			}
		}
	}

	void physics_manager::project_constraints()
	{
		for(auto const& c : constraints)
		{
			project_single_constraint(*c, solver_iterations);
		}	
	}
}}}
