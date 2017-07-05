#include "core/physics/physics_manager.h"
#include "core/physics/constraint.h"
#include "core/rendering/render_manager.h"
#include <glm/gtx/norm.hpp>

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

		int num_iterations = 5;
		for (auto i = 0; i < num_iterations; ++i)
		{
			project_constraints();
		}

		for (auto& p : particles)
		{
			p.vel = (p.projected_position - p.pos) / dt;
			p.pos = p.projected_position;
		}
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
			rendering::draw_line(p->projected_position, p->projected_position+correction);
			p->projected_position += correction;
		}
	}

	void physics_manager::project_constraints()
	{
		for (auto const& c : std::get<0>(constraints))
		{
			project_single_constraint(*c);
		}
		for (auto const& c : std::get<1>(constraints))
		{
			project_single_constraint(*c);
			auto p0 = c->particles[0];
			auto p1 = c->particles[1];
			rendering::draw_line(p0->projected_position, p1->projected_position);
		}
	}
}}}
