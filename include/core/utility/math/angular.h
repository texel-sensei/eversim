#pragma once

#include "core/utility/math/constants.h"

#include <glm/detail/type_vec2.hpp>
#include <boost/operators.hpp>

#include <complex>
#include <cmath>

namespace eversim::core::utility::math
{
	
	struct rotation;

	struct orientation
			: boost::additive<orientation, rotation
			, boost::equality_comparable<orientation>
		>
	{
		using representation = std::complex<float>;

		explicit orientation(representation repr = {1,0}) : value(repr)
		{
			normalize();
		}

		/*
		 * Without this deleted c'tor something like 
		 * orientation(0) would use the complex number c'tor but deliver
		 * unexpected results
		 */
		explicit orientation(float) = delete;


		static orientation from_radians(float rad) {
			return orientation{{cosf(rad) , sinf(rad)}};
		}

		static orientation from_degrees(float deg)
		{
			return from_radians( deg * PI / 180.f );
		}

		static orientation from_direction(glm::vec2 const& v)
		{
			return orientation{ {v.x,v.y} };
		}

		glm::vec2 to_direction() const
		{
			return { value.real(), value.imag() };
		}

		float as_radians() const
		{
			return std::atan2(value.imag(), value.real());
		}

		float as_degrees() const
		{
			return as_radians() * 180.f / PI;
		}

		orientation& operator+=(rotation const& rot);
		orientation& operator-=(rotation const& rot);

		friend rotation operator-(orientation const& a, orientation const& b);

		friend bool operator==(orientation const& a, orientation const& b) noexcept
		{
			return a.value == b.value;
		}

		void normalize() { value /= abs(value); }

		friend orientation lerp(orientation const& a, orientation const& b, float t);
		
	private:
		representation value;
	};

	inline orientation normalize(orientation const& o)
	{
		auto c = o;
		c.normalize();
		return c;
	}
	inline orientation lerp(orientation const& a, orientation const& b, float t)
	{
		return normalize(orientation{ (1 - t) * a.value + t*b.value });
	}

	struct rotation 
			: boost::additive1<rotation
			, boost::equality_comparable<rotation>
		>
	{
		friend struct orientation;
		using representation = orientation::representation;

		explicit rotation(float theta) : value(cosf(theta), sinf(theta)){}
		explicit rotation(representation rep = {1,0}) : value(rep){}

		float as_radians() const
		{
			return std::atan2(value.imag(), value.real());
		}

		float as_degrees() const
		{
			return as_radians() * 180.f / PI;
		}

		rotation operator-() const
		{
			return rotation{ conj(value) };
		}

		rotation& operator+=(rotation const& other)
		{
			value *= other.value;
			return *this;
		}

		rotation& operator-=(rotation const& other)
		{
			*this += -other;
			return *this;
		}

		friend bool operator==(rotation const& a, rotation const& b) noexcept
		{
			return a.value == b.value;
		}

	private:
		representation value;
	};

	namespace literals
	{
		inline rotation operator"" _deg(long double d) {
			return rotation{ float(d * PI/180) };
		}
		inline rotation operator"" _deg(unsigned long long int l) {
			return rotation{ float(l * PI/180) };
		}
	}

	/*
	 * Returns the rotation, that maps the positive X-axis onto the vector
	 * from a to b
	 */
	inline rotation angle_between_points(glm::vec2 const& a, glm::vec2 const& b) noexcept
	{
		const auto dif = b - a;
		return rotation{ {dif.x, dif.y} };
	}

	inline orientation& orientation::operator+=(rotation const& rot)
	{
		value *= rot.value;
		return *this;
	}

	inline orientation& orientation::operator-=(rotation const& rot)
	{
		*this += -rot;
		return *this;
	}

	inline rotation operator-(orientation const& a, orientation const& b)
	{
		return rotation{ b.value / a.value };
	}
}
