#include "core/rendering/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eversim {	namespace core { namespace rendering {

	void Camera::calc()
	{
		P = glm::fmat3(2.f / (left_right[1] - left_right[0]), 0, 0,
			0, 2.f / (bottom_top[1] - bottom_top[0]), 0,
			-1, -1, 1);
		V = glm::fmat3(
			right_vector[0], right_vector[1], 0,
			up_vector[0], up_vector[1], 0,
			-position[0], -position[1], 1
		);
	}


	Camera::Camera(const std::string& n,
		const glm::fvec2 left_right,
		const glm::fvec2 bottom_top,
		const glm::fvec2 position) :
		name(n),
		left_right(left_right),
		bottom_top(bottom_top),
		up_vector({ 0,1 }),
		right_vector({ 1,0 }),
		position(position)
	{
		calc();
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

	void Camera::use(ShaderProgram& program)
	{
		calc();

		GLint location = glGetUniformLocation(program.getID(), "V");
		if (location == -1)
			LOG(INFO) << "Uniform name ""V"" does not exist";
		glUniformMatrix3fv(location, 1, GL_FALSE, &V[0][0]);

		location = glGetUniformLocation(program.getID(), "P");
		if (location == -1)
			LOG(INFO) << "Uniform name ""P"" does not exist";
		glUniformMatrix3fv(location, 1, GL_FALSE, &P[0][0]);
	}

	void Camera::translate(const glm::fvec2& t)
	{
		position += t;
	}

	void Camera::rotate(const float angle)
	{
		float a = (angle / 180.) * 3.141592653589;
		glm::fmat2 R(std::cos(a), std::sin(a),
			-std::sin(a), std::cos(a));

		up_vector = R * up_vector; 
		up_vector = glm::normalize(up_vector);

		right_vector = R * right_vector; 
		right_vector = glm::normalize(right_vector);
	}
}}}