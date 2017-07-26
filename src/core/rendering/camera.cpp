#include "core/rendering/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eversim {	namespace core { namespace rendering {

	void Camera::calc()
	{
		left_right = glm::fvec2(0, width_in_meters);
		bottom_top = glm::fvec2(0, width_in_meters / aspect_ratio);
		auto pos = position - glm::fvec2(
			left_right[1] / 2.f,
			bottom_top[1] / 2.f
		);
		P = glm::fmat3(2.f / (left_right[1] - left_right[0]), 0, 0,
			0, 2.f / (bottom_top[1] - bottom_top[0]), 0,
			-1, -1, 1);
		V = glm::fmat3(
			right_vector[0], right_vector[1], 0,
			up_vector[0], up_vector[1], 0,
			-pos[0], -pos[1], 1
		);
	}


	Camera::Camera(const std::string& n,
		const glm::fvec2 left_right,
		const glm::fvec2 bottom_top,
		const float width_in_meters,
		const glm::fvec2 position) :
		name(n),
		aspect_ratio((left_right[1]-left_right[0])/(bottom_top[1]-bottom_top[0])),
		left_right(left_right),
		bottom_top(bottom_top),
		up_vector({ 0,1 }),
		right_vector({ 1,0 }),
		position(position),
		width_in_meters(width_in_meters)
	{
		calc();
		//LOG(INFO) << "camera with aspect ratio = " << aspect_ratio << "\n" << "screen size in meters = " << width_in_meters << "x" << width_in_meters / aspect_ratio;
	} 

	void Camera::use(ShaderProgram& program)
	{
		calc();

		auto location = glGetUniformLocation(program.getID(), "V");
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

	void Camera::set_width_in_meters(const float m)
	{
		width_in_meters = m;
	}

	float Camera::get_width_in_meters() const
	{
		return width_in_meters;
	}
}}}