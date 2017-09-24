#include "core/physics/particle.h"
#include "core/physics/body.h"

namespace eversim::core::physics {
	using namespace utility::math;

	bool particle::is_alive() const
	{
		return owner && owner->is_alive(); 
	}

	void particle::update_relative_pose()
	{
		const auto east = orientation::from_degrees(0);
		base_orientation = east + angle_between_points(owner->position, this->pos);
	}
}
