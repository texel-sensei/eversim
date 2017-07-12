#include "core/physics/body_template.h"
#include <catch.hpp>

using namespace std;
using namespace eversim::core::physics;

TEST_CASE("particle parser positive", "[physics][body_template_loader]")
{
	SECTION("simple") {
		auto good = "0.75 1.0";
		auto desc = particle_descriptor::parse(good);
		REQUIRE(desc.pos.x == Approx(0.75f));
		REQUIRE(desc.pos.y == Approx(1.00f));
	}
	SECTION("trailing whitespace") {
		auto good = "-0.75 1.0				";
		auto desc = particle_descriptor::parse(good);
		REQUIRE(desc.pos.x == Approx(-0.75f));
		REQUIRE(desc.pos.y == Approx(1.00f));
	}
	SECTION("trailing comment") {
		auto good = "0.75 -1.0 #This particle is special!@@&&\"stuff";
		auto desc = particle_descriptor::parse(good);
		REQUIRE(desc.pos.x == Approx(0.75f));
		REQUIRE(desc.pos.y == Approx(-1.00f));
	}
}

TEST_CASE("particle parser negative", "[physics][body_template_loader]")
{
	SECTION("empty")
	{
		auto txt = "";
		REQUIRE_THROWS(particle_descriptor::parse(txt));
	}
	SECTION("invalid number")
	{
		auto txt = "0.75 0.1.1";
		REQUIRE_THROWS(particle_descriptor::parse(txt));
	}
	SECTION("too much data")
	{
		auto txt = "0.75 0.1 0.2";
		REQUIRE_THROWS(particle_descriptor::parse(txt));
	}
	SECTION("not enough data")
	{
		auto txt = "0.75";
		REQUIRE_THROWS(particle_descriptor::parse(txt));
	}
}

TEST_CASE("constraint parser positive", "[physics][body_template_loader]")
{
	SECTION("simple") {
		auto good = "2 0 1 0.75 distance";
		auto desc = constraint_descriptor::parse(good);
		REQUIRE(desc.arity == 2);
		REQUIRE(desc.particles.size() == 2);
		REQUIRE(desc.particles[0] == 0);
		REQUIRE(desc.particles[1] == 1);
		REQUIRE(desc.stiffness == Approx(0.75));
		REQUIRE(desc.type == "distance");
	}
	SECTION("trailing whitespace") {
		auto good = "1 7 1.0 floor			  ";
		auto desc = constraint_descriptor::parse(good);
		REQUIRE(desc.arity == 1);
		REQUIRE(desc.particles.size() == 1);
		REQUIRE(desc.particles[0] == 7);
		REQUIRE(desc.stiffness == Approx(1.00f));
		REQUIRE(desc.type == "floor");
	}
	SECTION("trailing comment") {
		auto good = "3 7 12 144 0.01 angle 	# this stuff keeps stuff good";
		auto desc = constraint_descriptor::parse(good);
		REQUIRE(desc.arity == 3);
		REQUIRE(desc.particles.size() == 3);
		REQUIRE(desc.particles[0] == 7);
		REQUIRE(desc.particles[1] == 12);
		REQUIRE(desc.particles[2] == 144);
		REQUIRE(desc.stiffness == Approx(0.01f));
		REQUIRE(desc.type == "angle");
	}
}

TEST_CASE("constraint parser negative", "[physics][body_template_loader]")
{
	SECTION("empty")
	{
		auto txt = "";
		REQUIRE_THROWS(constraint_descriptor::parse(txt));
	}
	SECTION("invalid number")
	{
		auto txt = "1.5 1 0.75 broken";
		REQUIRE_THROWS(constraint_descriptor::parse(txt));
	}
	SECTION("too much indices")
	{
		auto txt = "2 0 1 3 0.75 distance";
		REQUIRE_THROWS(constraint_descriptor::parse(txt));
	}
	SECTION("not enough indices")
	{
		auto txt = "7 0 1 3 0.75 distance";
		REQUIRE_THROWS(constraint_descriptor::parse(txt));
	}
	SECTION("missing type")
	{
		auto txt = "2 0 1 0.75";
		REQUIRE_THROWS(constraint_descriptor::parse(txt));
	}
	SECTION("missing stiffness")
	{
		auto txt = "2 0 1 distance";
		REQUIRE_THROWS(constraint_descriptor::parse(txt));
	}
	SECTION("trailing stuff")
	{
		auto txt = "2 0 1 0.75 distance angle";
		REQUIRE_THROWS(constraint_descriptor::parse(txt));
	}
}

TEST_CASE("full template loader", "[physics][body_template_loader]")
{
	body_template_loader loader;
	SECTION("positive")
	{
		SECTION("simple")
		{
			auto txt = 
			R"(
				2
				0 0
				1 1
				1
				2 0 1 0.75 distance
			)";
			auto data = istringstream(txt);
			auto templ = loader.parse(data);
		}
		SECTION("with whitespace")
		{
			auto txt =
				R"(
				2
				0 0
				1 1

				1
				2 0 1 0.75 distance
			)";
			auto data = istringstream(txt);
			auto templ = loader.parse(data);
		}
		SECTION("with comments")
		{
			auto txt =
				R"(
				# This describes a rigid body
				2 # Number of particles

				0 0
				# This is the second particle
				1 1


				1 # number of constraints

				2 0 1 0.75 distance
			)";
			auto data = istringstream(txt);
			auto templ = loader.parse(data);
		}
	}
	SECTION("negative")
	{
		SECTION("empty")
		{
			auto txt = "";
			auto data = istringstream(txt);
			REQUIRE_THROWS(loader.parse(data));
		}
		SECTION("no particles")
		{
			auto txt =
				R"(
				0
				0
			)";
			auto data = istringstream(txt);
			REQUIRE_THROWS(loader.parse(data));
		}
		SECTION("wrong number particles")
		{
			auto txt =
				R"(
				1
				0 0
				1 1

				1
				2 0 1 0.75 distance
			)";
			auto data = istringstream(txt);
			REQUIRE_THROWS(loader.parse(data));
		}
		SECTION("wrong number constraints")
		{
			auto txt =
				R"(
				2
				0 0
				1 1

				999
				2 0 1 0.75 distance
			)";
			auto data = istringstream(txt);
			REQUIRE_THROWS(loader.parse(data));
		}
		SECTION("constraint index invalid")
		{
			auto txt =
				R"(
				2
				0 0
				1 1

				1
				2 999 1 0.75 distance
			)";
			auto data = istringstream(txt);
			REQUIRE_THROWS(loader.parse(data));
		}
	}
}