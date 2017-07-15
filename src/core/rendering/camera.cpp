#include "core/rendering/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eversim {	namespace core { namespace rendering {
	Camera::Camera(const std::string& n, 
		const glm::fvec2 left_right,
		const glm::fvec2 bottom_top,
		const glm::fvec2 near_far,
		const glm::fvec2 position) :
		name(n)
	{
		P = glm::fmat3(2.f / (left_right[1] - left_right[0]), 0, 0,
						0, 2.f / (bottom_top[1] - bottom_top[0]),0,
						-1,-1,1);
		V = glm::fmat3(
			1,0,0,
			0,1,0,
			-position[0], -position[1], 1
		);
	}

	Camera::Camera(const Camera& c) :
		name(c.name), P(c.P), V(c.V)
	{}

	Camera::Camera(Camera&& c) :
		name(c.name)
	{
		std::swap(P, c.P);
		std::swap(V, c.V);
	}

	void Camera::use(ShaderProgram& program) const
	{
		//program.use();

		GLint location = glGetUniformLocation(program.getID(), "V");
		if (location == -1)
			LOG(INFO) << "Uniform name ""V"" does not exist";
		glUniformMatrix3fv(location, 1, GL_FALSE, &V[0][0]);

		location = glGetUniformLocation(program.getID(), "P");
		if (location == -1)
			LOG(INFO) << "Uniform name ""P"" does not exist";
		glUniformMatrix3fv(location, 1, GL_FALSE, &P[0][0]);

		//glUseProgram(0);
	}

	void Camera::translate(const glm::fvec2& t)
	{

	}

	void Camera::rotate(const float angle)
	{

	}
}}}