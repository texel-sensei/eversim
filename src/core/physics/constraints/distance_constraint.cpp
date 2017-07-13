#include "core/physics/constraints/distance_constraint.h"
#include <istream>
#include "core/physics/body_template.h"

using namespace std;

namespace eversim { namespace core { namespace physics {

	constraint_factory::data_container distance_constraint_factory::parse(std::istream& data) const
	{
		data.exceptions(istream::badbit);
		float target_distance;
		data >> target_distance;
		if(data.fail())
		{
			target_distance = -1;
		}
		return make_shared<float>(target_distance);
	}

	std::unique_ptr<constraint> distance_constraint_factory::build(
		constraint_descriptor const& desc, body const* bdy
	) const {
		assert(desc.arity == 2);
		auto target = *reinterpret_cast<float*>(desc.extra_data.get());
		auto c = make_unique<distance_constraint>(target);
		c->stiffness = desc.stiffness;

		c->particles = {
			{ bdy, desc.particles[0] },
			{ bdy, desc.particles[1] }
		};

		if(c->distance <= 0)
		{
			c->distance = length(c->particles[0]->pos - c->particles[1]->pos);
		}

		return c;
	}
}}}
