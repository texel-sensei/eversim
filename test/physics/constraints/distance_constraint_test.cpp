#include "core/physics/body_template.h"
#include <catch.hpp>
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/physics_manager.h"

using namespace std;
using namespace eversim::core::physics;

TEST_CASE("distance constraint loading", "[physics][constraints]")
{
	particle ps[] = {{},{}};
	ps[0].pos = {0.f, 0.f};
	ps[1].pos = {0.f,1.5f};
	body bdy{ps};

	distance_constraint_factory f;
	auto get_f = [&](string const&) -> distance_constraint_factory const& { return f; };
	SECTION("general")
	{
		auto txt = "2 0 1 0.75 distance";
		auto desc = constraint_descriptor::parse(txt, get_f);
		auto c = f.build(desc, &bdy);
		REQUIRE(c->get_arity() == 2);
		REQUIRE(c->stiffness == Approx(0.75f));
		REQUIRE(c->particles[0] == &ps[0]);
		REQUIRE(c->particles[1] == &ps[1]);
	}
	SECTION("calculate distance")
	{
		auto txt = "2 0 1 0.75 distance";
		auto desc = constraint_descriptor::parse(txt, get_f);
		auto c = f.build(desc, &bdy);
		auto& dc = static_cast<distance_constraint&>(*c);
		REQUIRE(dc.get_distance() == Approx(1.5f));
	}
	SECTION("given distance")
	{
		auto txt = "2 0 1 0.75 distance 17.8";
		auto desc = constraint_descriptor::parse(txt, get_f);
		auto c = f.build(desc, &bdy);
		auto& dc = static_cast<distance_constraint&>(*c);
		REQUIRE(dc.get_distance() == Approx(17.8f));
	}
}
