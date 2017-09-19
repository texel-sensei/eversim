#include "core/utility/math/angular.h"
#include "test_helper.h"
#include <catch.hpp>

using namespace std;
using namespace std::literals;
using namespace glm;
using namespace eversim::core::utility::math;
using namespace eversim::core::utility::math::literals;

TEST_CASE("angular: construction", "[utility][math][angular]")
{
	SECTION("from float")
	{
		const auto o = orientation{ 0 };
		const auto r = o.repr();

		REQUIRE(r.real() == Approx(1));
		REQUIRE(r.imag() == Approx(0));

		REQUIRE_THAT(o.to_direction(), is_approx(1, 0));

		const auto o2 = orientation::from_degrees(270);
		REQUIRE_THAT(o2.to_direction(), is_approx(0, -1));
	}

	SECTION("from complex")
	{
		const auto o = orientation{ 0. + 1i }; // up on the unit circle == 90 deg

		REQUIRE_THAT(o.to_direction(), is_approx(0, 1));
		REQUIRE(o.as_radians() == Approx(PI / 2));

		// should be the same as (1,1)/||(1,1)||
		const auto o2 = orientation{ 1. + 1i };
		REQUIRE(o2.as_degrees() == Approx(45));
		REQUIRE_THAT(o2.to_direction(), is_approx(normalize(vec2(1, 1))));
	}
}

TEST_CASE("angular: addition/subtraction")
{
	SECTION("rotation + orientation")
	{
		const auto o = orientation::from_degrees(45);

		const auto ccw_rotated = o + 45_deg;
		REQUIRE(ccw_rotated.as_degrees() == Approx(90));

		const auto cw_rotated = o - 70_deg;

		REQUIRE(cw_rotated.as_degrees() == Approx(-25));
	}
}