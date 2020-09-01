#include "core/physics/body_template.h"
#include <catch2/catch.hpp>
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/physics_manager.h"

using namespace std;
using namespace eversim::core::physics;

TEST_CASE("particle parser positive", "[physics][body_template_loader]")
{
	SECTION("simple") {
		auto good = "0.75 1.0 2.5";
		auto desc = particle_descriptor::parse(good);
		REQUIRE(desc.pos.x == Approx(0.75f));
		REQUIRE(desc.pos.y == Approx(1.00f));
	}
	SECTION("trailing whitespace") {
		auto good = "-0.75 1.0	7.3			";
		auto desc = particle_descriptor::parse(good);
		REQUIRE(desc.pos.x == Approx(-0.75f));
		REQUIRE(desc.pos.y == Approx(1.00f));
	}
	SECTION("trailing comment") {
		auto good = "0.75 -1.0  3.14 #This particle is special!@@&&\"stuff";
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
		auto txt = "0.75 0.a 7.4";
		REQUIRE_THROWS(particle_descriptor::parse(txt));
	}
	SECTION("too much data")
	{
		auto txt = "0.75 0.1 0.2 0.4";
		REQUIRE_THROWS(particle_descriptor::parse(txt));
	}
	SECTION("not enough data")
	{
		auto txt = "0.75";
		REQUIRE_THROWS(particle_descriptor::parse(txt));
	}
}

namespace {
	class mock_constraint_factory : public constraint_factory {
	public:
		data_container parse(std::istream&) const override { return {}; }
		std::unique_ptr<constraint> build(constraint_descriptor const&, body const*) const override { return {}; }
	};

	constraint_factory const& get_mock(string const&)
	{
		static mock_constraint_factory f;
		return f;
	}
}

TEST_CASE("constraint parser positive", "[physics][body_template_loader]")
{
	SECTION("simple") {
		auto good = "2 0 1 0.75 distance";
		auto desc = constraint_descriptor::parse(good, get_mock);
		REQUIRE(desc.arity == 2);
		REQUIRE(desc.particles.size() == 2);
		REQUIRE(desc.particles[0] == 0);
		REQUIRE(desc.particles[1] == 1);
		REQUIRE(desc.stiffness == Approx(0.75));
		REQUIRE(desc.type == "distance");
		REQUIRE(desc.factory == &get_mock(""));
	}
	SECTION("trailing whitespace") {
		auto good = "1 7 1.0 floor			  ";
		auto desc = constraint_descriptor::parse(good, get_mock);
		REQUIRE(desc.arity == 1);
		REQUIRE(desc.particles.size() == 1);
		REQUIRE(desc.particles[0] == 7);
		REQUIRE(desc.stiffness == Approx(1.00f));
		REQUIRE(desc.type == "floor");
	}
	SECTION("trailing comment") {
		auto good = "3 7 12 144 0.01 angle 	# this stuff keeps stuff good";
		auto desc = constraint_descriptor::parse(good, get_mock);
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
		REQUIRE_THROWS(constraint_descriptor::parse(txt, get_mock));
	}
	SECTION("invalid number")
	{
		auto txt = "1.5 1 0.75 broken";
		REQUIRE_THROWS(constraint_descriptor::parse(txt, get_mock));
	}
	SECTION("too much indices")
	{
		auto txt = "2 0 1 3 0.75 distance";
		REQUIRE_THROWS(constraint_descriptor::parse(txt, get_mock));
	}
	SECTION("not enough indices")
	{
		auto txt = "7 0 1 3 0.75 distance";
		REQUIRE_THROWS(constraint_descriptor::parse(txt, get_mock));
	}
	SECTION("missing type")
	{
		auto txt = "2 0 1 0.75";
		REQUIRE_THROWS(constraint_descriptor::parse(txt, get_mock));
	}
	SECTION("missing stiffness")
	{
		auto txt = "2 0 1 distance";
		REQUIRE_THROWS(constraint_descriptor::parse(txt, get_mock));
	}
	SECTION("trailing stuff")
	{
		auto txt = "2 0 1 0.75 distance angle";
		REQUIRE_THROWS(constraint_descriptor::parse(txt, get_mock));
	}
}

TEST_CASE("full template loader", "[physics][body_template_loader]")
{
	body_template_loader loader;
	loader.register_factory("distance", make_unique<distance_constraint_factory>());
	SECTION("positive")
	{
		SECTION("simple")
		{
			auto txt =
			R"(
				2
				0 0 1.0
				1 1 2.0
				1
				2 0 1 0.75 distance
			)";
			auto data = istringstream(txt);
			auto templ = loader.parse(data);
			REQUIRE(templ.particles.size() == 2);
			REQUIRE(templ.constraints.size() == 1);
		}
		SECTION("extra data")
		{
			auto txt =
				R"(
				2
				0 0 1.0
				1 1 2.0
				1
				2 0 1 0.75 distance 17.8
			)";
			auto data = istringstream(txt);
			auto templ = loader.parse(data);
			auto fl = boost::any_cast<float>(templ.constraints[0].extra_data);

			REQUIRE(fl == Approx(17.8));
		}
		SECTION("with whitespace")
		{
			auto txt =
				R"(
				2
				0 0 1.0
				1 1 2.0

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

				0 0 1.0
				# This is the second particle
				1 1 2.0


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
				0 0 1.0
				1 1 2.0

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
				0 0 1.0
				1 1 2.0

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
				0 0 1.0
				1 1 2.0

				1
				2 999 1 0.75 distance
			)";
			auto data = istringstream(txt);
			REQUIRE_THROWS(loader.parse(data));
		}
	}
}
