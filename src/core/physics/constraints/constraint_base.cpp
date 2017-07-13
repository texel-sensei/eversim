#include "core/physics/constraints/constraint_base.h"
#include "core/physics/body.h"

namespace eversim { namespace core { namespace physics {

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
