#include "core/physics/physics_manager.h"
#include "core/physics/constraints/constraint_base.h"
#include "core/physics/body_template.h"

#include "core/world/level.h"
#include "core/world/tile.h"
#include "core/world/tile_descriptor.h"

#include "core/rendering/render_manager.h"

#include "core/utility/spatial_hashmap.h"

#include <easylogging++.h>

#include <glm/detail/type_mat.hpp>
#include <glm/gtx/norm.hpp>

#include <boost/range/adaptor/indirected.hpp>
#include <boost/range/join.hpp>

#include <algorithm>
#include <bitset>


using namespace std;

namespace eversim { namespace core { namespace physics {

	body* physics_manager::add_body(body_template const& templ, glm::vec2 pos, glm::vec2 scale)
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

	void physics_manager::remove_body(body* b)
	{
		if (!b) return;
		b->kill();
		num_dead_bodies++;
	}

	void physics_manager::set_level(world::level const* l)
	{
		level = l;
	}

	void physics_manager::integrate(float dt)
	{
		apply_external_forces(dt);
		damp_velocities();
		for (auto& p : particles)
		{
			if (!p.is_alive()) continue;
			p.projected_position = p.pos + dt * p.vel;
		}

		check_collisions();

		for (auto i = 0; i < solver_iterations; ++i)
		{
			project_constraints();
		}

		finalize_changes(dt);
		call_events();
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
			current_state = simulation_state::call_events;
			break;
		case simulation_state::call_events: 
			call_events();
			current_state = simulation_state::external;
			break;
		default:;
			assert(!"unknown state!");
		}
	}

