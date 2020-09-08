#include "core/physics/physics_manager.h"
#include <catch2/catch.hpp>
#include "core/physics/body_template.h"
#include <SDL2/SDL_stdinc.h>
#include <random>

using namespace std;
using namespace glm;
using namespace eversim::core::physics;

namespace {
	class mock_constraint : public constraint {
		bool* constructed;
	public:
		explicit mock_constraint(size_t N, bool* ptr = nullptr)
			: constraint(N), constructed(ptr)
		{
			if (constructed)
				*constructed = true;
		}

		~mock_constraint()
		{
			if (constructed)
				*constructed = false;
		}

		float operator()() const override { return 0.f; }
		void grad(eversim::core::utility::array_view<glm::vec2>) const override {}
	};

	class mock_constraint_factory : public constraint_factory {
	public:
		data_container parse(istream&) const override { return {}; }

		unique_ptr<constraint> build(constraint_descriptor const& d, body const* b) const override
		{
			bool* ptr = nullptr;
			if (!d.extra_data.empty())
				ptr = boost::any_cast<bool*>(d.extra_data);
			auto c = make_unique<mock_constraint>(d.arity);
			for (int i = 0; i < d.arity; ++i)
			{
				c->particles[i] = {b, d.particles[i]};
			}
			return c;
		}
	};

	class physics_test_fixture {
	public:
		physics_test_fixture()
		{
			loader.register_factory("mock", make_unique<mock_constraint_factory>());
			man.register_constraint_types<mock_constraint>();
			single_particle.particles = {{}};

			auto N = 25;
			auto delta = float(2 * M_PI / (N + 1));
			boulder.particles.resize(N);
			for(int i = 0; i < N; ++i)
			{
				auto& p = boulder.particles[i];
				p.pos = { cosf(i*delta), sinf(i*delta) };
			}
		}

	protected:
		physics_manager man;
		body_template_loader loader;
		body_template single_particle;
		body_template boulder;
		body_template rand_body;

		void check_particle_pointer_integrity()
		{
			for(auto const& p : man.get_particles())
			{
				if (!p.is_alive()) continue;
				auto owner = p.owner;
				const auto begin_ptr = owner->get_particles().data();
				const auto end_ptr = begin_ptr + owner->get_particles().size();
				REQUIRE(begin_ptr <= &p);
				REQUIRE(&p < end_ptr);
			}
		}
	};
}

TEST_CASE_METHOD(physics_test_fixture,"velocity", "[physics][physics_manager]")
{
	auto* b = man.add_body(single_particle, {});
	b->velocity = { 1.f,0.f };
	b->gravity_scale = 0;
	man.set_quadratic_drag(0);


	SECTION("movement without damping"){
		man.set_damping_coefficient(0);
		REQUIRE(b->position.x == Approx(0.f));
		REQUIRE(b->position.y == Approx(0.f));
		man.integrate(1);
		REQUIRE(b->position.x == Approx(1.f));
		REQUIRE(b->position.y == Approx(0.f));
		man.integrate(1);
		REQUIRE(b->position.x == Approx(2.f));
		REQUIRE(b->position.y == Approx(0.f));
	}

	SECTION("fully damped movement")
	{
		man.set_damping_coefficient(1);
		REQUIRE(b->position.x == Approx(0.f));
		REQUIRE(b->position.y == Approx(0.f));
		man.integrate(1);
		REQUIRE(b->position.x == Approx(0.f));
		REQUIRE(b->position.y == Approx(0.f));
	}

	SECTION("some damping")
	{
		man.set_damping_coefficient(0.5);
		REQUIRE(b->position.x == Approx(0.f));
		REQUIRE(b->position.y == Approx(0.f));
		man.integrate(1);
		REQUIRE(b->position.x > 0.f);
		REQUIRE(b->position.x < 1.f);
		REQUIRE(b->position.y == Approx(0.f));
	}
}

