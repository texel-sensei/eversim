#pragma once

#include "core/utility/math/angular.h"

#include <glm/glm.hpp>

namespace eversim { namespace core { namespace physics {
	
	struct body;

	struct particle {
		glm::vec2 pos;
		glm::vec2 vel;

		float inv_mass = 1.f;

		glm::vec2 projected_position;
		body* owner;

		bool is_alive() const;

		void update_relative_pose();
		utility::math::orientation get_base_orientation() const
		{
			return base_orientation;
		}
	private:
		/// The orientation of the particle compared to the
		/// body origin
		utility::math::orientation base_orientation;
	};

}}}
