#include "core/utility/math/line.h"

namespace eversim::core::utility::math {

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

	float line::distance_to_point(glm::vec2 p) const
	{
		return glm::length(closest_point(p) - p);
	}

	glm::vec2 line::closest_point(glm::vec2 p) const
	{
		auto ps = p - start;
		if(dot(ps, delta()) <= 0)
		{
			return start;
		}
		ps = end - p;
		if(dot(ps, delta()) <= 0)
		{
			return end;
		}
		const auto e = p - start;
		return start + direction() * dot(e, direction());
		
	}
}
