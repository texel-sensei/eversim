#include "core/utility/math/angular.h"
#include "test_helper.h"
#include <catch.hpp>

using namespace std;
using namespace std::literals;
using namespace glm;
using namespace eversim::core::utility::math;
using namespace eversim::core::utility::math::literals;

namespace Catch {
	template<>
	struct StringMaker<orientation> {
		static std::string convert(orientation const& v)
		{
			return to_string(v.as_degrees()) + "°";
		}
	};
}

TEST_CASE("angular: construction", "[utility][math][angular]")
{
	SECTION("from float")
	{
		const auto o = orientation::from_radians(0);

		REQUIRE_THAT(o.to_direction(), is_approx(1, 0));

		const auto o2 = orientation::from_degrees(270);
		REQUIRE_THAT(o2.to_direction(), is_approx(0, -1));
	}

	SECTION("from complex")
	{
		const auto o = orientation{ 0. + 1i }; // up on the unit circle == 90 deg

		REQUIRE_THAT(o.to_direction(), is_approx(0, 1));
		REQUIRE(o.as_radians() == Approx(PI / 2));

		const auto o2 = orientation{ 1. + 1i };
		REQUIRE(o2.as_degrees() == Approx(45));
		REQUIRE_THAT(o2.to_direction(), is_approx(normalize(vec2(1, 1))));
	}
}

TEST_CASE("angular: addition/subtraction", "[utility][math][angular]")
{
	SECTION("rotation + orientation")
	{
		const auto o = orientation::from_degrees(45);

		const auto ccw_rotated = o + 45_deg;
		REQUIRE(ccw_rotated.as_degrees() == Approx(90));

		const auto cw_rotated = o - 70_deg;

		REQUIRE(cw_rotated.as_degrees() == Approx(-25));

		REQUIRE(o + 0_deg == o);
		REQUIRE((o + 360_deg).as_degrees() == Approx(o.as_degrees()));
	}

	SECTION("orientation - orientation")
	{
		const auto o1 = orientation::from_degrees(45);
		const auto o2 = orientation::from_degrees(-45);

		const auto r1 = o1 - o2;

		REQUIRE(r1.as_degrees() == Approx(-90));
		REQUIRE(o1 + r1 == o2);

		const auto o3 = orientation::from_degrees(0);
		const auto o4 = orientation::from_degrees(180);

		const auto r2 = o3 - o4;
		REQUIRE(r2.as_degrees() == Approx(-180));
		REQUIRE(o3 + r2 == o4);
	}
}

TEST_CASE("angular: point functions", "[utility][math][angular]")
{
	const auto a = vec2(2, 1);
	const auto b = vec2(3, 2);

	const auto angle = angle_between_points(a, b);
	REQUIRE(angle.as_degrees() == Approx(45));

	const auto r_angle = angle_between_points(b, a);
	REQUIRE(r_angle.as_degrees() == Approx(-135));
}

TEST_CASE("angular: lerp", "[utility][math][angular]")
{
	const auto a = orientation::from_degrees(45);
	const auto b = orientation::from_degrees(315);

	REQUIRE(lerp(a, b, 0) == a);
	REQUIRE(lerp(a, b, 1) == b);
	REQUIRE(lerp(a, b, 0.5).as_degrees() == Approx(0));
}