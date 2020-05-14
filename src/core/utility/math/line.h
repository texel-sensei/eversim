#pragma once

#include <glm/glm.hpp>
#include <boost/optional.hpp>

namespace eversim::core::utility::math {
	
	struct line {
		glm::vec2 start, end;

		// points to the right of the line
		glm::vec2 normal() const
		{
			const auto dir = direction();
			return { dir.y, -dir.x };
		}

		float length() const
		{
			return glm::length(end - start);
		}

		glm::vec2 delta() const
		{
			return end - start;
		}

		glm::vec2 direction() const
		{
			return normalize(delta());
		}

		glm::vec2 lerp(float t) const
		{
			return (1.f - t)*start + t*end;
		}

		float slope() const
		{
			const auto d = delta();
			return d.y / d.x;
		}

		/*
		 * Checks if lines *this and other intersect. If there is an intersection,
		 * then it returns the value 't' such that this->lerp(t) equals the intersection point.
		 * 
		 * If the lines do not intersect it returns an empty optional.
		 */
		boost::optional<float> intersect(line const& other) const;

		float distance_to_point(glm::vec2 p) const;

		/*
		 * Returns the point that has the least distance to p and lies on the line
		 */
		glm::vec2 closest_point(glm::vec2 p) const;
	};

}