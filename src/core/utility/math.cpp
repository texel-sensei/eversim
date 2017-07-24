#include "core/utility/math.h"

namespace eversim { namespace core { namespace utility {

	boost::optional<float> line::intersect(line const& other) const
	{
		const auto A = glm::mat2(delta(), -other.delta());
		const auto b = other.start - start;

		// solve A*x = b
		const auto inv_det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
		if(inv_det == 0)
		{
			return {};
		}
		const auto sol = inverse(A) * b;

		// check if intersection is inside of line segments
		if(sol.x < 0 || sol.x > 1 || sol.y < 0 || sol.y > 1)
		{
			return {};
		}
		return sol.x;
	}
}}}
