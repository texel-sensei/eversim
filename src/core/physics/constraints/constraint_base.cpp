#include "core/physics/constraints/constraint_base.h"
#include "core/physics/body.h"
#include "core/physics/particle.h"
#include <algorithm>

namespace eversim { namespace core { namespace physics {

	body_offset_ptr::body_offset_ptr(particle const* p)
	{
		assert(p);
		base = p->owner;
		offset = p - base->get_particles().data();
	}

	particle* body_offset_ptr::resolve() const
	{
		return &base->get_particle(offset);
	}

	particle* body_offset_ptr::operator->() const
	{
		return resolve();
	}

	particle& body_offset_ptr::operator*() const
	{
		return *resolve();
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
