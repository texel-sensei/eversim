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
		: boost::addable2<orientation, rotation>,
		boost::subtractable2<orientation, rotation>
	{
		using representation = std::complex<float>;

		explicit orientation(float rad) : value(cosf(rad), sinf(rad)){}
		explicit orientation(representation repr) : value(repr)
		{
			normalize();
		}

		static orientation from_degrees(float deg)
		{
			return orientation{ deg * PI / 180.f };
		}

		representation repr() const
		{
			return { value.real(), value.imag() };
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
	private:
		representation value;
	};

	struct rotation
	{
		friend struct orientation;
		using representation = orientation::representation;

		explicit rotation(float theta) : value(cosf(theta), sinf(theta)){}
		explicit rotation(representation rep) : value(rep){}

		rotation operator-() const
		{
			return rotation{ conj(value) };
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
		using namespace literals;
		return 0_deg;
	}
}
