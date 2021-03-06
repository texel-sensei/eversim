#include "core/utility/math.h"
#include "test_helper.h"
#include <catch.hpp>

using namespace eversim::core::utility;

TEST_CASE("line", "[utility][math]")
{
	const auto l = line{ {},{ 1.f,1.f } };
	SECTION("length")
	{
		REQUIRE(l.length() == Approx(sqrt(2.0)));
	}
	SECTION("normal")
	{
		const auto n = l.normal();
		REQUIRE(length(n) == Approx(1.f));
		REQUIRE(dot(n, l.direction()) == Approx(0.f));
	}
	SECTION("direction")
	{
		const auto dir = l.direction();
		REQUIRE(length(dir) == Approx(1.f));
		REQUIRE(dir.x == Approx(dir.y));

		const auto l2 = line{ {},{0.f,1.f} };
		const auto dir2 = l2.direction();

		REQUIRE_THAT(dir2, is_approx(0,1));
	}

	SECTION("lerp")
	{
		const auto start = l.lerp(0);
		REQUIRE_THAT(start, is_approx(l.start));

		const auto end = l.lerp(1);
		REQUIRE_THAT(end, is_approx(l.end));

		const auto center = l.lerp(0.5);
		REQUIRE_THAT(center, is_approx(0.5f, 0.5f));

		const auto outside = l.lerp(1.5);
		REQUIRE_THAT(outside, is_approx(1.5f, 1.5f));
	}

	SECTION("closest point")
	{
		REQUIRE_THAT(l.closest_point(l.start), is_approx(l.start));
		REQUIRE_THAT(l.closest_point(l.end), is_approx(l.end));

		const auto center = l.lerp(0.5);
		REQUIRE_THAT(l.closest_point(center), is_approx(center));

		const auto outside = center + 0.8f*l.normal();
		REQUIRE_THAT(l.closest_point(outside), is_approx(center));
	}

	SECTION("distance to point")
	{
		REQUIRE(l.distance_to_point(l.start) == Approx(0.f));
		REQUIRE(l.distance_to_point(l.lerp(0.5)) == Approx(0.f));
		REQUIRE(l.distance_to_point(l.end) == Approx(0.f));

		REQUIRE(l.distance_to_point(l.end + glm::vec2{0.5f,0.f}) == Approx(0.5f));

		const auto p2 = l.start + 0.4f * l.normal();
		REQUIRE(l.distance_to_point(p2) == Approx(0.4f));
	}
}

TEST_CASE("line intersections", "[utility][math]")
{
	const auto X = line{ { -1.f,0.f }, { 1.f, 0.f } };
	const auto Y = line{ { 0.f,-1.f },{ 0.f, 1.f } };
	SECTION("no intersection")
	{
		const auto other = line{{1.f,1.f},{2.f,2.f}};
		REQUIRE(!X.intersect(other));
	}
	SECTION("parallel lines")
	{
		const auto offset = glm::vec2{ 0.f,1.f };
		const auto other = line{ X.start + offset, X.end + offset };
		REQUIRE(!X.intersect(other));
	}
	SECTION("simple intersection")
	{
		const auto opt_t = X.intersect(Y);
		REQUIRE(opt_t.is_initialized());
		REQUIRE(*opt_t == Approx(0.5));
		REQUIRE_THAT(X.lerp(*opt_t), is_approx(0, 0));
	}
}