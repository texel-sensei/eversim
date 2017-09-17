#include "core/physics/constraints/angle_constraint.h"
#include <istream>
#include "core/physics/body_template.h"

using namespace std;

namespace eversim {
	namespace core {
		namespace physics {

			constraint_factory::data_container angle_constraint_factory::parse(std::istream& data) const
			{
				data.exceptions(istream::badbit);
				float target_angle;
				data >> target_angle;
				if (data.fail())
				{
					target_angle = -2;
				}
				return target_angle;
			}

			std::unique_ptr<constraint> angle_constraint_factory::build(
				constraint_descriptor const& desc, body const* bdy
			) const {
				EVERSIM_ASSERT(desc.arity == 3);
				auto target = boost::any_cast<float>(desc.extra_data);
				auto c = make_unique<angle_constraint>(target);
				c->stiffness = desc.stiffness;

				c->particles = {
					{ bdy, desc.particles[0] },
					{ bdy, desc.particles[1] },
					{ bdy, desc.particles[2] }
				};

				if (c->target_angle == -2)
				{
					auto v1 = normalize(c->particles[0]->pos - c->particles[1]->pos);
					auto v2 = normalize(c->particles[2]->pos - c->particles[1]->pos);
					c->target_angle = acosf(dot(v1,v2));
				}

				return c;
			}
		}
	}
}
