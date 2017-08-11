#include "core/system/game.h"
#include "test_helper.h"
#include <catch.hpp>
#include <SDL2/SDL_stdinc.h>

using namespace eversim::core::system;

using namespace std;
using namespace chrono_literals;

TEST_CASE("Transform parameters", "[system][gameobject]")
{
	game g;
	auto obj = g.create_empty();

	SECTION("default constructed") {
		REQUIRE_THAT(obj->get_position(), is_approx(0, 0));
		REQUIRE_THAT(obj->get_scale(), is_approx(1, 1));
		REQUIRE(obj->get_angle() == Approx(0.f));
	}

	SECTION("Set/get")
	{
		obj->set_scale({ 7.f,0.3f });
		obj->set_angle(M_PI / 2);
		obj->set_position({ 1.f,2.f });

		REQUIRE_THAT(obj->get_position(), is_approx(1, 2));
		REQUIRE_THAT(obj->get_scale(), is_approx(7, .3f));
		REQUIRE(obj->get_angle() == Approx(M_PI/2));
	}
	
}

TEST_CASE("Object cloning", "[system][gameobject]")
{
	game g;
	auto obj = g.create_empty();

	SECTION("Check transform")
	{
		obj->set_scale({ 7.f,0.3f });
		obj->set_angle(M_PI / 2);
		obj->set_position({ 1.f,2.f });

		auto child = obj->clone();

		REQUIRE_THAT(child->get_position(), is_approx(1, 2));
		REQUIRE_THAT(child->get_scale(), is_approx(7, .3f));
		REQUIRE(child->get_angle() == Approx(M_PI / 2));
	}
}