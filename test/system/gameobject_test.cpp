#include "core/system/game.h"
#include "core/system/component.h"

#include "test_helper.h"
#include <catch.hpp>
#include <SDL2/SDL_stdinc.h>

using namespace eversim::core::system;
using namespace eversim::core::utility::math;

using namespace std;
using namespace chrono_literals;

namespace {
	class MockComponent : public component {
		using component::component;
	protected:
		unique_ptr<component> do_clone() const override { return make_unique<MockComponent>(*this); }
	};
}

TEST_CASE("Transform parameters", "[system][gameobject]")
{
	game g;
	auto obj = g.create_empty();

	SECTION("default constructed") {
		REQUIRE_THAT(obj->get_position(), is_approx(0, 0));
		REQUIRE_THAT(obj->get_scale(), is_approx(1, 1));
		REQUIRE_THAT(obj->get_angle(), is_approx(orientation::from_degrees(0)));
	}

	SECTION("Set/get")
	{
		const auto pi_over_two = orientation::from_radians(M_PI / 2);
		obj->set_scale({ 7.f,0.3f });
		obj->set_angle(pi_over_two);
		obj->set_position({ 1.f,2.f });

		REQUIRE_THAT(obj->get_position(), is_approx(1, 2));
		REQUIRE_THAT(obj->get_scale(), is_approx(7, .3f));
		REQUIRE_THAT(obj->get_angle(), is_approx(pi_over_two));
	}
	
}

TEST_CASE("Add component", "[system][gameobject]")
{
	game g;
	auto obj = g.create_empty();

	REQUIRE(obj->get_num_components() == 0);

	SECTION("Add component adds the component")
	{
		auto comp = obj->add_component<MockComponent>();
		REQUIRE(obj->get_num_components() == 1);
		REQUIRE(obj->has_component<MockComponent>());
		REQUIRE(obj->get_component(typeid(MockComponent)) == comp);
	}

	SECTION("Add nullptr fails")
	{
		REQUIRE_THROWS(obj->add_component(nullptr));
	}

	SECTION("Add duplicate fails")
	{
		REQUIRE_NOTHROW(obj->add_component<MockComponent>());
		REQUIRE_THROWS(obj->add_component<MockComponent>());
		REQUIRE_THROWS(obj->add_component(obj->get_component<MockComponent>()->clone()));
	}
}

TEST_CASE("Object cloning", "[system][gameobject]")
{
	game g;
	auto obj = g.create_empty();

	SECTION("Check transform")
	{
		obj->set_scale({ 7.f,0.3f });
		const auto pi_over_two = orientation::from_radians(M_PI / 2);
		obj->set_angle(pi_over_two);
		obj->set_position({ 1.f,2.f });

		auto child = obj->clone();

		REQUIRE_THAT(child->get_position(), is_approx(1, 2));
		REQUIRE_THAT(child->get_scale(), is_approx(7, .3f));
		REQUIRE_THAT(child->get_angle(), is_approx(pi_over_two));
	}

	SECTION("Check components")
	{
		obj->add_component<MockComponent>();

		auto child = obj->clone();
		REQUIRE(child->get_num_components() == 1);
		REQUIRE(child->has_component<MockComponent>());
	}
}