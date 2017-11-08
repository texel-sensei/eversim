#include "core/physics/body.h"
#include "core/physics/particle.h"
#include <numeric>

using namespace std;
using namespace glm;

namespace eversim::core::physics {
	body::body(utility::array_view<particle> ps)
		: particles(ps)
	{
		
	}

	void body::init(
		utility::array_view<particle> ps
	){
		this->particles = ps;
		for (auto& p : particles)
		{
			p.owner = this;
			p.update_relative_pose();
		}
	}
}
