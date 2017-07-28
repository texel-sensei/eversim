#include "core/physics/constraints/static_collision_constraint.h"
#include "core/world/tile.h"
#include "core/utility/math.h"
#include "core/rendering/render_manager.h"

#include <array>

using namespace std;

namespace eversim { namespace core { namespace physics {

	namespace {
		array<utility::line, 4> tile_lines(world::tile const& t)
		{
			const auto s = t.size()/2.f;
			const auto p = t.position();

			const auto TL = p + glm::vec2(-s, s);
			const auto BL = p + glm::vec2(-s,-s);
			const auto BR = p + glm::vec2( s,-s);
			const auto TR = p + glm::vec2( s, s);

			return {
				utility::line{TL,BL},
				utility::line{BL,BR},
				utility::line{BR,TR},
				utility::line{TR,TL}
			};
		}

	}

	static_collision_constraint::static_collision_constraint(
		world::tile const& t, particle const& p
	): constraint(1), tile_(&t)
	{
		type = constraint_type::inequality;
		particles[0] = &p;

		assert(t.point_inside(p.projected_position));

		auto full_particle_inside = t.point_inside(p.pos);
		const auto ray = utility::line{ p.pos, p.projected_position };
		const auto lines = tile_lines(t);
		utility::line const* l = nullptr;

		if(full_particle_inside)
		{
			float min_dist = std::numeric_limits<float>::max();
			for(int i = 0; i < lines.size(); ++i)
			{
				const auto dist = lines[i].distance_to_point(p.pos);
				if(dist < min_dist)
				{
					min_dist = dist;
					l = &lines[i];
				}
			}
			entry = l->closest_point(p.pos);
		} else
		{
			float t = -1;
			
			for(int i = 0; i < lines.size(); ++i)
			{
				const auto intersection = ray.intersect(lines[i]);
				if(intersection)
				{
					l = &lines[i];
					t = *intersection;
					break;
				}
			}
			
			entry = ray.lerp(t);
		}

		if (!l)
		{
			throw runtime_error{ "No intersection found!" };
		}
		n = l->normal();
	}

	float static_collision_constraint::operator()() const
	{
		return dot(particles[0]->projected_position - entry, n);
	}

	vector<glm::vec2> static_collision_constraint::grad() const
	{
		// (p.x - e.x)*n.x + (p.y - e.y)*n.y
		// <=>
		// p.x*n.x - e.x*n.x + p.y*n.y - e.y*n.y
		return {particles[0]->projected_position*n};
	}
}}}
