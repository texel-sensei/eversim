#include "core/physics/constraints/constraint_base.h"
#include "core/physics/body.h"
#include "core/physics/particle.h"

namespace eversim { namespace core { namespace physics {

	body_offset_ptr::body_offset_ptr(particle const* p)
	{
		assert(p);
		base = p->owner;
		offset = p - base->particles.data();
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
}}}
