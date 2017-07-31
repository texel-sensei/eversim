#include "core/physics/constraints/static_collision_constraint.h"
#include "core/world/tile.h"
#include "core/utility/math.h"
#include "core/rendering/render_manager.h"

#include <vector>

using namespace std;

namespace eversim { namespace core { namespace physics {

	static_collision_constraint::static_collision_constraint(
		world::tile const& t, particle const& p
	): constraint(1), tile_(&t)
	{
		type = constraint_type::inequality;
		particles[0] = &p;

		assert(t.point_inside(p.projected_position));

		auto full_particle_inside = t.point_inside(p.pos);

		// calculate ray, centered at (0,0) instead of tile position
		const auto mpos = p.pos - t.position();
		const auto mprojpos = p.projected_position - t.position();
		const auto ray = utility::line{ mpos, mprojpos };


		const auto lines = t.get_collision_shape();
		utility::line const* l = nullptr;

		const auto tp = t.position();

		if(full_particle_inside || ray.length() < 0.001f)
		{
			float min_dist = numeric_limits<float>::max();
			for(int i = 0; i < lines.size(); ++i)
			{
				const auto dist = lines[i].distance_to_point(mpos);
				if(dist < min_dist)
				{
					min_dist = dist;
					l = &lines[i];
				}
			}
			entry = l->closest_point(mpos) + t.position();
		} else
		{
			float f = -1;
			for(int i = 0; i < lines.size(); ++i)
			{
				const auto intersection = ray.intersect(lines[i]);
				if(intersection)
				{
					l = &lines[i];
					f = *intersection;
					break;
				}
			}
			
			entry = ray.lerp(f) + t.position();
		}

		if (!l)
		{
			throw runtime_error{ "No intersection found!" };
		}
		n = l->normal();
	}

	float static_collision_constraint::operator()() const
	{
		const auto pos = particles[0]->projected_position;
		return dot(pos - entry, n);
	}

	vector<glm::vec2> static_collision_constraint::grad() const
	{
		// (p.x - e.x)*n.x + (p.y - e.y)*n.y
		// <=>
		// p.x*n.x - e.x*n.x + p.y*n.y - e.y*n.y
		const auto pos = particles[0]->projected_position;
		return {pos*n};
	}
}}}
