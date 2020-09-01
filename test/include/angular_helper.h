#pragma once

#include "core/utility/math/angular.h"

#include <catch2/catch.hpp>
#include <sstream>

namespace detail{
	template<typename T>
	class base_angular_equals : public Catch::MatcherBase<T> {
	public:
		explicit base_angular_equals(T v) : expected(v) {}

		bool match(T const& v) const override {
			return v.as_radians() == Approx(expected.as_radians());
		}

		std::string describe() const override
		{
			std::ostringstream ss;
			ss << "is approximate the same as ("
			<< Catch::StringMaker<T>::convert(expected) << ")";
			return ss.str();
		}
	private:
		T expected;
	};
}

using orientation_equals = ::detail::base_angular_equals<eversim::core::utility::math::orientation>;
using rotation_equals = ::detail::base_angular_equals<eversim::core::utility::math::rotation>;

inline orientation_equals is_approx(eversim::core::utility::math::orientation expected)
{
	return orientation_equals(expected);
}

inline rotation_equals is_approx(eversim::core::utility::math::rotation expected)
{
	return rotation_equals(expected);
}

namespace Catch {
	template<>
	struct StringMaker<eversim::core::utility::math::orientation> {
		static std::string convert(eversim::core::utility::math::orientation const& v)
		{
			return std::to_string(v.as_degrees()) + "°";
		}
	};
	template<>
	struct StringMaker<eversim::core::utility::math::rotation> {
		static std::string convert(eversim::core::utility::math::rotation const& v)
		{
			return std::to_string(v.as_degrees()) + "°";
		}
	};
}
