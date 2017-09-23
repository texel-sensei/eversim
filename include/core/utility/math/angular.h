#pragma once

#include "core/utility/math/constants.h"

#include <glm/detail/type_vec2.hpp>
#include <boost/operators.hpp>

#include <complex>
#include <cmath>

namespace eversim::core::utility::math
{
	
	namespace detail
	{
		template<typename Derived>
		struct angular_base 
			: boost::equality_comparable<angular_base<Derived>>
		{
			using representation = std::complex<float>;

			/*
			* Without this deleted c'tor something like
			* orientation(0) would use the complex number c'tor but deliver
			* unexpected results
			*/
			explicit angular_base(float) = delete;


			static Derived from_radians(float rad) {
				return from_direction( { cosf(rad) , sinf(rad) } );
			}

			static Derived from_degrees(float deg)
			{
				return from_radians(deg * PI / 180.f);
			}

			static Derived from_direction(glm::vec2 const& v)
			{
				return Derived{ { v.x,v.y } };
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

			friend bool operator==(angular_base const& a, angular_base const& b) noexcept
			{
				return a.value == b.value;
			}

			void normalize() { value /= abs(value); }

			friend Derived lerp(angular_base const& a, angular_base const& b, float t)
			{
				const auto d = (1 - t)*a.to_direction() + t*b.to_direction();
				return from_direction(d);
			}

		protected:
			representation value;

			angular_base() : angular_base({ 1,0 }) {}
			explicit angular_base(representation repr) : value(repr)
			{
				normalize();
			}
		};
	}

	class rotation : public detail::angular_base<rotation>
		, boost::additive1<rotation
		, boost::multipliable<rotation, float
		, boost::dividable<rotation, float
		, boost::equality_comparable<rotation>
		>>>
	{
		using base = detail::angular_base<rotation>;
		friend class base;
		friend class orientation;
	public:
		rotation() = default;

		friend rotation angle_between_points(glm::vec2 const& a, glm::vec2 const& b) noexcept;

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

		rotation& operator*=(float f)
		{
			value = pow(value, f);
			return *this;
		}

		rotation& operator/=(float f)
		{
			value = pow(value, 1/f);
			return *this;
		}

	private:
		explicit rotation(representation re):base(re){}
	};

	class orientation : public detail::angular_base<orientation>
			, boost::additive<orientation, rotation
			, boost::equality_comparable<orientation>
		>
	{
		using base = detail::angular_base<orientation>;
		friend class base;
	public:
		orientation() = default;

		orientation& operator+=(rotation const& rot);
		orientation& operator-=(rotation const& rot);

		rotation operator-(orientation const& b) const
		{
			return rotation{ b.value / this->value };
		}

	private:
		explicit orientation(representation re) : base(re) {}
	};

	namespace literals
	{
		inline rotation operator"" _deg(long double d) {
			return rotation::from_degrees(d);
		}
		inline rotation operator"" _deg(unsigned long long int l) {
			return rotation::from_degrees(l);
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
}
