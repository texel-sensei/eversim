#include "core/physics/constraints/static_collision_constraint.h"
#include "core/world/tile.h"
#include "core/utility/math.h"
#include "core/rendering/render_manager.h"

#include <vector>
#include "core/physics/constraints/angle_constraint.h"

using namespace std;

namespace eversim { namespace core { namespace physics {

	static_collision_constraint::static_collision_constraint(
		world::tile const& t, particle const& p, glm::vec2 n, glm::vec2 entry
	)
		: constraint(1), n(n), entry(entry), tile_(&t)
	{
		type = constraint_type::inequality;
		particles[0] = &p;
	}

	float static_collision_constraint::operator()() const
	{
		const auto pos = particles[0]->projected_position;
		return dot(pos - entry, n);
	}

	void static_collision_constraint::grad(utility::array_view<glm::vec2> out) const
	{
		EVERSIM_ASSERT(out.size() == get_arity());
		// (p.x - e.x)*n.x + (p.y - e.y)*n.y
		// <=>
		// p.x*n.x - e.x*n.x + p.y*n.y - e.y*n.y
		const auto pos = particles[0]->projected_position;
		out[0] = pos*n;
	}
}}}
