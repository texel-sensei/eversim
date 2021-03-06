#pragma once

#include "core/physics/constraints/constraint_base.h"
#include "core/physics/particle.h"

namespace eversim {namespace core {namespace world {
	struct tile;
}}}

namespace eversim { namespace core { namespace physics {

	class static_collision_constraint : public physics::constraint {
		friend class distance_constraint_factory;
	public:
		explicit static_collision_constraint(
			world::tile const& t, particle const& p, glm::vec2 n, glm::vec2 entry
		);

		float operator()() const override;
		void grad(utility::array_view<glm::vec2> out) const override;
		
		glm::vec2 normal() const { return n; }
		world::tile const* tile() const { return tile_; }

	private:
		glm::vec2 n;
		glm::vec2 entry;
		world::tile const* tile_;
	};

}}}
