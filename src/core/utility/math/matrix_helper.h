#pragma once
#include <glm/glm.hpp>

namespace eversim {namespace core{ namespace utility {

	inline glm::mat3 translation(glm::vec2 t)
	{
		auto M = glm::mat3();
		M[2] = glm::vec3(t,1.f);
		return M;
	}
	
	inline glm::mat3 rotation(float a)
	{
		return glm::mat3(
			cos(a), -sin(a), 0,
			sin(a), cos(a), 0,
			0, 0, 1
		);
	}

	inline glm::mat3 scale(glm::vec2 s)
	{
		auto M = glm::mat3();
		M[0][0] = s.x;
		M[1][1] = s.y;
		return M;
	}
} } }