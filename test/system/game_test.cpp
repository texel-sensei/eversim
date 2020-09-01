#include "core/system/game.h"
#include <catch2/catch.hpp>

using namespace eversim::core::system;

using namespace std;
using namespace chrono_literals;

TEST_CASE("simple object creation and deletion", "[system][game]")
{
	game g;
	SECTION("default game empty"){
		REQUIRE(g.get_num_gameobjects() == 0);
	}

	auto obj = g.create_empty();
	SECTION("simple creation"){
		REQUIRE(obj != nullptr);
		REQUIRE(g.get_num_gameobjects() == 1);
	}

	SECTION("clone"){
		auto clone = obj->clone();
		REQUIRE(clone != nullptr);
		REQUIRE(g.get_num_gameobjects() == 2);
	}
	SECTION("simple deletion")
	{
		obj->kill();

		REQUIRE(g.get_num_gameobjects() == 1); // kill shouldn't directly delete the object
		g.cleanup_dead();
		REQUIRE(g.get_num_gameobjects() == 0);
	}
}

TEST_CASE("complex object creation and deletion", "[system][game]")
{
	game g;
	auto obj = g.create_empty();
	auto child = g.create_empty();
	child->set_parent(obj);
	child->add_child(g.create_empty());
	obj->add_child(g.create_empty());
	REQUIRE(g.get_num_gameobjects() == 4);

	SECTION("complex clone")
	{
		obj->clone();
		REQUIRE(g.get_num_gameobjects() == 8);
	}

	SECTION("complex deletion")
	{
		obj->kill();

		REQUIRE(g.get_num_gameobjects() == 4); // kill shouldn't directly delete the object
		g.cleanup_dead();
		REQUIRE(g.get_num_gameobjects() == 0);
	}
}
