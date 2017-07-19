#include "core/physics/particle.h"
#include "core/physics/body.h"

namespace eversim { namespace core { namespace physics {
	bool particle::is_alive() const
	{
		return owner && owner->is_alive(); 
	}
}}}
