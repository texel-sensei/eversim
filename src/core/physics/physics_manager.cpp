#include "core/physics/physics_manager.h"
#include "core/physics/constraints/constraint_base.h"
#include "core/rendering/render_manager.h"
#include <glm/gtx/norm.hpp>
#include <bitset>
#include "core/physics/body_template.h"
#include <easylogging++.h>
#include <glm/detail/type_mat.hpp>
#include "core/utility/spatial_hashmap.h"

using namespace std;

namespace eversim { namespace core { namespace physics {

	body* physics_manager::add_body(body_template const& templ, glm::vec2 pos, float scale)
	{
		// create body
		auto* bdy = bodies.emplace();

		// reserve space for new particles

		auto const& p_tmpl = templ.particles;
		bdy->particles = allocate_particles(p_tmpl.size());
		bdy->position = bdy->old_position = pos;

		transform(
			p_tmpl.begin(), p_tmpl.end(),
			bdy->particles.begin(),
			[&](particle_descriptor const& desc)
		{
			particle p;
			p.pos = desc.pos * scale + pos;
			p.owner = bdy;
			p.inv_mass = 1.f / desc.mass;
			return p;
		});

		// create constraints
		for (auto const& cd : templ.constraints)
		{
			auto& loader = cd.factory;
			auto constraint = loader->build(cd, bdy);
			add_constraint(move(constraint));
		}

		return bdy;
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

		check_collisions();

		for (auto i = 0; i < solver_iterations; ++i)
		{
			project_constraints();
		}

		finalize_changes(dt);
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
			current_state = simulation_state::check_collisions;
			break;
		case simulation_state::check_collisions:
			check_collisions();
			current_state = simulation_state::constraint_iteration;
			break;
		case simulation_state::constraint_iteration:
			if (current_iteration < solver_iterations)
			{
				project_constraints();
				current_iteration++;
			} else
			{
				current_iteration = 0;
				current_state = simulation_state::apply_changes;
			}
			break;
		case simulation_state::apply_changes:
			finalize_changes(dt);
			current_state = simulation_state::external;
			break;
		default: ;
			assert(!"unknown state!");
		}
	}

	void physics_manager::check_collisions()
	{
		collision_constraints.clear();
		utility::spatial_hashmap<particle*> possible_collisions{0.1f};
		for(auto& p : particles)
		{
			possible_collisions.insert(p.pos, &p);
		}
		for (auto& p : particles)
		{
			auto index = possible_collisions.get_index(p.pos);
			for(int dx = -1; dx <=1; ++dx)
			{
				for(int dy = -1; dy <= 1; ++dy)
				{
					for(auto* other : possible_collisions.get_cell(index + glm::ivec2(dx,dy)))
					{
						if (other >= &p || other->owner == p.owner)
							continue;
						if(length(p.pos - other->pos) < 0.05f)
						{
							rendering::draw_line(p.pos, other->pos, 1);
							auto c = distance_constraint{0.05f};
							c.set_type(constraint_type::inequality);
							c.particles = {&p, other};
							collision_constraints.emplace_back(c);
							
						}
					}
				}
			}
		}
	}

	string physics_manager::get_step_name() const
	{
		switch (current_state)
		{
		case simulation_state::external: return "Applying external forces";
		case simulation_state::damp: return "Damping velocity";
		case simulation_state::apply_velocity: return "Moving particles by v*dt";
		case simulation_state::constraint_iteration:
			return "Constraint solver " + to_string(current_iteration) + "/" + to_string(solver_iterations);
		case simulation_state::apply_changes: return "Moving particles to projected pos";
		case simulation_state::check_collisions: return "Checking collisions";
		default: return "Invalid state!";
		}
	}

	void physics_manager::draw_constraints(bitset<max_constraint_arity> to_render)
	{
		for (auto const& c : constraints)
		{
			if (!to_render[c->get_arity()-1])
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

	void physics_manager::add_constraint(unique_ptr<constraint> c)
	{
		constraints.push_back(move(c));
	}

	utility::array_view<particle> physics_manager::allocate_particles(size_t num)
	{
		const auto oldsize = particles.size();
		const auto old_capacity = particles.capacity();

		particles.resize(oldsize + num);

		if (oldsize + num > old_capacity)
		{
			auto* ptr = particles.data();
			for (auto& b : bodies)
			{
				auto s = b.particles.size();
				b.particles = utility::make_array_view(ptr, s);
				ptr += s;
			}
		}
		return utility::make_array_view(particles).slice(oldsize, 0);
	}

	void physics_manager::apply_external_forces(float dt)
	{
		for (auto& p : particles)
		{
			const auto* bdy = p.owner;
			const auto body_movement = bdy->position - bdy->old_position;
			p.pos += body_movement;

			const auto body_acceleration = bdy->velocity - bdy->old_velocity;
			p.vel += body_acceleration;

			if (p.inv_mass == 0.f)
				continue;

			p.vel += dt * glm::vec2{0,-1}; // TODO: don't hardcode, support for other forces
		}
		for(auto& b : bodies)
		{
			b.position = {};
			b.velocity = {};
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
					sum += c.particles[i]->inv_mass * length2(grad[i]);
				}
				return sum;
			}();
			const auto scale = err / sum;

			const auto k = 1.f - powf(1.f - c.stiffness, 1.f / solver_iterations);

			for (auto i = 0; i < c.get_arity(); ++i)
			{
				auto& p = c.particles[i];
				const auto correction = -scale * p->inv_mass * grad[i] * k;
				rendering::draw_line(p->projected_position, p->projected_position + correction, 60);
				p->projected_position += correction;
			}
		}
	}

	void physics_manager::project_constraints()
	{
		for (auto const& c : constraints)
		{
			project_single_constraint(*c, solver_iterations);
		}
		for(auto const& c : collision_constraints)
		{
			project_single_constraint(c, solver_iterations);
		}
	}

	void physics_manager::finalize_changes(float dt)
	{
		for (auto& p : particles)
		{
			p.vel = (p.projected_position - p.pos) / dt;
			p.pos = p.projected_position;

			auto* const b = p.owner;
			b->velocity += p.vel;
			b->position += p.pos;
		}
		for(auto& b : bodies)
		{
			b.old_position = b.position = b.position/float(b.particles.size());
			b.old_velocity = b.velocity = b.velocity/float(b.particles.size());
		}
	}
}}}
