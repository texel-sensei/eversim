#include "core/rendering/camera.h"

namespace eversim {	namespace core { namespace rendering {

	void Camera::calc()
	{
		calc_P();
		calc_V();
	}

	void Camera::calc_P()
	{
		P = glm::fmat3(2.f / width_in_meters, 0, 0,
			0, (2.f * aspect_ratio) / width_in_meters, 0,
			-1, -1, 1);
	}

	void Camera::calc_V()
	{
		auto pos = position - glm::fvec2(
			width_in_meters / 2.f,
			width_in_meters / (2.f * aspect_ratio)
		);

		V = glm::fmat3(
			up_vector[1], -up_vector[0], 0,
			up_vector[0], up_vector[1], 0,
			-pos[0], -pos[1], 1
		);
	}


	Camera::Camera(const std::string& n,
		const glm::ivec2& resolution,
		const float width_in_meters,
		const glm::fvec2 position) :
		name(n),
		position(position),
		width_in_meters(width_in_meters)
	{
		set_resolution(resolution);
		calc();
	} 

	void Camera::use(ShaderProgram& program)
	{
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
		calc_V();
	}

	void Camera::set_position(const glm::fvec2& p)
	{
		position = p;
		calc_V();
	}

	glm::fvec2 Camera::get_position() const
	{
		return position;
	}

	void Camera::set_rotation(const float radians)
	{
		rotation = radians;
		up_vector = {0,1};
		rotate(radians);
	}

	void Camera::rotate(const float radians)
	{
		rotation += radians;
		glm::fmat2 R(std::cos(radians), std::sin(radians),
			-std::sin(radians), std::cos(radians));

		up_vector = R * up_vector;
		up_vector = glm::normalize(up_vector);
		calc_V();
	}

	void Camera::set_width_in_meters(const float m)
	{
		width_in_meters = m;
		calc();
	}

	void Camera::set_aspect_ratio(const float ar)
	{
		aspect_ratio = ar;
		calc();
	}

	void Camera::set_resolution(const glm::ivec2& resolution)
	{
		set_aspect_ratio(static_cast<float>(resolution[0]) / static_cast<float>(resolution[1]));
	}

	float Camera::get_width_in_meters() const
	{
		return width_in_meters;
	}

	float Camera::get_rotation() const
	{
		return rotation;
	}

	float Camera::get_aspect_ratio() const
	{
		return aspect_ratio;
	}

	std::string Camera::get_name() const
	{
		return name;
	}

	glm::fmat3 Camera::get_projection_matrix() const
	{
		return P;
	}

	glm::fmat3 Camera::get_view_matrix() const
	{
		return V;
	}
}}}