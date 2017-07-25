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