TEST_CASE_METHOD(physics_test_fixture, "gravity","[physics][physics_manager]")
{
	man.set_damping_coefficient(0);
	man.set_quadratic_drag(0);
	SECTION("change gravity"){
		const auto g = vec2(0, -10);
		man.set_gravity(g);
		REQUIRE(man.get_gravity() == g);
	}
	SECTION("object falls")
	{
		REQUIRE(man.get_gravity().y < 0);
		auto* b = man.add_body(single_particle,{});
		auto old_pos = b->position;
		man.integrate(1);
		auto new_pos = b->position;
		REQUIRE(new_pos.y < old_pos.y);
	}
	SECTION("gravity scale")
	{
		man.set_gravity({ 0.f,-1.f });

		auto* b1 = man.add_body(single_particle, {-1.f,0});
		REQUIRE(b1->gravity_scale == Approx(1.f));

		auto* b2 = man.add_body(single_particle, {1.f, 0});
		b2->gravity_scale = 0;

		auto* b3 = man.add_body(single_particle, {2.f, 0 });
		b3->gravity_scale = 2;

		man.integrate(1);

		REQUIRE(b1->position.y == Approx(-1.f));
		REQUIRE(b2->position.y == Approx(0.f));
		REQUIRE(b3->position.y == Approx(-2.f));
	}
}

TEST_CASE_METHOD(physics_test_fixture, "body management", "[physics][physics_manager]")
{
	check_particle_pointer_integrity();
	SECTION("single body")
	{
		auto* b = man.add_body(boulder, {});
		REQUIRE(man.get_num_bodies() == 1);
		REQUIRE(man.get_particles().size() == boulder.particles.size());
		for(auto const& p : man.get_particles())
		{
			REQUIRE(p.owner == b);
		}
		SECTION("add"){
			check_particle_pointer_integrity();
			man.integrate(1.f);
		}
		SECTION("remove")
		{
			REQUIRE(man.get_num_bodies() == 1);
			auto const& ps = b->get_particles();
			man.remove_body(b);
			REQUIRE(man.get_num_bodies() == 0);
			for(auto const& _ : man.get_bodies())
			{
				FAIL("Found body where nobody was expected! " << &_);
			}
			for(auto const& p : ps)
			{
				REQUIRE(!p.is_alive());
			}
			man.cleanup_dead_bodies();
			man.integrate(1.f);
		}
	}
	SECTION("multiple bodies")
	{
		const int N = 20;

		mt19937 rng{42}; // the one and only seed

		const auto insert_random = [&] {
			auto dist = uniform_int_distribution<>(3, 36);
			rand_body.particles.resize(dist(rng));
			return man.add_body(rand_body, {});
		};

		for(int i = 0; i < N; ++i) {
			auto old_num_particles = man.get_particles().size();
			auto b = insert_random();
			REQUIRE(man.get_num_bodies() == i+1);
			REQUIRE(man.get_particles().size() == old_num_particles + b->get_particles().size());
		}
		SECTION("insert only") {
			check_particle_pointer_integrity();
		}

		const auto remove_random = [&] {
			auto dist = uniform_int_distribution<>(0, man.get_num_bodies()-1);
			auto it = man.get_bodies().begin();
			advance(it, dist(rng));
			man.remove_body(&*it);
		};

		SECTION("remove only")
		{
			REQUIRE(man.get_num_bodies() == N);

			while(man.get_num_bodies() > 0)
			{
				auto oldsize = man.get_num_bodies();
				remove_random();
				man.cleanup_dead_bodies();
				REQUIRE(man.get_num_bodies() == oldsize - 1);
			}
			check_particle_pointer_integrity();
			man.integrate(1);
			check_particle_pointer_integrity();
		}

		SECTION("insert & remove")
		{
			REQUIRE(man.get_num_bodies() == N);
			auto coinflip = bernoulli_distribution(0.5);

			for(auto& b : man.get_bodies())
			{
				man.remove_body(&b);
			}

			for(int i = 0; i < N; i++)
			{
				insert_random();
			}

			for(int i = 0; i < N; ++i)
			{
				auto remove = coinflip(rng);
				auto old_num_bodies = man.get_num_bodies();
				if(remove)
				{
					remove_random();
					REQUIRE(man.get_num_bodies() == old_num_bodies - 1);
				}
				else
				{
					insert_random();
					REQUIRE(man.get_num_bodies() == old_num_bodies + 1);
				}
				if(coinflip(rng))
				{
					man.cleanup_dead_bodies();
				}
				man.integrate(1.f);
			}
			check_particle_pointer_integrity();
		}
	}
}
