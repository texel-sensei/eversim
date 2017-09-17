#include "core/physics/constraints/constraint_base.h"
#include "core/physics/body.h"
#include "core/physics/particle.h"
#include <algorithm>

namespace eversim { namespace core { namespace physics {

	body_offset_ptr::body_offset_ptr(particle const* p)
	{
		EVERSIM_ASSERT(p);
		base = p->owner;
		offset = p - base->get_particles().data();
	}

	bool constraint::is_alive() const
	{
		return std::all_of(
			particles.begin(),
			particles.end(),
			[](auto const& ptr)
			{
				return ptr.base->is_alive();
			}
		);
	}
}}}