	void physics_manager::check_collisions()
	{
		collision_constraints.clear();
		static_collision_constraints.clear();

		static thread_local utility::spatial_hashmap<particle*> possible_collisions{2*particle_radius};
		possible_collisions.reset(particles.size(), particles.size() * 2);
		for(auto& p : particles)
		{
			if (!p.is_alive()) continue;
			possible_collisions.insert(p.pos, &p);

			if(level)
			{
				particle_tile_collision(p);
			}
		}
		for (auto& p : particles)
		{
			if (!p.is_alive()) continue;
			auto index = possible_collisions.get_index(p.pos);
			for(int dx = -1; dx <=1; ++dx)
			{
				for(int dy = -1; dy <= 1; ++dy)
				{
					for(auto* other : possible_collisions.get_cell(index + glm::ivec2(dx,dy)))
					{
						if (other >= &p || other->owner == p.owner)
							continue;
						if(length(p.pos - other->pos) < 2*particle_radius)
						{
							auto c = distance_constraint{2*particle_radius};
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
		case simulation_state::call_events: return "Calling events";
		default: return "Invalid state!";
		}
	}

	auto physics_manager::get_constraints() const
		-> boost::any_range<constraint const&, boost::forward_traversal_tag>
	{
		using namespace boost;
		using namespace boost::range;
		const auto r1 = join(constraints, collision_constraints);
		const auto r2 = join(r1, static_collision_constraints);
		return r2;
	}

	void physics_manager::add_constraint(unique_ptr<constraint> c)
	{
		constraints.insert(*c);
	}
	

	utility::array_view<particle> physics_manager::allocate_particles(size_t num)
	{
		const auto oldsize = particles.size();
		const auto old_capacity = particles.capacity();

		const auto* old_begin = particles.data();

		if (oldsize + num > old_capacity)
		{
			auto newvec = vector<particle>();
			newvec.reserve(max(particles.capacity() * 2, oldsize + num));

			auto* ptr = newvec.data();
			for (auto& b : bodies)
			{
				if (!b.is_alive())
					continue;

				newvec.insert(newvec.end(), b.particles.begin(), b.particles.end());
				const auto s = b.particles.size();
				b.particles = utility::make_array_view(ptr, s);
				ptr += s;
			}
			particles.swap(newvec);
		}
		particles.resize(oldsize + num);
		return utility::make_array_view(particles).slice(oldsize, 0);
	}

	void physics_manager::cleanup_dead_bodies()
	{
		for(auto it = constraints.begin(); it != constraints.end(); ++it)
		{
			if(!it->is_alive())
			{
				it = constraints.erase(it);
			}
		}
		for(auto& b : bodies)
		{
			if(!b.is_alive())
			{
				for(auto& p : b.particles)
				{
					p.owner = nullptr;
				}
				bodies.erase(bodies.locate(&b));
			}
		}
		num_dead_bodies = 0;
	}

	void physics_manager::apply_external_forces(float dt)
	{
		for (auto& p : particles)
		{
			if (!p.is_alive()) continue;
			const auto* bdy = p.owner;
			const auto body_movement = bdy->position - bdy->old_position;
			p.pos += body_movement;

			const auto body_acceleration = bdy->velocity - bdy->old_velocity;
			p.vel += body_acceleration;

			if (p.inv_mass == 0.f)
				continue;

			p.vel += dt * gravity * p.owner->gravity_scale;
		}
		for(auto& b : get_bodies())
		{
			b.position = {};
			b.velocity = {};
		}
	}

	void physics_manager::damp_velocities()
	{
		for (auto& p : particles)
		{
			if (!p.is_alive()) continue;
			p.vel *= damping; // TODO: improve
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

			static thread_local glm::vec2 cache[physics_manager::max_constraint_arity];
			auto grad = utility::make_array_view(cache).slice_n(0, c.get_arity());
			c.grad(grad);

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
				if(glm::length(correction) > 0.5f)
				{
					LOG(WARNING) << "Way too big step! (" << correction.x << ", " << correction.y <<")";
				}
				p->projected_position += correction;
			}
		}
	}

	void physics_manager::project_constraints()
	{
		for (auto const& c : constraints)
		{
			project_single_constraint(c, solver_iterations);
		}
		for(auto const& c : collision_constraints)
		{
			project_single_constraint(c, solver_iterations);
		}
		for(auto const& c : static_collision_constraints)
		{
			project_single_constraint(c, solver_iterations);
		}
	}

	void physics_manager::finalize_changes(float dt)
	{
		for (auto& p : particles)
		{
			if (!p.is_alive()) continue;
			p.vel = (p.projected_position - p.pos) / dt;
			p.pos = p.projected_position;

			auto* const b = p.owner;
			b->velocity += p.vel;
			b->position += p.pos;
		}
		for(auto& b : get_bodies())
		{
			b.old_position = b.position = b.position/float(b.particles.size());
			b.old_velocity = b.velocity = b.velocity/float(b.particles.size());
		}
	}

	void physics_manager::call_events()
	{
		using bodypair = pair<body*, body*>;
		map<bodypair, events::dyn_col_list> bb_collisions;
		for(auto& c : collision_constraints)
		{
			auto p1 = c.particles[0];
			auto p2 = c.particles[1];

			assert(p1.base != p2.base && "Collision between particles of the same body is not allowed!");

			if(p1.base > p2.base)
			{
				swap(p1, p2);
			}

			auto key = make_pair( p1->owner, p2->owner );
			bb_collisions[key].push_back({ p1.resolve(), p2.resolve() });
		}

		map<body*, events::static_col_list> bl_collisions;
		for(auto& c : static_collision_constraints)
		{
			auto& p = *c.particles[0];
			auto& l = bl_collisions[p.owner];
			
			l.push_back({&p, c.tile(), c.normal()});
		}

		for(auto& p : bb_collisions)
		{
			body* a;
			body* b;
			tie(a, b) = p.first;

			auto const& list = p.second;
			body_body_collision_event(a, b, list);
		}

		for(auto& p : bl_collisions)
		{
			body_level_collision_event(p.first, p.second);
		}
	}



	void physics_manager::particle_tile_collision(particle& p)
	{
		assert(level);
		
		const auto pos = p.projected_position;

		auto const& t = level->get_tile_by_pos(pos);

		const auto tile_coord = t.to_tile_coordinates(pos);
		int dx = (tile_coord.x > 0) ? 1 : -1;
		int dy = (tile_coord.y > 0) ? 1 : -1;

		world::tile const* tiles[] = {
			&t,
			t.get_neighbour({ dx, 0 }),
			t.get_neighbour({  0,dy }),
			t.get_neighbour({ dx,dy }),
		};
		
		for(auto const* tile : tiles)
		{
			if(!tile || !tile->has_collision())
				continue;

			check_tile_collisions(p, *tile);
		}
	}

	void physics_manager::check_tile_collisions(particle const& particle, const world::tile& tile)
	{
		const auto mpos = particle.pos - tile.position();
		const auto mprojpos = particle.projected_position - tile.position();
		const auto ray = utility::line{ mpos, mprojpos };

		for(auto const& side : tile.get_collision_shape())
		{
			const auto normal = side.normal();

			if (dot(particle.vel, normal) > 0)
				continue;

			if(auto intersection = ray.intersect(side))
			{
				const auto entry = ray.lerp(*intersection) + tile.position();

				static_collision_constraints.emplace_back(tile, particle, normal, entry);
			}

			const auto dist = side.distance_to_point(mpos);
			if(dist < 0.1f * tile.size())
			{
				const auto entry = side.closest_point(mpos) + tile.position();

				static_collision_constraints.emplace_back(tile, particle, normal, entry);
			}
		}
		
	}

}}}
