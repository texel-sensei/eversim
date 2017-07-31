#include "core/world/level.h"
#include "test_helper.h"
#include <catch.hpp>

using namespace eversim::core::world;

TEST_CASE("default tiles empty", "[world][level]")
{
	auto l = level({ 5,5 });
	for (auto x = 0; x < l.get_num_tiles().x; ++x)
	{
		for (auto y = 0; y < l.get_num_tiles().y; ++y)
		{
			auto& t = l.get_tile_by_index({ x,y });
			REQUIRE(t.get_descriptor() == &blank_tile);
		}
	}
}

TEST_CASE("tile size", "[world][level]")
{
	auto l = level({ 10,1 });
	REQUIRE(l.get_num_tiles().x == 10);
	REQUIRE(l.get_num_tiles().y == 1);

	l.set_tile_size(0.75f);
	REQUIRE(l.get_tile_size() == Approx(0.75f));
	REQUIRE(l.get_world_size().x == Approx(7.5f));
	REQUIRE(l.get_world_size().y == Approx(0.75f));

	auto& t = l.get_tile_by_index({ 1,0 });
	REQUIRE(t.size() == Approx(0.75f));

	SECTION("inside/outside test")
	{
		const auto p = glm::vec2{ 1,.5f };
		auto& tile = l.get_tile_by_pos(p);
		REQUIRE(tile.point_inside(p));
		REQUIRE(!tile.point_inside(p + glm::vec2(3, 3)));
		REQUIRE(!tile.point_inside(p - glm::vec2(3, 3)));
	}
}

TEST_CASE("index / position conversion", "[world][level]")
{
	auto l = level({ 12,17 });
	
	l.set_tile_size(2.f);
	auto idx = l.index_for_pos({1.5f, 2.5f});
	REQUIRE(idx.x == 0);
	REQUIRE(idx.y == 1);
	auto pos = l.center_of_tile({0,1});
	REQUIRE(pos.x == Approx(1.f));
	REQUIRE(pos.y == Approx(3.f));

	auto& t = l.get_tile_by_pos(pos);
	REQUIRE(t.x() == Approx(pos.x));
	REQUIRE(t.y() == Approx(pos.y));
}

TEST_CASE("tile neighbours", "[world][level]")
{
	auto l = level({ 12,17 });
	auto const& t = l.get_tile_by_index({ 4,4 });

	auto const& n = t.get_neighbour({ 1,0 });
	REQUIRE(n.index() == glm::ivec2(5, 4));
	REQUIRE(&n.get_neighbour({ -1,0 }) == &t);
}

TEST_CASE("tile positions", "[world][level]")
{
	auto l = level({ 5,5 });
	l.set_tile_size(1.15f);

	for(auto x = 0; x < l.get_num_tiles().x; ++x)
	{
		for (auto y = 0; y < l.get_num_tiles().y; ++y)
		{
			auto& t = l.get_tile_by_index({ x,y });
			auto center = l.center_of_tile({ x,y });

			REQUIRE(t.index().x == x);
			REQUIRE(t.index().y == y);

			REQUIRE(t.x() == Approx(center.x));
			REQUIRE(t.y() == Approx(center.y));
		}
	}
}