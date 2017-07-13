#include "core/physics/body.h"
#include "core/physics/particle.h"

namespace eversim { namespace core { namespace physics {
	particle& body::get_particle(size_t idx) const
	{
		assert(idx < particles.size());
		return particles[idx];
	}
}}}
